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
    // Replace the plant placement section with this code

    // Calculate road bounds
    float roadStartX = 15.0f; // Safe margin from start
    float roadEndX = (roadPointCount - 1) * segmentWidth - 15.0f; // Safe margin from end
    float roadLength = roadEndX - roadStartX;

    // Separate plants by type
    std::vector<int> poisonousPlantIndices;
    std::vector<int> herbPlantIndices;

    for (int i = 0; i < m_plantDatabase.size(); i++) {
        if (m_plantDatabase[i].type == "poisonous") {
            poisonousPlantIndices.push_back(i);
        } else if (m_plantDatabase[i].type == "herb") {
            herbPlantIndices.push_back(i);
        }
    }

    // Fisher-Yates shuffle for poisonous plants
    for (int i = poisonousPlantIndices.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        std::swap(poisonousPlantIndices[i], poisonousPlantIndices[j]);
    }

    // Select 5 poisonous plants
    poisonousPlantIndices.resize(5);

    // Combine selected poisonous and all herb plants
    std::vector<int> selectedPlantIndices;
    selectedPlantIndices.insert(selectedPlantIndices.end(), poisonousPlantIndices.begin(), poisonousPlantIndices.end());
    selectedPlantIndices.insert(selectedPlantIndices.end(), herbPlantIndices.begin(), herbPlantIndices.end());

    // Shuffle the combined list of plant indices
    for (int i = selectedPlantIndices.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        std::swap(selectedPlantIndices[i], selectedPlantIndices[j]);
    }

    // Create an array of possible X positions along the road
    int actualHazardCount = selectedPlantIndices.size(); // Should be 16 (5 poisonous + 11 herbs)
    std::vector<float> hazardPositions;

    // Generate possible positions with minimum spacing
    float minSpacing = roadLength / (actualHazardCount * 1.5f); // Ensure some minimum spacing
    for (int i = 0; i < actualHazardCount; i++) {
        float position;
        bool validPosition;

        // Try to find a position that's not too close to existing positions
        int maxAttempts = 20;
        int attempts = 0;

        do {
            validPosition = true;
            position = roadStartX + (static_cast<float>(rand()) / RAND_MAX) * roadLength;

            // Check distance from all existing positions
            for (float existingPos : hazardPositions) {
                if (std::abs(position - existingPos) < minSpacing) {
                    validPosition = false;
                    break;
                }
            }
            attempts++;
        } while (!validPosition && attempts < maxAttempts);

        hazardPositions.push_back(position);
    }

    // Place plants at random positions (ensure they stay on the road)
    int placedCount = 0;
    int attempts = 0;
    int maxPlacementAttempts = 100;

    while (placedCount < actualHazardCount && attempts < maxPlacementAttempts) {
        float hazardX = hazardPositions[placedCount];

        int segmentIndex = static_cast<int>(hazardX / segmentWidth);
        segmentIndex = std::min(segmentIndex, roadPointCount - 2);

        // Skip upward slopes
        float slope = deltaY[segmentIndex % segmentCount];
        if (slope > 0.0f) {
            attempts++;
            // Try finding a new position
            float newPosition;
            bool validPosition;
            int innerAttempts = 0;
            do {
                validPosition = true;
                newPosition = roadStartX + (static_cast<float>(rand()) / RAND_MAX) * roadLength;

                for (float existingPos : hazardPositions) {
                    if (std::abs(newPosition - existingPos) < minSpacing) {
                        validPosition = false;
                        break;
                    }
                }

                innerAttempts++;
            } while (!validPosition && innerAttempts < 20);

            hazardPositions[placedCount] = newPosition;
            continue;
        }

        // Interpolate Y position
        float segmentPosition = (hazardX - segmentIndex * segmentWidth) / segmentWidth;

        float startY = -2.0f;
        for (int j = 0; j < segmentIndex; j++) {
            startY += deltaY[j % segmentCount];
        }

        float endY = startY + deltaY[segmentIndex % segmentCount];
        float hazardY = startY + segmentPosition * (endY - startY);
        hazardY += 0.7f;

        int plantIndex = selectedPlantIndices[placedCount];
        const PlantData& plant = m_plantDatabase[plantIndex];

        m_hazards.push_back(new Hazard(
            m_world,
            b2Vec2(hazardX, hazardY),
            1.0,
            plant.type,
            plant.name,
            plant.description,
            plant.imagePath
            ));

        placedCount++;
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
    int plantIndex1 = rand() % m_plantDatabase.size();
    int plantIndex2 = (plantIndex1 + rand() % (m_plantDatabase.size() - 1) + 1) % m_plantDatabase.size(); // Ensure different plants

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
                               "- Toxic parts: All parts, especially berries and leaves.\n"
                               "- Effects: Nausea, vomiting, dizziness, abdominal pain, respiratory distress, and possible cardiac arrest.\n"
                               "- Use: Not safe to consume.\n",
                               ":/dangerous/Plants/Danger_Plants/baneberry.jpg"});

    m_plantDatabase.push_back({"poisonous", "Death Camas",
                               "- Toxic parts: Bulbs and leaves.\n"
                               "- Effects: Vomiting, diarrhea, seizures, slow heart rate, and respiratory failure.\n"
                               "- Use: Not safe to consume.\n"
                               " Easily mistaken for edible wild onions, but highly toxic.\n",
                               ":/dangerous/Plants/Danger_Plants/death_camas.jpg"});

    m_plantDatabase.push_back({"poisonous", "Water Hemlock",
                               "- Toxic parts: Roots, stems, and leaves.\n"
                               "- Effects: Seizures, nausea, vomiting, respiratory paralysis, and death.\n"
                               "- Use: Not safe under any circumstances.\n"
                               " One of the most toxic plants in North America; even small amounts are lethal.\n",
                               ":/dangerous/Plants/Danger_Plants/water_hemlock.jpg"});

    m_plantDatabase.push_back({"poisonous", "Poison Ivy",
                               "- Toxic parts: All parts contain urushiol oil.\n"
                               "- Effects: Severe itching, rash, blisters, and allergic reactions upon contact.\n"
                               "- Use: Not used medicinally; avoid skin contact.\n"
                               " The oil can linger on clothing, tools, and pet fur.\n",
                               ":/dangerous/Plants/Danger_Plants/poison_ivy.jpg"});

    m_plantDatabase.push_back({"poisonous", "Jimson Weed",
                               "- Toxic parts: Seeds and leaves.\n"
                               "- Effects: Hallucinations, hyperthermia, dry mouth, delirium, heart arrhythmias, and death.\n"
                               "- Use: Historically used in spiritual rituals, but extremely dangerous and not recommended.\n"
                               " Contains potent tropane alkaloids such as atropine and scopolamine.\n",
                               ":/dangerous/Plants/Danger_Plants/jimson_weed.jpg"});

    m_plantDatabase.push_back({"poisonous", "Bittersweet Nightshade",
                               "- Toxic parts: Berries, leaves, and stems.\n"
                               "- Effects: Vomiting, diarrhea, slowed heartbeat, hallucinations, and paralysis.\n"
                               "- Use: Historically used in folk remedies, but extremely unsafe and not recommended.\n",
                               ":/dangerous/Plants/Danger_Plants/bittersweet_nightshade.jpg"});

    m_plantDatabase.push_back({"poisonous", "False Hellebore",
                               "- Toxic parts: All parts, especially roots and young shoots.\n"
                               "- Effects: Nausea, vomiting, low blood pressure, slowed heart rate, and death.\n"
                               "- Use: Not safe for any medicinal or nutritional purpose.\n"
                               " Sometimes mistaken for edible wild leeks in early spring.\n",
                               ":/dangerous/Plants/Danger_Plants/false_hellebore.jpg"});

    m_plantDatabase.push_back({"poisonous", "Lupine/Blue Bonnet",
                               "- Toxic parts: Seeds and young leaves.\n"
                               "- Effects: Tremors, labored breathing, birth defects (in livestock), and possible death.\n"
                               "- Use: Some species have historical medicinal use, but caution is strongly advised.\n"
                               " Toxic alkaloids vary by species and region.\n",
                               ":/dangerous/Plants/Danger_Plants/lupine_blue_bonnet.jpeg"});

    m_plantDatabase.push_back({"poisonous", "Monkshood/Wolfbane",
                               "- Toxic parts: All parts, especially roots and flowers.\n"
                               "- Effects: Numbness, nausea, cardiac arrest, and death within hours.\n"
                               "- Use: Occasionally used in ancient medicines, but highly lethal.\n"
                               " Absorption through skin is possible — even small contact can be dangerous.\n",
                               ":/dangerous/Plants/Danger_Plants/monkshood_wolfbane.jpg"});

    m_plantDatabase.push_back({"poisonous", "Myrtle Spurge",
                               "- Toxic parts: Milky sap from all parts.\n"
                               "- Effects: Skin and eye irritation, nausea, vomiting, and blistering.\n"
                               "- Use: No known safe uses; sap is highly caustic.\n"
                               " Classified as a noxious weed in several states due to its spread and toxicity.\n",
                               ":/dangerous/Plants/Danger_Plants/myrtle_spurge.jpg"});

    m_plantDatabase.push_back({"poisonous", "Poison Hemlock",
                               "- Toxic parts: All parts, especially roots and seeds.\n"
                               "- Effects: Tremors, paralysis, respiratory failure, and death.\n"
                               "- Use: Historically used as a sedative or poison, but fatally toxic.\n"
                               " Easily mistaken for wild carrots or parsley.\n",
                               ":/dangerous/Plants/Danger_Plants/poison_hemlock.jpg"});

    m_plantDatabase.push_back({"poisonous", "Snow on the Mountain",
                               "- Toxic parts: All parts, especially the milky sap.\n"
                               "- Effects: Skin irritation, blisters, and eye damage if contact occurs.\n"
                               "- Use: Not safe for internal or topical use.\n"
                               " The sap is a strong irritant; protective gloves should be worn when handling.\n",
                               ":/dangerous/Plants/Danger_Plants/snow_on_the_mountain.jpg"});

    m_plantDatabase.push_back({"poisonous", "Stinging Nettle",
                               "- Toxic parts: Tiny hairs on leaves and stems.\n"
                               "- Effects: Burning, itching, rash, and temporary numbness upon contact.\n"
                               "- Use: Despite irritation, leaves are edible when cooked and used for teas and soups.\n"
                               " Cooking or drying neutralizes the stinging compounds.\n",
                               ":/dangerous/Plants/Danger_Plants/stinging_nettle.jpg"});

    m_plantDatabase.push_back({"poisonous", "Wart Weed",
                               "- Toxic parts: Sap from leaves and stems.\n"
                               "- Effects: Skin blistering, irritation, and eye damage upon contact.\n"
                               "- Use: Historically applied to warts, but may cause more harm than benefit.\n"
                               " Not recommended due to caustic nature of its sap.\n",
                               ":/dangerous/Plants/Danger_Plants/wart_weed.jpg"});

    m_plantDatabase.push_back({"poisonous", "Oregon Grape",
                               "- Toxic parts: Roots and bark (in large amounts).\n"
                               "- Effects: Can cause gastrointestinal upset in high doses.\n"
                               "- Use: Root extracts sometimes used in herbal medicine for skin issues and digestion.\n"
                               " Berries are edible but very tart; often used in jams when prepared properly.\n",
                               ":/dangerous/Plants/Danger_Plants/oregon_grape.jpg"});

    // Beneficial herbs
    m_plantDatabase.push_back({"herb", "Creosote Bush",
                               "- Affected parts: Leaves and stems.\n"
                               "- Effects: May cause liver or kidney irritation in large amounts.\n"
                               "- Use: Traditionally used to boost the immune system and reduce symptoms of infections.\n"
                               " Has antimicrobial properties and was used by Indigenous peoples for colds and wounds.\n",
                               ":/safe/Plants/Safe_Plants/creosote_bush.jpg"});

    m_plantDatabase.push_back({"herb", "Golden Currant",
                               "- Affected parts: Berries and leaves.\n"
                               "- Effects: Non-toxic in moderate amounts.\n"
                               "- Use: Rich in antioxidants and vitamins; berries eaten fresh or dried.\n"
                               " Leaves brewed as tea to reduce inflammation and support immunity.\n",
                               ":/safe/Plants/Safe_Plants/golden_currant.jpg"});

    m_plantDatabase.push_back({"herb", "Mormon Tea",
                               "- Affected parts: Stems.\n"
                               "- Effects: Mild stimulant; can raise heart rate if consumed in large quantities.\n"
                               "- Use: Used to stop bleeding, reduce fever, and treat digestive issues.\n"
                               " Was historically brewed as tea.\n",
                               ":/safe/Plants/Safe_Plants/mormon_tea.jpg"});

    m_plantDatabase.push_back({"herb", "Osha",
                               "- Affected parts: Root.\n"
                               "- Effects: Generally safe in small doses.\n"
                               "- Use: Used for respiratory issues, sleep aid, and inflammation.\n"
                               " Often chewed or brewed into tea for sore throats and colds.\n"
                               " Caution: Resembles Water Hemlock.",
                               ":/safe/Plants/Safe_Plants/osha.jpg"});

    m_plantDatabase.push_back({"herb", "Prairie Flax",
                               "- Affected parts: Seeds and leaves.\n"
                               "- Effects: Non-toxic; excessive seed intake may cause digestive discomfort.\n"
                               "- Use: Seeds soothe digestion and freshen breath; leaves contain menthol to relieve nasal congestion.\n"
                               " Sometimes used for mild respiratory relief.\n"
                               " Caution: Resembles Lupine/Blue Bonnet.",
                               ":/safe/Plants/Safe_Plants/prairie_flax.jpg"});

    m_plantDatabase.push_back({"herb", "Prickly Pear Cactus",
                               "- Affected parts: Pads and fruit.\n"
                               "- Effects: Safe when de-spined and properly prepared.\n"
                               "- Use: Eaten for fiber and antioxidants; used to regulate blood sugar and hydration.\n"
                               " Pads and fruit are both edible and nutritionally beneficial.\n",
                               ":/safe/Plants/Safe_Plants/prickly_pear_cactus.jpg"});

    m_plantDatabase.push_back({"herb", "Sacred Thornapple",
                               "- Affected parts: All parts, especially seeds and leaves.\n"
                               "- Effects: Highly toxic; can cause hallucinations, dry mouth, rapid heartbeat, and death.\n"
                               "- Use: Historically used in small doses as a sedative or in spiritual rituals.\n"
                               " Use is extremely dangerous and not recommended.\n"
                               " Caution: Resembles Jimson Weed.",
                               ":/safe/Plants/Safe_Plants/sacred_thornapple.jpg"});

    m_plantDatabase.push_back({"herb", "Sagebrush",
                               "- Affected parts: Leaves and stems.\n"
                               "- Effects: Generally non-toxic; may cause mild skin irritation or allergic reaction.\n"
                               "- Use: Traditionally used in teas, smudging, and as a mild antiseptic.\n"
                               " Seeds were used to make flour and the wood is highly flammable.\n",
                               ":/safe/Plants/Safe_Plants/sagebrush.jpg"});

    m_plantDatabase.push_back({"herb", "Snowberry",
                               "- Affected parts: Berries.\n"
                               "- Effects: Mildly toxic; may cause vomiting or dizziness if eaten in large amounts.\n"
                               "- Use: Berries used externally to treat skin issues; bark and leaves used in traditional medicine.\n"
                               " Often used as a poultice for cuts and burns.\n",
                               ":/safe/Plants/Safe_Plants/snowberry.jpg"});

    m_plantDatabase.push_back({"herb", "Utah Juniper",
                               "- Affected parts: Berries and leaves.\n"
                               "- Effects: May cause kidney irritation in large amounts.\n"
                               "- Use: Berries used for treating colds and digestive issues; leaves used as poultice for wounds.\n"
                               " Traditionally brewed as tea or burned for purification.\n",
                               ":/safe/Plants/Safe_Plants/utah_juniper.jpg"});

    m_plantDatabase.push_back({"herb", "Yarrow",
                               "- Affected parts: Leaves and flowers.\n"
                               "- Effects: Supports sweating for detox; calming effects may ease anxiety and depression; aids with allergies, digestion, hormone balance, circulation, and urinary health.\n"
                               "- Use: Internally for fever, menstrual regulation, UTIs, and gastrointestinal relief. Externally to stop bleeding, speed wound healing, soothe bites, treat eczema, and repel insects.\n"
                               " Caution: Resembles Poison Hemlock.",
                               ":/safe/Plants/Safe_Plants/yarrow.jpg"});

}
