/**
 * @file GameContactListener.h
 * @brief Header file for the GameContactListener class that handles contact event between the vehicle and plant object.
 * @details Handles Box2D collision events between the vehicle and plant objects.
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Kirra Kotsenburg
 */

#ifndef GAMECONTACTLISTENER_H
#define GAMECONTACTLISTENER_H

#include <QObject>
#include <box2d/box2d.h>
#include <unordered_set>
#include "gameManager.h"
#include "hazard.h"

// Forward declaration
class PhysicsWorld;

/**
 * @brief handles Box2D collision contact events and connect them to game logic.
 */
class GameContactListener : public QObject, public b2ContactListener {
    Q_OBJECT

private:

    GameManager *m_gameManager;

    PhysicsWorld *m_physicsWorld;

    std::unordered_set<b2Body*> m_processedBodies;

signals:

    /**
     * @brief Function to handle contact event. Signal emitted when contact with a plant occurs.
     * @param hazard pointer to the hazard that was contacted.
     */
    void plantContact(Hazard *hazard);

public:

    /**
     * @brief Constructor for GameContactListener.
     * @param gameManager Pointer to the game manager.
     * @param physicsWorld Pointer to the physics world.
     */
    GameContactListener(GameManager *gameManager, PhysicsWorld *physicsWorld);

    /**
     * @brief Called when vehicle and plant object begin to touch.
     * @param contact Pointer to the contact object.
     */
    virtual void beginContact(b2Contact* contact) override;
};

#endif // GAMECONTACTLISTENER_H
