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

// PhysicsWorld manages the Box2D physics world, vehicle, and hazards
class PhysicsWorld
{
private:
    b2World m_world;                     // Box2D physics world
    float m_timeStep;                   // Simulation time step
    int m_velocityIterations;          // Velocity constraint solver iterations
    int m_positionIterations;          // Position constraint solver iterations

    Vehicle *m_vehicle;                // The player's controllable vehicle
    std::vector<Hazard*> m_hazards;   // List of hazardous plants in the world
    b2ContactListener* m_contactListener; // Contact listener for collision detection
    std::vector<b2Body*> m_removeQueue; // Bodies queued for removal
    std::vector<PlantData> m_plantDatabase;



public:
    // Constructor sets up world, vehicle, terrain, and hazards
    PhysicsWorld();
    PhysicsWorld(int hazardCount = 10);

    // Destructor cleans up dynamically allocated objects
    ~PhysicsWorld();

    // Advances the physics simulation by one step
    void Step();

    // Returns a reference to the Box2D world
    b2World &GetWorld();

    // Returns a pointer to the vehicle
    Vehicle *GetVehicle() const;

    // Returns the list of hazards
    const std::vector<Hazard*>& getHazards() const { return m_hazards; }

    void SetContactListener(b2ContactListener* listener);

    void QueueForRemoval(b2Body* hazardBody);
    void ProcessRemovalQueue();

    void Reset();
    void initializePlantDatabase();


};

#endif // PHYSICSWORLD_H
