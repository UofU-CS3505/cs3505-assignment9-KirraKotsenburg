/**
 * @file WorldRenderer.cpp
 * @brief Implementation of the WorldRenderer class.
 *
 * @author Jason Chang
 *
 * Checked by
 */

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
#include <random>

WorldRenderer::WorldRenderer(QWidget *parent) : QWidget(parent),
                                                m_physicsWorld(new PhysicsWorld(15)), // Limit to 15 plants
                                                m_scale(50.0f), // 50 pixels per meter
                                                m_background(":/images/Plants/Images/background.jpg") {
    // Initialize GameManager
    m_gameManager = new GameManager(this);

    // Register custom contact listener to handle game logic on collisions
    m_contactListener = new GameContactListener(m_gameManager, m_physicsWorld);
    m_physicsWorld->getWorld().SetContactListener(m_contactListener);

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
    Q_UNUSED(event);
    QPainter painter(this);

    // Clear screen with black background
    painter.fillRect(rect(), Qt::black);

    // Set white stroke and no fill for wireframe drawing
    painter.setPen(Qt::white);
    painter.setBrush(Qt::NoBrush);

    // Camera follows the vehicle's chassis
    Vehicle *vehicle = m_physicsWorld->getVehicle();
    b2Body *chassis = vehicle->getChassis();
    b2Vec2 camCenter = chassis->GetPosition();
    float cameraYOffset = 4.0f;

    // Define a lambda for world-to-screen conversion with camera offset
    auto worldToScreenCamera = [this, camCenter, cameraYOffset](const b2Vec2 &worldPos) -> QPointF {
        return QPointF((worldPos.x - camCenter.x) * m_scale + width() / 2,
                       height() / 2 - ((worldPos.y - camCenter.y - cameraYOffset) * m_scale));
    };

    // draw background
    if (!m_background.isNull()) {
        int bgWidth = m_background.width();
        float offsetX = chassis->GetPosition().x * m_scale;
        int startX = -static_cast<int>(offsetX) % bgWidth - bgWidth;

        for (int x = startX; x < width(); x += bgWidth) {
            painter.drawPixmap(x, 0, bgWidth, height(), m_background);
        }
    }

    // --- Draw Road ---
    QPainterPath roadPath;
    QPainterPath filledRoadPath;
    QVector<QPointF> upperPoints;

    // Get the list of bodies in the Box2D world
    const auto& roadBody = m_physicsWorld->getWorld().GetBodyList();

    // Loop through all bodies in the world
    for (b2Body* body = roadBody; body; body = body->GetNext()) {
        // Loop through all fixtures of the body
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            // Check if the fixture is a chain shape (used for the road)
            if (f->GetType() == b2Shape::e_chain) {
                b2ChainShape* chain = static_cast<b2ChainShape*>(f->GetShape());

                // If the chain has points, start drawing
                if (chain->m_count > 0) {
                    QPointF firstPoint = worldToScreenCamera(chain->m_vertices[0]);
                    upperPoints.append(firstPoint);

                    // Move to the first point of the chain
                    roadPath.moveTo(firstPoint);
                    filledRoadPath.moveTo(firstPoint);

                    // Connect lines to each subsequent point
                    for (int i = 1; i < chain->m_count; ++i) {
                        QPointF point = worldToScreenCamera(chain->m_vertices[i]);
                        roadPath.lineTo(point);
                        filledRoadPath.lineTo(point);
                        upperPoints.append(point);
                    }
                }
            }
        }
    }

    if (!upperPoints.isEmpty()) {
        QPointF lastPoint = upperPoints.last();
        QPointF bottomRight = QPointF(lastPoint.x(), height());
        filledRoadPath.lineTo(bottomRight);

        QPointF bottomLeft = QPointF(upperPoints.first().x(), height());
        filledRoadPath.lineTo(bottomLeft);

        filledRoadPath.lineTo(upperPoints.first());
    }

    // Draw the filled road
    QBrush roadBrush(QColor(210, 180, 140));

    painter.setPen(Qt::NoPen);
    painter.setBrush(roadBrush);
    painter.drawPath(filledRoadPath);

    // --- Draw House ---
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
    painter.scale(m_scale, m_scale);

    // Car body design
    QPainterPath CarBody;
    CarBody.moveTo(-1.2, 0.0);       // rear bottom
    CarBody.lineTo(-1.2, -0.4);      // rear up
    CarBody.lineTo(-0.8, -0.8);      // curve to roof
    CarBody.lineTo(0.4, -0.8);       // roof
    CarBody.lineTo(0.6, -0.5);       // sloped front
    CarBody.lineTo(1.2, -0.4);       // bottom front up
    CarBody.lineTo(1.2, 0.0);        // front bottom
    CarBody.lineTo(-1.2, 0.0);       // close
    CarBody.closeSubpath();

    painter.setBrush(QColor(200, 40, 40));  // 진한 빨간색
    painter.setPen(Qt::NoPen);
    painter.drawPath(CarBody);
    painter.restore();

    // Wheels
    for (int i = 0; i < 2; ++i) {
        b2Body *wheel = vehicle->getWheel(i);
        QPointF wheelScreenPos = worldToScreenCamera(wheel->GetPosition());
        painter.save();
        painter.translate(wheelScreenPos);
        painter.rotate(wheel->GetAngle() * 180.0f / b2_pi);
        painter.scale(m_scale, m_scale);

        // Outer white tire
        QRectF outerWheel(-0.3, -0.3, 0.6, 0.6);
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(outerWheel);

        // Inner white hub
        QRectF innerWheel(-0.18, -0.18, 0.36, 0.36);
        painter.setBrush(Qt::gray);
        painter.drawEllipse(innerWheel);

        // Center dot (larger gray hub center)
       QRectF centerDot(-0.045, -0.045, 0.09, 0.09);
        painter.setBrush(Qt::black);
        painter.drawEllipse(centerDot);

        painter.restore();
    }

    // --- Draw Plants ---
    const auto &hazards = m_physicsWorld->getHazards();
    std::vector<Hazard*> herbsToRender;
    std::vector<Hazard*> poisonousToRender;

    // First, separate herbs and poisonous plants
    for (auto hazard : hazards) {
        if (hazard->type() == "herb") {
            herbsToRender.push_back(hazard);
        } else if (hazard->type() == "poisonous") {
            poisonousToRender.push_back(hazard);
        }
    }

    // Limit poisonous plants to at most 5
    if (poisonousToRender.size() > 5) {
        poisonousToRender.resize(5);
    }

    // Combine the filtered lists
    std::vector<Hazard*> hazardsToRender;
    hazardsToRender.insert(hazardsToRender.end(), herbsToRender.begin(), herbsToRender.end());
    hazardsToRender.insert(hazardsToRender.end(), poisonousToRender.begin(), poisonousToRender.end());

    // Now draw only the filtered hazards
    for (auto hazard : hazardsToRender) {
        b2Body *body = hazard->getBody();
        b2Vec2 pos = body->GetPosition();
        float angle = body->GetAngle();
        QPointF screenPos = worldToScreenCamera(pos);

        painter.save();
        painter.translate(screenPos);
        painter.rotate(angle * 180.0f / b2_pi);
        painter.scale(m_scale, m_scale);

        // Use position-based seed for deterministic randomness
        std::seed_seq seed{static_cast<int>(pos.x * 1000), static_cast<int>(pos.y * 1000)};
        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> greenOffset(0, 80);
        std::uniform_real_distribution<float> ctrlOffset(-0.25f, 0.25f);
        std::uniform_real_distribution<float> tipJitter(0.0f, 0.3f);

        // Set pen color and thickness for plant
        int green = 130 + static_cast<int>(greenOffset(rng));
        painter.setPen(QPen(QColor(0, green, 0), 0.05));

        // Plant configuration
        int bladeCount = 13;
        float baseSpread = 0.1f;
        float bladeHeight = 1.6f;

        for (int i = -bladeCount / 2; i <= bladeCount / 2; ++i) {
            float offsetX = i * baseSpread;
            float ctrlX = offsetX * 0.5f + ctrlOffset(rng);
            float tipY = bladeHeight + tipJitter(rng);

            QPainterPath blade;
            blade.moveTo(offsetX, 0.0f);
            blade.quadTo(ctrlX, tipY * 0.5f, 0.0f, tipY);
            painter.drawPath(blade);
        }

        painter.restore();
    }


    // --- Draw HUD ---
    painter.resetTransform();
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(14);
    painter.setFont(font);

    // Draw plants to collect info
    QFont titleFont = font;
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(10, 20, "Collect all plants below to complete the game");

    font.setBold(false);
    painter.setFont(font);

    // List all plants to collect with status
    int yPos = 40;
    const auto& plants = m_gameManager->plantsToCollect();
    for (const auto& pair : plants) {
        painter.drawText(10, yPos += 20,
                         QString("%1 %2/%3").arg(pair.first)
                             .arg(pair.second.collected)
                             .arg(pair.second.total));
    }

    // Show poisonous plants collected
    painter.setPen(Qt::red);
    painter.drawText(10, yPos += 30,
                     QString("Poison Collected: %1/%2")
                         .arg(m_gameManager->poisonousCollected())
                         .arg(m_gameManager->maxPoisonousAllowed()));

    painter.end();
}

