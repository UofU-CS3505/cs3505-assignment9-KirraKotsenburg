#include "gameContactListener.h"
#include <cstring>

GameContactListener::GameContactListener(GameManager *gameManager)
    : m_gameManager(gameManager)
{
}

void GameContactListener::BeginContact(b2Contact* contact) {
    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();

    Hazard* hazard = nullptr;

    if (bodyA->GetUserData()) hazard = static_cast<Hazard*>(bodyA->GetUserData());
    else if (bodyB->GetUserData()) hazard = static_cast<Hazard*>(bodyB->GetUserData());

    if (hazard) {
        if (hazard->type() == "poisonous") {
            m_gameManager->damage(10);
        }

        emit plantContact(hazard);  // We’ll change this to include the hazard pointer
    }
}


void GameContactListener::EndContact(b2Contact* /*contact*/) {
    // Optional: handle end of contact event if needed
}
