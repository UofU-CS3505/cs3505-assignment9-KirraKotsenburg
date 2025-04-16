#include "worldrenderer.h"
#include <QPainterPath>
#include <QFont>

WorldRenderer::WorldRenderer(QWidget *parent)
    : QWidget(parent)
    , m_physicsWorld(new PhysicsWorld())
    , m_scale(50.0f) // 50 pixels per meter
{
    // Initialize GameManager
    m_gameManager = new GameManager(this);

    // Register custom contact listener to handle game logic on collisions
    m_physicsWorld->GetWorld().SetContactListener(new GameContactListener(m_gameManager, m_physicsWorld));

    // Configure and start a timer to refresh screen at ~60 FPS
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &WorldRenderer::updateGameState);
    m_timer->start(16);

    // Allow keyboard focus for input handling
    setFocusPolicy(Qt::StrongFocus);
}

WorldRenderer::~WorldRenderer()
{
    delete m_timer;
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

    // Update physics and game state
    m_physicsWorld->Step();
    m_physicsWorld->ProcessRemovalQueue();
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

void WorldRenderer::resetGame() {
    m_physicsWorld->Reset();
    m_gameManager->resetGame();
}

void WorldRenderer::updateGameState()
{
    if(m_gameManager->gameState() == Playing) {
        m_physicsWorld->Step();
        m_physicsWorld->ProcessRemovalQueue();
        m_gameManager->update();
    }
    update(); // Request repaint
}
