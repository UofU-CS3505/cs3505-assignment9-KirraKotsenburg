// worldrenderer.cpp
#include "worldrenderer.h"
#include <QPainter>
#include <QPainterPath>   // Added this include to resolve incomplete type error.
#include <QTimer>
#include <QDebug>

WorldRenderer::WorldRenderer(QWidget* parent)
    : QWidget(parent),
    m_physicsWorld(new PhysicsWorld()),
    m_scale(50.0f)  // 50 pixels per meter
{
    // Set up rendering timer
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&WorldRenderer::update));
    m_timer->start(16);  // ~60 FPS

    // Enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);
}

WorldRenderer::~WorldRenderer()
{
    // Clean up resources
    delete m_timer;
    delete m_physicsWorld;
}

void WorldRenderer::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Step the physics simulation.
    m_physicsWorld->Step();

    // Retrieve the vehicle and use its chassis position as the camera center.
    Vehicle* vehicle = m_physicsWorld->GetVehicle();
    b2Body* chassis = vehicle->GetChassis();
    b2Vec2 camCenter = chassis->GetPosition();

    // Define a lambda that converts world coordinates to screen coordinates,
    // taking into account the camera center. With this conversion, the camera
    // will follow the vehicle so that its position maps to the center of the widget.
    auto worldToScreenCamera = [this, camCenter](const b2Vec2 &worldPos) -> QPointF {
        return QPointF((worldPos.x - camCenter.x) * m_scale + width() / 2,
                       height() / 2 - (worldPos.y - camCenter.y) * m_scale);
    };

    // --- Draw the Hiking Road ---
    // Use the same set of vertices as in your physics simulation.
    QPainterPath roadPath;
    roadPath.moveTo(worldToScreenCamera(b2Vec2(-50.0f, -5.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-40.0f, -4.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-30.0f, -1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-20.0f,  1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(-10.0f, -2.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(0.0f,   -1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(10.0f,    2.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(20.0f,    0.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(30.0f,   -1.0f)));
    roadPath.lineTo(worldToScreenCamera(b2Vec2(50.0f,    0.5f)));

    // To create a filled road (the appearance of a solid trail), extend the path to the bottom of the view.
    // Calculate the bottom in world space; note that the height in world units is height()/m_scale.
    float bottomWorldY = camCenter.y - (height() / (2 * m_scale));
    QPainterPath filledRoadPath = roadPath;
    filledRoadPath.lineTo(worldToScreenCamera(b2Vec2(50.0f, bottomWorldY)));
    filledRoadPath.lineTo(worldToScreenCamera(b2Vec2(-50.0f, bottomWorldY)));
    filledRoadPath.closeSubpath();

    // Draw the filled road first, then its outline.
    painter.setBrush(Qt::darkYellow);
    painter.drawPath(filledRoadPath);
    painter.setPen(Qt::darkGreen);
    painter.drawPath(roadPath);

    // --- Draw the Vehicle ---
    // Draw the chassis.
    QPointF chassisScreenPos = worldToScreenCamera(chassis->GetPosition());
    painter.save();
    painter.translate(chassisScreenPos);
    painter.rotate(chassis->GetAngle() * 180.0f / b2_pi);
    // Draw the chassis as a rectangle representing 2x1 meters in world coordinates.
    // To convert from meters to pixels, we use m_scale.
    QRectF chassisRect(-1.0, -0.5, 0.5, 0.5);
    painter.scale(m_scale, m_scale);
    painter.setBrush(Qt::blue);
    painter.drawRect(chassisRect);
    painter.restore();

    // Draw the wheels.
    painter.setBrush(Qt::black);
    for (int i = 0; i < 2; ++i) {
        b2Body* wheel = vehicle->GetWheel(i);
        QPointF wheelScreenPos = worldToScreenCamera(wheel->GetPosition());
        painter.save();
        painter.translate(wheelScreenPos);
        painter.rotate(wheel->GetAngle() * 180.0f / b2_pi);
        // Each wheel is drawn as a circle with a 0.5m radius (diameter = 1.0 m).
        QRectF wheelRect(-0.5, -0.5, 0.5, 0.5);
        painter.scale(m_scale, m_scale);
        painter.drawEllipse(wheelRect);
        painter.restore();
    }
}

void WorldRenderer::keyPressEvent(QKeyEvent* event)
{
    Vehicle* vehicle = m_physicsWorld->GetVehicle();

    switch(event->key()) {
    case Qt::Key_Up:
        // Apply forward drive force
        vehicle->ApplyDriveForce(100.0f);
        break;
    case Qt::Key_Down:
        // Apply reverse drive force
        vehicle->ApplyDriveForce(-100.0f);
        break;
    case Qt::Key_Left:
        // Steer left
        vehicle->ApplyDriveForce(-100.0f);
        break;
    case Qt::Key_Right:
        // Steer right
        vehicle->ApplyDriveForce(100.0f);
        break;
    }
}

void WorldRenderer::keyReleaseEvent(QKeyEvent* event)
{
    Vehicle* vehicle = m_physicsWorld->GetVehicle();

    switch(event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
        // Reset steering
        vehicle->ApplySteering(0.0f);
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
        // Stop drive force
        vehicle->ApplyDriveForce(0.0f);
        break;
    }
}

QPointF WorldRenderer::worldToScreen(float x, float y)
{
    return QPointF(
        x * m_scale + width() / 2.0f,
        height() / 2.0f - y * m_scale
        );
}

QPointF WorldRenderer::worldToScreen(const b2Vec2& position)
{
    return worldToScreen(position.x, position.y);
}
