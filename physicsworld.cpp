// physicsworld.cpp
#include "physicsworld.h"

PhysicsWorld::PhysicsWorld()
    : m_world(b2Vec2(0.0f, -9.8f)),   // Default gravity.
    m_timeStep(1.0f / 60.0f),       // 60 Hz simulation.
    m_velocityIterations(6),        // Recommended Box2D value.
    m_positionIterations(2)
{
    // Create the vehicle at the world's origin.
    m_vehicle = new Vehicle(m_world, b2Vec2(0.0f, 10.0f));

    // Create a static ground body for the hiking road.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f);
    b2Body* groundBody = m_world.CreateBody(&groundBodyDef);

    // Define a chain shape for an undulating hiking road.
    // These vertices give you a curvy road with hills and valleys.
    b2ChainShape roadChain;
    const int roadPointCount = 10;
    b2Vec2 roadPoints[roadPointCount] = {
        b2Vec2(-50.0f, -5.0f),
        b2Vec2(-40.0f, -4.0f),
        b2Vec2(-30.0f, -1.0f),
        b2Vec2(-20.0f,  1.0f),
        b2Vec2(-10.0f, -2.0f),
        b2Vec2(0.0f,   -1.0f),
        b2Vec2(10.0f,   2.0f),
        b2Vec2(20.0f,   0.0f),
        b2Vec2(30.0f,  -1.0f),
        b2Vec2(50.0f,   0.5f)
    };

    roadChain.CreateChain(roadPoints, roadPointCount);
    groundBody->CreateFixture(&roadChain, 0.0f);
}



PhysicsWorld::~PhysicsWorld()
{
    // Clean up the vehicle
    if (m_vehicle) {
        delete m_vehicle;
    }
}

void PhysicsWorld::Step()
{
    // Advance the physics simulation
    m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);
}

b2World& PhysicsWorld::GetWorld()
{
    return m_world;
}

Vehicle* PhysicsWorld::GetVehicle() const
{
    return m_vehicle;
}
