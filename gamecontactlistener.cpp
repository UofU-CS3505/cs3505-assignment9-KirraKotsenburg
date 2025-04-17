// gamecontactlistener.cpp
#include "gameContactListener.h"
#include <cstring>
#include "physicsworld.h"

GameContactListener::GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld)
    : m_gameManager(gameManager), m_physicsWorld(physicsWorld)
{
}

void GameContactListener::BeginContact(b2Contact* contact) {
    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();

    // Check for the "poisonous" tag using user data
    void* dataA = bodyA->GetUserData();
    void* dataB = bodyB->GetUserData();

    if ((dataA && strcmp(static_cast<const char*>(dataA), "poisonous") == 0) ||
        (dataB && strcmp(static_cast<const char*>(dataB), "poisonous") == 0)) {
        // If collision is detected, reduce health


        // Determine which body is the hazard
        b2Body* hazardBody = (dataA && strcmp(static_cast<const char*>(dataA), "poisonous") == 0) ? bodyA : bodyB;

        // Queue the hazard for removal instead of removing immediately
        m_physicsWorld->QueueForRemoval(hazardBody);
        m_gameManager->damage(10);
    }
}

void GameContactListener::EndContact(b2Contact* contact) {
    // Optional: handle end of contact event if needed
}
