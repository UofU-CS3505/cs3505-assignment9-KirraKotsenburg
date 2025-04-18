// gamecontactlistener.h - add PhysicsWorld forward declaration
#ifndef GAMECONTACTLISTENER_H
#define GAMECONTACTLISTENER_H

#include <QObject>
#include <box2d/box2d.h>
#include "gameManager.h"
#include "hazard.h"

// Forward declaration
class PhysicsWorld;

class GameContactListener : public QObject, public b2ContactListener
{
    Q_OBJECT
private:
    GameManager *m_gameManager;
    PhysicsWorld *m_physicsWorld;

signals:
    void plantContact(Hazard *hazard);

public:
    GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld);
    virtual void BeginContact(b2Contact* contact) override;
    virtual void EndContact(b2Contact* contact) override;
};

#endif // GAMECONTACTLISTENER_H
