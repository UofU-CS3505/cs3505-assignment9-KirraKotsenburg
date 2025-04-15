#include "hazard.h"

Hazard::Hazard(b2World &world, const b2Vec2 &position, float radius)
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
    m_body->SetUserData((void*)"poisonous");
}

Hazard::~Hazard()
{
    // Note: Body destruction should be managed externally (e.g., by PhysicsWorld)
}
