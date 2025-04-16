#include "gameContactListener.h"
#include <cstring>

GameContactListener::GameContactListener(GameManager *gameManager)
    : m_gameManager(gameManager)
{
}

void GameContactListener::BeginContact(b2Contact* contact) {
    // Skip if in cooldown
    if (m_damageCooldown > 0) return;

    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    // Check if either body is a hazard
    bool isHazardA = bodyA->GetUserData() &&
                     strcmp(static_cast<const char*>(bodyA->GetUserData()), "poisonous") == 0;
    bool isHazardB = bodyB->GetUserData() &&
                     strcmp(static_cast<const char*>(bodyB->GetUserData()), "poisonous") == 0;

    if (isHazardA || isHazardB) {
        m_gameManager->damage(10);
        m_damageCooldown = m_cooldownTime; // Start cooldown
    }
}

void GameContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    contact->SetEnabled(true);
}

void GameContactListener::update(float dt) {
    if (m_damageCooldown > 0) {
        m_damageCooldown -= dt;
    }
}

void GameContactListener::EndContact(b2Contact* contact) {

}
