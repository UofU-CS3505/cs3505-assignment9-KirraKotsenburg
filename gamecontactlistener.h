// gamecontactlistener.h - add PhysicsWorld forward declaration
#ifndef GAMECONTACTLISTENER_H
#define GAMECONTACTLISTENER_H

#include <box2d/box2d.h>
#include "gameManager.h"

// Forward declaration
class PhysicsWorld;

class GameContactListener : public b2ContactListener
{
private:
    GameManager *m_gameManager;
    PhysicsWorld *m_physicsWorld;

public:
    GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld);
    virtual void BeginContact(b2Contact* contact) override;
    virtual void EndContact(b2Contact* contact) override;
};

#endif // GAMECONTACTLISTENER_H
