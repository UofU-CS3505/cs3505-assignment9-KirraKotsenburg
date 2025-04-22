/**
 * @file physicsworld.h
 * @brief Defines the PhysicsWorld class that manages Box2D physics simulation
 *
 * @author Jason Chang
 *
 * Checked by
 */

#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "vehicle.h"
#include "hazard.h"
#include <box2d/box2d.h>
#include <vector>
#include <QString>
#include <QPair>


struct PlantData {
    QString type;         // "herb" or "poisonous"
    QString name;         // Plant name
    QString description;  // Plant description
    QString imagePath;    // Path to plant image
};

/**
 * @brief PhysicsWorld manages the Box2D physics world, vehicle, and hazards
 */
class PhysicsWorld {

private:
    b2World m_world;                    // Box2D physics world
    float m_timeStep;                   // Simulation time step
    int m_velocityIterations;           // Velocity constraint solver iterations
    int m_positionIterations;           // Position constraint solver iterations

    Vehicle *m_vehicle;                     // The player's controllable vehicle
    std::vector<Hazard*> m_hazards;         // List of hazardous plants in the world
    b2ContactListener* m_contactListener;   // Contact listener for collision detection
    std::vector<b2Body*> m_removeQueue;     // Bodies queued for removal
    std::vector<PlantData> m_plantDatabase; // Database of all plant information



public:

    /**
     * @brief Default constructor
     */
    PhysicsWorld();

    /**
     * @brief Constructor that sets up world, vehicle, terrain, and hazards
     * @param hazardCount Number of hazards to create in the world
     */
    PhysicsWorld(int hazardCount);

    /**
     * @brief Destructor cleans up dynamically allocated objects
     */
    ~PhysicsWorld();

    /**
     * @brief Advances the physics simulation by one step
     */
    void step();

    /**
     * @brief Returns a reference to the Box2D world
     * @return Reference to the Box2D world
     */
    b2World &getWorld();

    /**
     * @brief Returns a pointer to the vehicle
     * @return Pointer to the player's vehicle
     */
    Vehicle *getVehicle() const;

    /**
     * @brief Returns the list of hazards
     * @return Constant reference to the vector of hazards
     */
    const std::vector<Hazard*>& getHazards() const { return m_hazards; }

    /**
     * @brief Sets the contact listener for collision detection
     * @param listener Pointer to a b2ContactListener object
     */
    void setContactListener(b2ContactListener* listener);

    /**
     * @brief Queues a hazard body for safe removal
     * @param hazardBody Pointer to the Box2D body to remove
     */
    void queueForRemoval(b2Body* hazardBody);

    /**
     * @brief Processes the queue of bodies marked for removal
     */
    void processRemovalQueue();

    /**
     * @brief Resets the physics world to initial state
     */
    void reset();

    /**
     * @brief Initializes the plant database with information
     */
    void initializePlantDatabase();
};

#endif // PHYSICSWORLD_H
