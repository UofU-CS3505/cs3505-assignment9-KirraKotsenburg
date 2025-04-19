#include "worldrenderer.h"
#include "mainwindow.h"
#include <QPainterPath>
#include <QFont>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QPointer>

WorldRenderer::WorldRenderer(QWidget *parent)
    : QWidget(parent)
    , m_physicsWorld(new PhysicsWorld(15)) // Limit to 15 hazards to prevent Box2D issues
    , m_scale(50.0f) // 50 pixels per meter
{
    // Initialize GameManager
    m_gameManager = new GameManager(this);

    // Register custom contact listener to handle game logic on collisions
    m_contactListener = new GameContactListener(m_gameManager, m_physicsWorld);
    m_physicsWorld->GetWorld().SetContactListener(m_contactListener);

    // Configure and start a timer to refresh screen at ~60 FPS
    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, &WorldRenderer::updateGameState);

    m_timer->start(16);

    // Allow keyboard focus for input handling
    setFocusPolicy(Qt::StrongFocus);
    connect(m_contactListener, &GameContactListener::plantContact, this, &WorldRenderer::showPlantPopup);
}

WorldRenderer::~WorldRenderer()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
    delete m_physicsWorld;
}

void WorldRenderer::paintEvent(QPaintEvent *event)
{

    auto drawHouse = [&](QPainter& painter, const QPointF& screenPos) {
        painter.save();

        const float scaleFactor = 3.0f;
        const float width = m_scale * scaleFactor;
        const float height = m_scale * scaleFactor;
        const float roofHeight = 0.7f * m_scale * scaleFactor;

        QPointF adjustedPos = screenPos + QPointF(0, -height + m_scale * 0.85f);

        painter.translate(adjustedPos);

        // Draw base (square part)
        QRectF base(-width / 2, 0, width, height);
        painter.setBrush(Qt::gray);
        painter.setPen(Qt::white);
        painter.drawRect(base);

        // Draw roof (triangle)
        QPolygonF roof;
        roof << QPointF(-width / 2 - 15, 0)
             << QPointF(width / 2 + 15, 0)
             << QPointF(0, -roofHeight);
        painter.setBrush(Qt::darkRed);
        painter.drawPolygon(roof);

        painter.restore();
    };



    Q_UNUSED(event);
    QPainter painter(this);

    // Clear screen with black background
    painter.fillRect(rect(), Qt::black);

    // Set white stroke and no fill for wireframe drawing
    painter.setPen(Qt::white);
    painter.setBrush(Qt::NoBrush);

    // Camera follows the vehicle's chassis
    Vehicle *vehicle = m_physicsWorld->GetVehicle();
    b2Body *chassis = vehicle->GetChassis();
    b2Vec2 camCenter = chassis->GetPosition();

    auto worldToScreenCamera = [this, camCenter](const b2Vec2 &worldPos) -> QPointF {
        return QPointF((worldPos.x - camCenter.x) * m_scale + width() / 2,
                       height() / 2 - (worldPos.y - camCenter.y) * m_scale);
    };

    // --- Draw Road ---
    QPainterPath roadPath;

    // Get the list of bodies in the Box2D world
    const auto& roadBody = m_physicsWorld->GetWorld().GetBodyList();

    // Loop through all bodies in the world
    for (b2Body* body = roadBody; body; body = body->GetNext()) {

        // Loop through all fixtures of the body
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {

            // Check if the fixture is a chain shape (used for the road)
            if (f->GetType() == b2Shape::e_chain) {
                b2ChainShape* chain = static_cast<b2ChainShape*>(f->GetShape());

                // If the chain has points, start drawing
                if (chain->m_count > 0) {
                    // Move to the first point of the chain
                    roadPath.moveTo(worldToScreenCamera(chain->m_vertices[0]));

                    // Connect lines to each subsequent point
                    for (int i = 1; i < chain->m_count; ++i) {
                        roadPath.lineTo(worldToScreenCamera(chain->m_vertices[i]));
                    }
                }
            }
        }
    }

    // Draw the entire road path using the painter
    painter.drawPath(roadPath);

    float startX = 0.0f;
    float endX = 990.0f;       
    float wallY = -1.0f;       

    QPointF leftHousePos = worldToScreenCamera(b2Vec2(startX, wallY));
    QPointF rightHousePos = worldToScreenCamera(b2Vec2(endX, wallY));

    drawHouse(painter, leftHousePos);
    drawHouse(painter, rightHousePos);


    // --- Draw Vehicle ---
    // Chassis
    QPointF chassisScreenPos = worldToScreenCamera(chassis->GetPosition());
    painter.save();
    painter.translate(chassisScreenPos);
    painter.rotate(chassis->GetAngle() * 180.0f / b2_pi);
    QRectF chassisRect(-0.5, -0.25, 1.0, 0.5);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::white);
    painter.scale(m_scale, m_scale);
    painter.drawRect(chassisRect);
    painter.restore();

    // Wheels
    for (int i = 0; i < 2; ++i) {
        b2Body *wheel = vehicle->GetWheel(i);
        QPointF wheelScreenPos = worldToScreenCamera(wheel->GetPosition());
        painter.save();
        painter.translate(wheelScreenPos);
        painter.rotate(wheel->GetAngle() * 180.0f / b2_pi);
        QRectF wheelRect(-0.25, -0.25, 0.5, 0.5);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::white, 1.0));
        painter.scale(m_scale, m_scale);
        painter.drawEllipse(wheelRect);
        painter.restore();
    }

    // --- Draw Hazards (Poisonous Plants) ---
    const auto &hazards = m_physicsWorld->getHazards();
    for (auto hazard : hazards) {
        b2Body *body = hazard->getBody();
        b2Vec2 pos = body->GetPosition();
        float angle = body->GetAngle();
        QPointF screenPos = worldToScreenCamera(pos);

        painter.save();
        painter.translate(screenPos);
        painter.rotate(angle * 180.0f / b2_pi);
        float r = 0.5f; // Fixed hazard radius
        painter.scale(m_scale, m_scale);
        painter.drawEllipse(QRectF(-r, -r, 2*r, 2*r));
        painter.restore();
    }

    // --- Draw HUD (Health and Score) ---
    painter.resetTransform();
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(14);
    painter.setFont(font);
    painter.drawText(10, 20, QString("Health: %1").arg(m_gameManager->health()));
    painter.drawText(10, 40, QString("Score: %1").arg(m_gameManager->score()));

    // Warning message if health is low
    if (m_gameManager->health() < 50) {
        painter.setPen(Qt::red);
        //painter.drawText(width() / 2 - 50, 50, "Warning: Poisonous Plant!");
    }

    painter.end();

}

