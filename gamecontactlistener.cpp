/**
 * @file GameContactListener.cpp
 * @brief Implementation of the GameContactListener class
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Kirra Kotsenburg, Jay Lee
 */

#include "gameContactListener.h"
#include <cstring>
#include "physicsworld.h"
#include <QDebug>

GameContactListener::GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld)
    : m_gameManager(gameManager), m_physicsWorld(physicsWorld), m_processedBodies(){ }

void GameContactListener::beginContact(b2Contact* contact) {
    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();

    Hazard* hazard = nullptr;
    b2Body* hazardBody = nullptr;

    // Check if either body is a hazard
    if (bodyA->GetUserData()) {
        hazard = static_cast<Hazard*>(bodyA->GetUserData());
        hazardBody = bodyA;
    }
    else if (bodyB->GetUserData()) {
        hazard = static_cast<Hazard*>(bodyB->GetUserData());
        hazardBody = bodyB;
    }

    // Process hazard if it exists and hasen't been processed yet
    if (hazard && m_processedBodies.find(hazardBody) == m_processedBodies.end()) {

        // Mark this body as processed to prevent duplicate processing
        m_processedBodies.insert(hazardBody);

        // Queue the hazard for removal
        m_physicsWorld->queueForRemoval(hazard->getBody());

        // Emit the signal to show popup
        qDebug() << "About to emit!";
        emit plantContact(hazard);
    }
}
