#include "physicsworld.h"
#include "hazard.h"

PhysicsWorld::PhysicsWorld(int hazardCount)
    : m_world(b2Vec2(0.0f, -9.8f))  // Set gravity: downward 9.8 m/s²
    , m_timeStep(1.0f / 60.0f)      // 60 FPS simulation
    , m_velocityIterations(6)
    , m_positionIterations(2)
    , m_contactListener(nullptr)
{
    srand(static_cast<unsigned int>(time(nullptr)));

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

    // Create static wall at the beginning of the road
    b2BodyDef leftWallDef;
    leftWallDef.position.Set(0.0f, currentY); // use currentY after loop
    b2Body* leftWall = m_world.CreateBody(&leftWallDef);

    b2PolygonShape leftShape;
    leftShape.SetAsBox(0.5f, 10.0f); // width 1m, height 20m
    leftWall->CreateFixture(&leftShape, 0.0f);

    // Create static wall at the end of the road
    float endX = (roadPointCount - 1) * segmentWidth;
    b2BodyDef rightWallDef;
    rightWallDef.position.Set(endX, currentY); // same height
    b2Body* rightWall = m_world.CreateBody(&rightWallDef);

    b2PolygonShape rightShape;
    rightShape.SetAsBox(0.5f, 10.0f);
    rightWall->CreateFixture(&rightShape, 0.0f);


    // -----------------------------------------------------

    // Generate random hazards along the road
    int actualHazardCount = std::min(hazardCount, 15); // Cap at 15 hazards

    for (int i = 0; i < actualHazardCount; i++) {
        // Generate a random X position along the road
        // Limit the range to avoid placing hazards too close to the start
        float minX = 15.0f;  // Minimum distance from start
        float maxX = (roadPointCount - 1) * segmentWidth;  // Maximum x based on road length
        float hazardX = minX + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxX - minX);

        // Find the exact segment the hazard is on
        int segmentIndex = static_cast<int>(hazardX / segmentWidth);

        // Calculate exact position within segment (from 0.0 to 1.0)
        float segmentPosition = (hazardX - segmentIndex * segmentWidth) / segmentWidth;

        // Get the Y values at the start and end of this segment
        float startY = -2.0f;
        float endY = -2.0f;

        // Calculate the Y values at segment start
        for (int j = 0; j < segmentIndex; j++) {
            startY += deltaY[j % segmentCount];
        }

        // Calculate the Y value at segment end
        endY = startY + deltaY[segmentIndex % segmentCount];

        // Interpolate between start and end Y values based on position within segment
        float hazardY = startY + segmentPosition * (endY - startY);

        // Add a consistent offset to ensure it's above the road
        hazardY += 0.7f;

        // Create the hazard at this position
        m_hazards.push_back(new Hazard(m_world, b2Vec2(hazardX, hazardY)));
    }
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
    // Process only a limited number of bodies per frame to avoid stressing Box2D
    const int maxRemovalsPerFrame = 3;
    int removalsThisFrame = 0;

    // Create a temporary list of bodies we actually removed
    std::vector<b2Body*> removedBodies;

    for (auto it = m_removeQueue.begin(); it != m_removeQueue.end() && removalsThisFrame < maxRemovalsPerFrame;) {
        b2Body* bodyToRemove = *it;

        // Find and remove the hazard from the hazards vector
        bool hazardFound = false;
        for (auto hazardIt = m_hazards.begin(); hazardIt != m_hazards.end(); ++hazardIt) {
            if ((*hazardIt)->getBody() == bodyToRemove) {
                // Delete the hazard object
                delete *hazardIt;

                // Remove from vector
                m_hazards.erase(hazardIt);
                hazardFound = true;
                break;
            }
        }

        if (hazardFound) {
            // Now safely destroy the Box2D body
            try {
                m_world.DestroyBody(bodyToRemove);
                removedBodies.push_back(bodyToRemove);
                it = m_removeQueue.erase(it);
                removalsThisFrame++;
            }
            catch (...) {
                // If destruction fails, move to next body
                ++it;
            }
        }
        else {
            // If hazard wasn't found, move to next body
            ++it;
        }
    }
}
