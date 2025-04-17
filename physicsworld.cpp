#include "physicsworld.h"
#include "hazard.h"
#include <QFile>
#include <QJsonDocument>

PhysicsWorld::PhysicsWorld()
    : m_world(b2Vec2(0.0f, -9.8f))  // Set gravity: downward 9.8 m/s²
    , m_timeStep(1.0f / 60.0f)      // 60 FPS simulation
    , m_velocityIterations(6)
    , m_positionIterations(2)
{
    // Create the player's vehicle at an initial position
    m_vehicle = new Vehicle(m_world, b2Vec2(0.0f, 10.0f));

    // Define static ground body to represent terrain
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f);
    b2Body *groundBody = m_world.CreateBody(&groundBodyDef);

    b2ChainShape roadChain;
    const int roadPointCount = 10;
    b2Vec2 roadPoints[roadPointCount] = { b2Vec2(-50.0f, -5.0f),
                                         b2Vec2(-40.0f, -4.0f),
                                         b2Vec2(-30.0f, -1.0f),
                                         b2Vec2(-20.0f, 1.0f),
                                         b2Vec2(-10.0f, -2.0f),
                                         b2Vec2(0.0f, -1.0f),
                                         b2Vec2(10.0f, 2.0f),
                                         b2Vec2(20.0f, 0.0f),
                                         b2Vec2(30.0f, -1.0f),
                                         b2Vec2(50.0f, 0.5f) };

    roadChain.CreateChain(roadPoints, roadPointCount);
    groundBody->CreateFixture(&roadChain, 0.0f);

    // TODO: parse the json file here to get plant info
    m_hazards.push_back(new Hazard(m_world, b2Vec2(15.0f, 0.5f), 1, "poisonous", "Bad plant", ":/Danger/Plants/Danger_Plants/poison_hemlock.jpg"));
    m_hazards.push_back(new Hazard(m_world, b2Vec2(25.0f, 0.5f), 1, "safe", "Good plant", ":/Safe/Plants/Safe_Plants/creosote_bush.jpg"));
    // Place poisonous plants (Hazard objects) at specific locations
    // m_hazards.push_back(new Hazard(m_world, b2Vec2(-15.0f, -2.0f)));
    // m_hazards.push_back(new Hazard(m_world, b2Vec2(25.0f, 0.5f)));
}

PhysicsWorld::~PhysicsWorld()
{
    // Clean up dynamically created vehicle and hazard objects
    delete m_vehicle;
    for (Hazard* hazard : m_hazards)
        delete hazard;
}

// Advance the simulation one time step
void PhysicsWorld::Step()
{
    m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);
}

// Accessor for Box2D world
b2World &PhysicsWorld::GetWorld()
{
    return m_world;
}

// Accessor for the player vehicle
Vehicle *PhysicsWorld::GetVehicle() const
{
    return m_vehicle;
}
