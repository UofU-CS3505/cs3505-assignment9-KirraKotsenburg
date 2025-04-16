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

void Hazard::reset() {
    // Since hazards are static bodies, we only need to reset their position
    // No need to reset velocity as static bodies don't move
    m_body->SetTransform(m_body->GetPosition(), 0.0f);

    // If you want hazards to be exactly at their original positions,
    // you'll need to store the initial position in the class
}
