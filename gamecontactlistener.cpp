// gamecontactlistener.cpp
#include "gameContactListener.h"
#include <cstring>
#include "physicsworld.h"
#include <QDebug>

GameContactListener::GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld)
    : m_gameManager(gameManager), m_physicsWorld(physicsWorld)
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
            // Queue the hazard's body for removal
              // assuming getBody() gives the associated b2Body*
            m_physicsWorld->QueueForRemoval(hazard->getBody());
            //m_gameManager->damage(10);
        }
        emit plantContact(hazard);
    }
}


void GameContactListener::EndContact(b2Contact* contact) {
    // Optional: handle end of contact event if needed
}
