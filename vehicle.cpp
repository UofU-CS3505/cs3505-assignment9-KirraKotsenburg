#include "vehicle.h"

Vehicle::Vehicle(b2World &world, const b2Vec2 &position)
    : m_chassis(nullptr)
    , m_wheels{nullptr, nullptr}
{
    // Define and create the chassis body
    b2BodyDef chassisBodyDef;
    chassisBodyDef.type = b2_dynamicBody;
    chassisBodyDef.position = position;
    m_chassis = world.CreateBody(&chassisBodyDef);

    // Define chassis shape as a rectangle (2m wide, 1m tall)
    b2PolygonShape chassisShape;
    chassisShape.SetAsBox(1.0f, 0.5f);

    b2FixtureDef chassisFixture;
    chassisFixture.shape = &chassisShape;
    chassisFixture.density = 1.5f;
    chassisFixture.friction = 1.0f;
    m_chassis->CreateFixture(&chassisFixture);

    // Define common wheel properties
    b2BodyDef wheelBodyDef;
    wheelBodyDef.type = b2_dynamicBody;
    wheelBodyDef.position = position;

    // --- Front Wheel ---
    m_wheels[0] = world.CreateBody(&wheelBodyDef);
    b2CircleShape frontWheelShape;
    frontWheelShape.m_radius = 0.3f;
    b2FixtureDef frontWheelFixture;
    frontWheelFixture.shape = &frontWheelShape;
    frontWheelFixture.density = 1.0f;
    frontWheelFixture.friction = 2.0f;
    m_wheels[0]->CreateFixture(&frontWheelFixture);

    // --- Rear Wheel ---
    m_wheels[1] = world.CreateBody(&wheelBodyDef);
    b2CircleShape rearWheelShape;
    rearWheelShape.m_radius = 0.3f;
    b2FixtureDef rearWheelFixture;
    rearWheelFixture.shape = &rearWheelShape;
    rearWheelFixture.density = 1.0f;
    rearWheelFixture.friction = 2.0f;
    m_wheels[1]->CreateFixture(&rearWheelFixture);

    // Define revolute joints to attach wheels to chassis
    b2RevoluteJointDef jointDef;
    jointDef.bodyA = m_chassis;
    jointDef.enableMotor = true;
    jointDef.maxMotorTorque = 1000.0f;

    // --- Front Wheel Joint ---
    jointDef.bodyB = m_wheels[0];
    jointDef.localAnchorA.Set(1.0f, -0.5f);  // right side
    jointDef.localAnchorB.SetZero();
    m_wheelJoints[0] = world.CreateJoint(&jointDef);

    // --- Rear Wheel Joint ---
    jointDef.bodyB = m_wheels[1];
    jointDef.localAnchorA.Set(-1.0f, -0.5f); // left side
    jointDef.localAnchorB.SetZero();
    m_wheelJoints[1] = world.CreateJoint(&jointDef);
}

void Vehicle::ApplyDriveForce(float force)
{
    // Apply horizontal force to both wheels (simple traction)
    m_wheels[0]->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
    m_wheels[1]->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
}

void Vehicle::ApplySteering(float angle)
{
    if (m_wheelJoints[0]) {
        m_wheelJoints[0]->SetUserData(reinterpret_cast<void *>(static_cast<intptr_t>(angle)));
    }
}

b2Body *Vehicle::GetChassis() const
{
    return m_chassis;
}

b2Body *Vehicle::GetWheel(int index) const
{
    return m_wheels[index];
}
