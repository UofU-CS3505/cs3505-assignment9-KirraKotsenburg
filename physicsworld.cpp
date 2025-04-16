#include "physicsworld.h"
#include "hazard.h"

PhysicsWorld::PhysicsWorld()
    : m_world(b2Vec2(0.0f, -9.8f))  // Set gravity: downward 9.8 m/s²
    , m_timeStep(1.0f / 60.0f)      // 60 FPS simulation
    , m_velocityIterations(6)
    , m_positionIterations(2)
    , m_contactListener(nullptr)
{
    // Create the player's vehicle at an initial position
    //m_vehicle = new Vehicle(m_world, b2Vec2(0.0f, 10.0f));
    m_vehicle = new Vehicle(m_world, b2Vec2(10.0f, 5.0f));


    // Define static ground body to represent terrain
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f);
    b2Body *groundBody = m_world.CreateBody(&groundBodyDef);


    // part for making road---------------------------------
    // Define smooth hill road pattern
    const int segmentCount = 10;           // Number of points per pattern
    const int repetitions = 10;            // How many times the pattern is repeated
    const int roadPointCount = segmentCount * repetitions;
    b2Vec2 roadPoints[roadPointCount];

    // Y-coordinate changes (Δy) for each segment to create slope up and down
    float deltaY[segmentCount] = {
        0.0f,
        0.5f,
        1.0f,
        1.2f,
        1.0f,
        -0.5f,
        -1.0f,
        -0.8f,
        -0.5f,
        -0.9f
    };

    float segmentWidth = 10.0f;  // Distance between each x point
    float currentY = -2.0f;      // Initial y-position of the road

    // Generate road points by accumulating y-values
    for (int i = 0; i < roadPointCount; ++i) {
        float x = i * segmentWidth;
        currentY += deltaY[i % segmentCount];  // Accumulate y to make smooth transitions
        roadPoints[i] = b2Vec2(x, currentY);
    }

    // Create a Box2D chain shape using the generated points
    b2ChainShape roadChain;
    roadChain.CreateChain(roadPoints, roadPointCount);
    groundBody->CreateFixture(&roadChain, 0.0f);


    // -----------------------------------------------------

    // Place poisonous plants (Hazard objects) at specific locations
    m_hazards.push_back(new Hazard(m_world, b2Vec2(-15.0f, -2.0f)));
    m_hazards.push_back(new Hazard(m_world, b2Vec2(25.0f, 0.5f)));
}

PhysicsWorld::~PhysicsWorld()
{
    // Clean up dynamically created vehicle and hazard objects
    delete m_vehicle;
    for (Hazard* hazard : m_hazards)
        delete hazard;

    if (m_contactListener) {
        delete m_contactListener;
    }
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


void PhysicsWorld::SetContactListener(b2ContactListener* listener) {
    if (m_contactListener) {
        delete m_contactListener;
    }
    m_contactListener = listener;
    m_world.SetContactListener(listener);
}

void PhysicsWorld::QueueForRemoval(b2Body* hazardBody) {
    // Add the body to the removal queue
    m_removeQueue.push_back(hazardBody);
}

void PhysicsWorld::ProcessRemovalQueue() {
    // Process all bodies queued for removal
    for (auto it = m_removeQueue.begin(); it != m_removeQueue.end(); ++it) {
        b2Body* bodyToRemove = *it;

        // Find and remove the hazard from the hazards vector
        for (auto hazardIt = m_hazards.begin(); hazardIt != m_hazards.end(); ++hazardIt) {
            if ((*hazardIt)->getBody() == bodyToRemove) {
                // Delete the hazard object
                delete *hazardIt;

                // Remove from vector
                m_hazards.erase(hazardIt);
                break;
            }
        }

        // Now safely destroy the Box2D body
        m_world.DestroyBody(bodyToRemove);
    }

    // Clear the queue
    m_removeQueue.clear();
}
