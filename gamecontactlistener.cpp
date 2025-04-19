// gamecontactlistener.cpp
#include "gameContactListener.h"
#include <cstring>
#include "physicsworld.h"
#include <QDebug>
#include <unordered_set>

// Add this at file scope to track which hazards have been processed
namespace {
std::unordered_set<b2Body*> g_processedBodies;
}

GameContactListener::GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld)
    : m_gameManager(gameManager), m_physicsWorld(physicsWorld)
{
}

void GameContactListener::BeginContact(b2Contact* contact) {
    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();

    Hazard* hazard = nullptr;
    b2Body* hazardBody = nullptr;

    if (bodyA->GetUserData()) {
        hazard = static_cast<Hazard*>(bodyA->GetUserData());
        hazardBody = bodyA;
    }
    else if (bodyB->GetUserData()) {
        hazard = static_cast<Hazard*>(bodyB->GetUserData());
        hazardBody = bodyB;
    }

    if (hazard && g_processedBodies.find(hazardBody) == g_processedBodies.end()) {
        // Mark this body as processed to prevent duplicate processing
        g_processedBodies.insert(hazardBody);

        // Queue the hazard for removal
        m_physicsWorld->QueueForRemoval(hazard->getBody());

        // Emit the signal to show popup
        qDebug() << "About to emit!";
        emit plantContact(hazard);
    }
}

void GameContactListener::EndContact(b2Contact* contact) {
    // Optional: handle end of contact event if needed
}