void WorldRenderer::keyPressEvent(QKeyEvent *event)
{
    Vehicle *vehicle = m_physicsWorld->getVehicle();

    switch (event->key()) {
    case Qt::Key_Left:
        vehicle->applyDriveForce(-60.0f);
        break;
    case Qt::Key_Right:
        vehicle->applyDriveForce(60.0f);
        break;
    }
}

void WorldRenderer::keyReleaseEvent(QKeyEvent *event)
{
    Vehicle *vehicle = m_physicsWorld->getVehicle();

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
        vehicle->applyDriveForce(0.0f); // Reset steering
        break;
    }
}

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
    m_physicsWorld->getWorld().SetContactListener(m_contactListener);

    connect(m_contactListener, &GameContactListener::plantContact, this, &WorldRenderer::showPlantPopup);
    // Reset game state
    if (m_gameManager) {
        int currentLevel = m_gameManager->currentLevel();
        if (currentLevel == 0) {
            // If not in a level state, start at level 1
            m_gameManager->startSpecificLevel(1);
        } else {
            // Restart current level
            m_gameManager->startSpecificLevel(currentLevel);
        }
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
    // Check if in any playable level state
    if (m_gameManager->gameState() == Level1 ||
        m_gameManager->gameState() == Level2 ||
        m_gameManager->gameState() == Level3) {

        try {
            // Process physics updates
            m_physicsWorld->step();

            // Process hazard removals after Step() completes
            m_physicsWorld->processRemovalQueue();

            // Update game logic
            m_gameManager->update();

            // Check if vehicle reached the right house (game clear condition)
            Vehicle *vehicle = m_physicsWorld->getVehicle();
            b2Vec2 vehiclePos = vehicle->getChassis()->GetPosition();

            // Right house position is at x=990.0f, y=-1.0f
            const float houseX = 990.0f;
            const float houseY = -1.0f;
            const float arrivalThreshold = 5.0f; // Distance threshold for arrival

            // Calculate distance to right house
            float distance = b2Distance(vehiclePos, b2Vec2(houseX, houseY));

            if (distance <= arrivalThreshold) {
                if (m_gameManager->isLevelComplete()) {
                    m_gameManager->gameClear(); // Successfully completed level
                } else {
                    m_gameManager->gameOver(); // Reached house but didn't collect all plants
                }
            }
        }
        catch (...) {
            // If an exception occurs, safely pause the game
            pauseGame();
        }
    }

    // Always request a repaint
    update();
}