// --- Handle Key Presses ---
void WorldRenderer::keyPressEvent(QKeyEvent *event)
{
    Vehicle *vehicle = m_physicsWorld->GetVehicle();

    switch (event->key()) {
    case Qt::Key_Up:
        vehicle->ApplyDriveForce(100.0f);
        break;
    case Qt::Key_Down:
        vehicle->ApplyDriveForce(-100.0f);
        break;
    case Qt::Key_Left:
        vehicle->ApplyDriveForce(-100.0f);
        break;
    case Qt::Key_Right:
        vehicle->ApplyDriveForce(100.0f);
        break;
    }
}

// --- Handle Key Releases ---
void WorldRenderer::keyReleaseEvent(QKeyEvent *event)
{
    Vehicle *vehicle = m_physicsWorld->GetVehicle();

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
        vehicle->ApplySteering(0.0f); // Reset steering
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
        vehicle->ApplyDriveForce(0.0f); // Stop driving
        break;
    }
}

// Convert world coordinates to screen coordinates
QPointF WorldRenderer::worldToScreen(float x, float y)
{
    return QPointF(x * m_scale + width() / 2.0f, height() / 2.0f - y * m_scale);
}

QPointF WorldRenderer::worldToScreen(const b2Vec2 &position)
{
    return worldToScreen(position.x, position.y);
}


