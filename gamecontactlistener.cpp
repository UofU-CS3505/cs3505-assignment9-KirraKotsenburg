#include "gameContactListener.h"
#include <cstring>
#include "physicsworld.h"
#include <QDebug>

GameContactListener::GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld)
    : m_gameManager(gameManager), m_physicsWorld(physicsWorld), m_processedBodies() // 초기화 추가
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

    if (hazard && m_processedBodies.find(hazardBody) == m_processedBodies.end()) {
        // Mark this body as processed to prevent duplicate processing
        m_processedBodies.insert(hazardBody);

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

void GameContactListener::clearProcessedBodies() {
    m_processedBodies.clear();
}
