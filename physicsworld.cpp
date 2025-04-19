#include "physicsworld.h"
#include "hazard.h"

PhysicsWorld::PhysicsWorld(int hazardCount)
    : m_world(b2Vec2(0.0f, -9.8f))  // Set gravity: downward 9.8 m/s²
    , m_timeStep(1.0f / 60.0f)      // 60 FPS simulation
    , m_velocityIterations(6)
    , m_positionIterations(2)
    , m_contactListener(nullptr)
{
    initializePlantDatabase();

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
    // Generate random hazards along the road with no duplicates
    int actualHazardCount = std::min(hazardCount, static_cast<int>(m_plantDatabase.size()));

    // Create a list of available plant indices
    std::vector<int> availablePlantIndices;
    for (int i = 0; i < m_plantDatabase.size(); i++) {
        availablePlantIndices.push_back(i);
    }

    // Fisher-Yates shuffle to randomize the indices
    for (int i = availablePlantIndices.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        std::swap(availablePlantIndices[i], availablePlantIndices[j]);
    }

    // Calculate road bounds
    float roadStartX = 15.0f; // Safe margin from start
    float roadEndX = (roadPointCount - 1) * segmentWidth - 15.0f; // Safe margin from end
    float roadLength = roadEndX - roadStartX;

    // Divide road into sections for plants with spacing
    float sectionLength = roadLength / actualHazardCount;

    for (int i = 0; i < actualHazardCount; i++) {
        // Calculate position within the section (add randomness but stay in section)
        float sectionStart = roadStartX + i * sectionLength;
        float randomOffset = (static_cast<float>(rand()) / RAND_MAX) * (sectionLength * 0.7f);
        float hazardX = sectionStart + randomOffset;

        // Find the road segment this position corresponds to
        int segmentIndex = static_cast<int>(hazardX / segmentWidth);
        segmentIndex = std::min(segmentIndex, roadPointCount - 2); // Ensure within valid range

        // Calculate position within segment (0.0 to 1.0)
        float segmentPosition = (hazardX - segmentIndex * segmentWidth) / segmentWidth;

        // Calculate Y value based on road height at this position
        float startY = -2.0f;
        for (int j = 0; j < segmentIndex; j++) {
            startY += deltaY[j % segmentCount];
        }

        float endY = startY + deltaY[segmentIndex % segmentCount];
        float hazardY = startY + segmentPosition * (endY - startY);

        // Add offset to place above the road
        hazardY += 0.7f;

        // Get unique plant from database
        int plantIndex = availablePlantIndices[i];
        const PlantData& plant = m_plantDatabase[plantIndex];

        // Create hazard with selected plant data
        m_hazards.push_back(new Hazard(
            m_world,
            b2Vec2(hazardX, hazardY),
            1.0,
            plant.type,
            plant.name,
            plant.description,
            plant.imagePath
            ));
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
    ProcessRemovalQueue();
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

// physicsworld.cpp - update Reset method
void PhysicsWorld::Reset() {
    // Clear existing hazards
    for(auto hazard : m_hazards) {
        m_world.DestroyBody(hazard->getBody());
        delete hazard;
    }
    m_hazards.clear();

    // Select two random plants for the initial hazards
    int plantIndex1 = arc4random() % m_plantDatabase.size();
    int plantIndex2 = (plantIndex1 + arc4random() % (m_plantDatabase.size() - 1) + 1) % m_plantDatabase.size(); // Ensure different plants

    const PlantData& plant1 = m_plantDatabase[plantIndex1];
    const PlantData& plant2 = m_plantDatabase[plantIndex2];

    // Recreate initial hazards with random plant data
    m_hazards.push_back(new Hazard(
        m_world,
        b2Vec2(-15.0f, -2.0f),
        1.0,
        plant1.type,
        plant1.name,
        plant1.description,
        plant1.imagePath
        ));

    m_hazards.push_back(new Hazard(
        m_world,
        b2Vec2(25.0f, 0.5f),
        1.0,
        plant2.type,
        plant2.name,
        plant2.description,
        plant2.imagePath
        ));

    // Reset vehicle
    m_vehicle->Reset(b2Vec2(0.0f, 10.0f));
}

// Add this new method implementation after the constructor
void PhysicsWorld::initializePlantDatabase()
{
    // Poisonous plants
    m_plantDatabase.push_back({"poisonous", "Baneberry",
                               "Highly poisonous berries that can cause cardiac arrest. All parts of the plant are toxic.",
                               ":/dangerous/Plants/Danger_Plants/baneberry.jpg"});

    m_plantDatabase.push_back({"poisonous", "Death Camas",
                               "Extremely toxic plant containing zygacine, which causes vomiting, seizures, and heart problems.",
                               ":/dangerous/Plants/Danger_Plants/death_camas.jpg"});

    m_plantDatabase.push_back({"poisonous", "Water Hemlock",
                               "One of the most toxic plants in North America. Contains cicutoxin that affects the central nervous system.",
                               ":/dangerous/Plants/Danger_Plants/water_hemlock.jpg"});

    m_plantDatabase.push_back({"poisonous", "Poison Ivy",
                               "Causes itchy rash due to urushiol oil. Contact can lead to severe allergic reactions.",
                               ":/dangerous/Plants/Danger_Plants/poison_ivy.jpg"});

    m_plantDatabase.push_back({"poisonous", "Jimsonweed",
                               "Contains tropane alkaloids that cause hallucinations, hyperthermia, and potentially fatal heart arrhythmias.",
                               ":/dangerous/Plants/Danger_Plants/jimson_weed.jpg"});

    // Beneficial herbs
    m_plantDatabase.push_back({"herb", "Creosote Bush",
                               "Used to boost the immune system and reduce symptoms of infections.",
                               ":/safe/Plants/Safe_Plants/creosote_bush.jpg"});

    m_plantDatabase.push_back({"herb", "Golden Currant",
                               "Rich in antioxidants and vitamins that may boost immunity and reduce inflammation.",
                               ":/safe/Plants/Safe_Plants/golden_currant.jpg"});

    m_plantDatabase.push_back({"herb", "Mormon Tea",
                               "Traditionally used to stop bleeding, reduce fever, and treat digestive issues.",
                               ":/safe/Plants/Safe_Plants/mormon_tea.jpg"});

    m_plantDatabase.push_back({"herb", "Osha",
                               "Calming herb that helps with sleep, digestive issues, and inflammation.",
                               ":/safe/Plants/Safe_Plants/osha.jpg"});

    m_plantDatabase.push_back({"herb", "Prairie Flax",
                               "Soothes digestive issues and freshens breath. Contains menthol that can relieve nasal congestion.",
                               ":/safe/Plants/Safe_Plants/prairie_flax.jpg"});
}