void WorldRenderer::resetGame()
{
    // Pause game first to prevent issues
    pauseGame();

    // Create a brand new physics world to avoid issues with reusing the old one
    delete m_physicsWorld;
    m_physicsWorld = new PhysicsWorld(15);
    m_contactListener = new GameContactListener(m_gameManager, m_physicsWorld);
    // Set up contact listener again
    m_physicsWorld->GetWorld().SetContactListener(m_contactListener);

    connect(m_contactListener, &GameContactListener::plantContact, this, &WorldRenderer::showPlantPopup);
    // Reset game state
    if (m_gameManager) {
        m_gameManager->startGame();
    }

    // Resume game
    resumeGame();
}

void WorldRenderer::resumeGame()
{
    if (m_timer && !m_timer->isActive()) {
        m_timer->start(16);
    }
}

void WorldRenderer::pauseGame()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

void WorldRenderer::updateGameState()
{
    if (m_gameManager->gameState() == Playing) {
        try {
            // Process physics updates
            m_physicsWorld->Step();

            // Process hazard removals after Step() completes
            m_physicsWorld->ProcessRemovalQueue();

            // Update game logic
            m_gameManager->update();
        }
        catch (...) {
            // If an exception occurs, safely pause the game
            pauseGame();
            // Could display an error message here
        }
    }

    // Always request a repaint
    update();
}

// worldrenderer.cpp - modify showPlantPopup
// Modified showPlantPopup to show description in the same dialog
void WorldRenderer::showPlantPopup(Hazard* hazard) {
    QDialog dialog;
    qDebug() << "Popup slot triggered!";
    dialog.setWindowTitle("Plant Found: " + hazard->plantName());

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // Image
    QPixmap image(hazard->imagePath());
    if (image.isNull()) {
        qDebug() << "Failed to load image from path:" << hazard->imagePath();
    } else {
        QLabel* imageLabel = new QLabel();
        image = image.scaled(480, 480, Qt::KeepAspectRatio);
        imageLabel->setPixmap(image);
        imageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(imageLabel);
    }

    // Plant Name
    QLabel* nameLabel = new QLabel(hazard->plantName());
    nameLabel->setAlignment(Qt::AlignCenter);
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(16);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    layout->addWidget(nameLabel);

    // Plant Type Indicator
    QString typeText = hazard->type() == "herb" ?
                           "This appears to be a beneficial herb!" :
                           "Warning: This may be poisonous!";

    QLabel* typeLabel = new QLabel(typeText);
    typeLabel->setAlignment(Qt::AlignCenter);
    typeLabel->setStyleSheet(hazard->type() == "herb" ?
                                 "color: green; font-weight: bold;" :
                                 "color: red; font-weight: bold;");
    layout->addWidget(typeLabel);

    // Description Label (always visible now)
    QLabel* descriptionLabel = new QLabel(hazard->description());
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setWordWrap(true);
    // descriptionLabel->setStyleSheet("margin: 10px; padding: 10px; background-color: #f0f0f0; border-radius: 5px;");
    layout->addWidget(descriptionLabel);

    // Text
    QLabel* textLabel = new QLabel("Do you want to pick this plant?");
    textLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(textLabel);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* yesButton = new QPushButton("Yes");
    QPushButton* noButton = new QPushButton("No");
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
    layout->addLayout(buttonLayout);

    QPointer<QDialog> dialogPtr = &dialog;

    QObject::connect(yesButton, &QPushButton::clicked, this, [dialogPtr]() {
        if(dialogPtr) dialogPtr->accept();
    });

    QObject::connect(noButton, &QPushButton::clicked, this, [dialogPtr]() {
        if(dialogPtr) dialogPtr->reject();
    });

    pauseGame(); // Pause the game while showing the popup
    int ret = dialog.exec();
    resumeGame(); // Resume the game after the popup is closed

    if (ret == QDialog::Accepted) {
        // Player chose to pick the plant
        // No separate description box needed here

        // Apply game effects based on plant type
        if (hazard->type() == "herb") {
            // Beneficial herb - increase score
            m_gameManager->updateScore(10);
        } else if (hazard->type() == "poisonous") {
            // Poisonous plant - decrease health
            m_gameManager->damage(1);
        }
    }
}