void WorldRenderer::showPlantPopup(Hazard* hazard) {
    QDialog* dialog = new QDialog;
    dialog->setWindowTitle("Plant Found");

    QVBoxLayout* layout = new QVBoxLayout(dialog);

    // Image (check if it opens correctly)
    QPixmap image(hazard->imagePath());
    QLabel* imageLabel = new QLabel();
    if (!image.isNull()) {
        image = image.scaled(480, 480, Qt::KeepAspectRatio);
        imageLabel->setPixmap(image);
    } else {
        imageLabel->setText("Image not found");
    }
    imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imageLabel);

    // Text prompt
    QLabel* questionLabel = new QLabel("Do you want to pick this plant?");
    questionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(questionLabel);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* yesButton = new QPushButton("Yes");
    QPushButton* noButton = new QPushButton("No");
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
    layout->addLayout(buttonLayout);

    // Info elements (initially hidden)
    QLabel* nameLabel = new QLabel(hazard->plantName());
    nameLabel->setAlignment(Qt::AlignCenter);
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(16);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    nameLabel->hide();
    layout->addWidget(nameLabel);

    QLabel* typeLabel = new QLabel(hazard->type() == "herb" ?
                                       "This appears to be a beneficial herb!" :
                                       "Warning: This may be poisonous!");
    typeLabel->setAlignment(Qt::AlignCenter);
    typeLabel->setStyleSheet(hazard->type() == "herb" ?
                                 "color: green; font-weight: bold;" :
                                 "color: red; font-weight: bold;");
    typeLabel->hide();
    layout->addWidget(typeLabel);

    QLabel* descriptionLabel = new QLabel(hazard->description());
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->hide();
    layout->addWidget(descriptionLabel);

    QPushButton* closeButton = new QPushButton("Close");
    closeButton->hide();
    layout->addWidget(closeButton);

    pauseGame();

    // Use QPointer to avoid warning for local variables going out of scope for lambda
    QPointer<QDialog> dialogPtr(dialog);
    QPointer<QLabel> questionPtr(questionLabel);
    QPointer<QLabel> namePtr(nameLabel);
    QPointer<QLabel> typePtr(typeLabel);
    QPointer<QLabel> descriptionPtr(descriptionLabel);
    QPointer<QPushButton> yesPtr(yesButton);
    QPointer<QPushButton> noPtr(noButton);
    QPointer<QPushButton> closePtr(closeButton);

    QObject::connect(noButton, &QPushButton::clicked, [dialogPtr]() {
        if (dialogPtr) dialogPtr->reject();
    });

    QObject::connect(yesButton, &QPushButton::clicked, this, [=]() {
        // Toggle display elements as before
        if (questionPtr) questionPtr->hide();
        if (yesPtr) yesPtr->hide();
        if (noPtr) noPtr->hide();
        if (namePtr) namePtr->show();
        if (typePtr) typePtr->show();
        if (descriptionPtr) descriptionPtr->show();
        if (closePtr) closePtr->show();

        // Game effects - track plant collection
        bool isPoisonous = (hazard->type() == "poisonous");
        m_gameManager->collectPlant(hazard->plantName(), isPoisonous);

        // Force a repaint to update the HUD immediately
        update();
    });

    QObject::connect(closeButton, &QPushButton::clicked, [dialogPtr]() {
        if (dialogPtr) dialogPtr->accept();
    });

    dialog->exec();
    resumeGame();

    // Clear up memory
    delete dialog;
}


