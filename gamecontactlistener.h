#ifndef GAMECONTACTLISTENER_H
#define GAMECONTACTLISTENER_H

#include <box2d/box2d.h>
#include <QObject>
#include "gameManager.h"
#include "hazard.h"

// A contact listener that handles collision events in the physics world
class GameContactListener :  public QObject, public b2ContactListener
{
    Q_OBJECT
private:
    GameManager *m_gameManager; // Pointer to the game manager for handling game logic (e.g., health reduction)

signals:
    void plantContact(Hazard *hazard);

public:
    // Constructor that initializes the listener with a reference to the game manager
    GameContactListener(GameManager *gameManager);

    // Called automatically when two Box2D bodies begin contact
    virtual void BeginContact(b2Contact* contact) override;

    // Called automatically when two Box2D bodies end contact
    virtual void EndContact(b2Contact* /*contact*/) override;
};

#endif // GAMECONTACTLISTENER_H
