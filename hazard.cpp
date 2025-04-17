#include "hazard.h"

// hazard.cpp
Hazard::Hazard(b2World &world, const b2Vec2 &position, float radius,
               const QString &type, const QString &desc, const QString &imgPath)
    : m_type(type), m_description(desc), m_imagePath(imgPath)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = position;
    m_body = world.CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.isSensor = true;
    m_body->CreateFixture(&fixtureDef);

    m_body->SetUserData(this); // Store pointer to this Hazard
}

Hazard::~Hazard()
{
    // Note: Body destruction should be managed externally (e.g., by PhysicsWorld)
}
