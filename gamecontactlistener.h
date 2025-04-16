#ifndef GAMECONTACTLISTENER_H
#define GAMECONTACTLISTENER_H

#include <box2d/box2d.h>
#include "gameManager.h"

// A contact listener that handles collision events in the physics world
class GameContactListener : public b2ContactListener
{
private:
    GameManager *m_gameManager; // Pointer to the game manager for handling game logic (e.g., health reduction)

    float m_damageCooldown = 0.0f;
    const float m_cooldownTime = 1.0f;

public:
    // Constructor that initializes the listener with a reference to the game manager
    GameContactListener(GameManager *gameManager);

    // Called automatically when two Box2D bodies begin contact
    virtual void BeginContact(b2Contact* contact) override;

    // Called automatically when two Box2D bodies end contact
    virtual void EndContact(b2Contact* contact) override;

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

    void update(float dt);

signals:
    void hitOccured();
};

#endif // GAMECONTACTLISTENER_H
