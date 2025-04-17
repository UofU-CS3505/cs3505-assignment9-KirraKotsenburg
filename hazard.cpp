#include "hazard.h"

Hazard::Hazard(b2World &world, const b2Vec2 &position, float radius,
               const QString &type, const QString &desc, const QString &imgPath)
    :  m_radius(radius), m_type(type), m_description(desc), m_imagePath(imgPath)
{
    // Define a static body at the specified position
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = position;
    m_body = world.CreateBody(&bodyDef);

    // Represent the hazard as a simple circular sensor
    b2CircleShape circle;
    circle.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.isSensor = true;  // Detect collisions without physical response
    m_body->CreateFixture(&fixtureDef);

    // Tag the body with user data to identify it during collision events
    m_body->SetUserData(this);
}

Hazard::~Hazard()
{
    // Note: Body destruction should be managed externally (e.g., by PhysicsWorld)
}

void Hazard::reset() {
    // Reset to original position and reactivate
    m_body->SetActive(true);

    // Recreate fixture if it was removed
    if(m_body->GetFixtureList() == nullptr) {
        b2CircleShape circle;
        circle.m_radius = m_radius;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circle;
        fixtureDef.isSensor = true;
        m_body->CreateFixture(&fixtureDef);
    }
}
