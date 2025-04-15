#include "gameContactListener.h"
#include <cstring>

GameContactListener::GameContactListener(GameManager *gameManager)
    : m_gameManager(gameManager)
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
        // If collision is detected, reduce health (e.g., by 10)
        m_gameManager->damage(10);
    }
}

void GameContactListener::EndContact(b2Contact* contact) {
    // Optional: handle end of contact event if needed
}
