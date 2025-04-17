#include "worldrenderer.h"
#include "mainwindow.h"
#include <QPainterPath>
#include <QFont>
#include <QMessageBox>

WorldRenderer::WorldRenderer(QWidget *parent)
    : QWidget(parent)
    , m_physicsWorld(new PhysicsWorld())
    , m_scale(50.0f) // 50 pixels per meter
{
    // Initialize GameManager
    m_gameManager = new GameManager(this);

    gameContactListener = new GameContactListener(m_gameManager);

    // Register custom contact listener to handle game logic on collisions
    m_physicsWorld->GetWorld().SetContactListener(gameContactListener);

    // Configure and start a timer to refresh screen at ~60 FPS
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&WorldRenderer::update));
    m_timer->start(16);

    // Allow keyboard focus for input handling
    setFocusPolicy(Qt::StrongFocus);
    // In WorldRenderer constructor or init function:
    connect(gameContactListener, &GameContactListener::plantContact, this, &WorldRenderer::showPlantPopup);

}

WorldRenderer::~WorldRenderer()
{
    delete m_timer;
    delete m_physicsWorld;
    // m_gameManager is managed by Qt via parent-child hierarchy, so no need to delete manually
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

    // Update physics and game state
    m_physicsWorld->Step();
    m_gameManager->update();

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
    roadPath.moveTo(worldToScreenCamera(b2Vec2(-50.0f, -5.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-40.0f, -4.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-30.0f, -1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-20.0f, 1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-10.0f, -2.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(0.0f, -1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(10.0f, 2.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(20.0f, 0.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(30.0f, -1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(50.0f, 0.5f)));
    painter.drawPath(roadPath); // Draw as white wireframe

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
        painter.drawText(width() / 2 - 50, 50, "Warning: Poisonous Plant!");
    }
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
    default:
        QWidget::keyPressEvent(event);
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
    default:
        QWidget::keyReleaseEvent(event);
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

void WorldRenderer::showPlantPopup() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Mysterious Plant Found");
    msgBox.setText("You've found a plant! Do you want to pick it?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        QMessageBox::information(this, "Plant Info", "This is Belladonna.\nIt's toxic but useful in small doses.");
    }
}

