// vehicle.cpp
#include "vehicle.h"

Vehicle::Vehicle(b2World& world, const b2Vec2& position)
    : m_chassis(nullptr),
    m_wheels{nullptr, nullptr}
{
    // Create chassis body definition
    b2BodyDef chassisBodyDef;
    chassisBodyDef.type = b2_dynamicBody;
    chassisBodyDef.position = position;

    // Create chassis body
    m_chassis = world.CreateBody(&chassisBodyDef);

    // Define chassis shape (assuming a rectangular chassis)
    b2PolygonShape chassisShape;
    chassisShape.SetAsBox(1.0f, 0.5f);  // Width: 2m, Height: 1m

    // Create chassis fixture
    b2FixtureDef chassisFixture;
    chassisFixture.shape = &chassisShape;
    chassisFixture.density = 1.0f;
    chassisFixture.friction = 0.3f;
    m_chassis->CreateFixture(&chassisFixture);

    // Create wheel bodies
    b2BodyDef wheelBodyDef;
    wheelBodyDef.type = b2_dynamicBody;
    wheelBodyDef.position = position;

    // Front wheel
    m_wheels[0] = world.CreateBody(&wheelBodyDef);
    b2CircleShape frontWheelShape;
    frontWheelShape.m_radius = 0.5f;  // Wheel radius
    b2FixtureDef frontWheelFixture;
    frontWheelFixture.shape = &frontWheelShape;
    frontWheelFixture.density = 0.5f;
    frontWheelFixture.friction = 1.0f;
    m_wheels[0]->CreateFixture(&frontWheelFixture);

    // Rear wheel
    m_wheels[1] = world.CreateBody(&wheelBodyDef);
    b2CircleShape rearWheelShape;
    rearWheelShape.m_radius = 0.5f;  // Wheel radius
    b2FixtureDef rearWheelFixture;
    rearWheelFixture.shape = &rearWheelShape;
    rearWheelFixture.density = 0.5f;
    rearWheelFixture.friction = 1.0f;
    m_wheels[1]->CreateFixture(&rearWheelFixture);

    // Create wheel joints to connect wheels to chassis
    b2RevoluteJointDef jointDef;
    jointDef.bodyA = m_chassis;
    jointDef.enableMotor = true;
    jointDef.maxMotorTorque = 1000.0f;

    // Front wheel joint
    jointDef.bodyB = m_wheels[0];
    jointDef.localAnchorA.Set(1.0f, -0.5f);  // Adjust anchor points as needed
    jointDef.localAnchorB.SetZero();
    m_wheelJoints[0] = world.CreateJoint(&jointDef);

    // Rear wheel joint
    jointDef.bodyB = m_wheels[1];
    jointDef.localAnchorA.Set(-1.0f, -0.5f);  // Adjust anchor points as needed
    jointDef.localAnchorB.SetZero();
    m_wheelJoints[1] = world.CreateJoint(&jointDef);
}

void Vehicle::ApplyDriveForce(float force)
{
    // Apply drive force to both wheels
    m_wheels[0]->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
    m_wheels[1]->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
}

void Vehicle::ApplySteering(float angle)
{
    // Rotate the front wheel joint to steer
    if (m_wheelJoints[0]) {
        m_wheelJoints[0]->SetUserData(reinterpret_cast<void*>(static_cast<intptr_t>(angle)));
    }
}

b2Body* Vehicle::GetChassis() const
{
    return m_chassis;
}

b2Body* Vehicle::GetWheel(int index) const
{
    return m_wheels[index];
}
