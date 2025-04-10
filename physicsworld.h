// physicsworld.h
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <box2d/box2d.h>
#include "vehicle.h"

class PhysicsWorld {
private:
    b2World m_world;
    float m_timeStep;
    int m_velocityIterations;
    int m_positionIterations;
    Vehicle* m_vehicle;

public:
    PhysicsWorld();
    ~PhysicsWorld();

    void Step();
    b2World& GetWorld();
    Vehicle* GetVehicle() const;
};

#endif // PHYSICSWORLD_H
