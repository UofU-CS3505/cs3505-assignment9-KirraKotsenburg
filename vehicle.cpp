#include "vehicle.h"

Vehicle::Vehicle(b2World &world, const b2Vec2 &position)
    : m_chassis(nullptr), m_wheels{nullptr, nullptr}, m_wheelJoints{nullptr, nullptr}
{
    // --- Chassis ---
    b2BodyDef chassisBodyDef;
    chassisBodyDef.type = b2_dynamicBody;
    chassisBodyDef.position = position;
    chassisBodyDef.fixedRotation = true; // Fix the car from turning.
    m_chassis = world.CreateBody(&chassisBodyDef);

    b2PolygonShape chassisShape;
    chassisShape.SetAsBox(1.2f, 0.3f);

    b2FixtureDef chassisFixture;
    chassisFixture.shape = &chassisShape;
    chassisFixture.density = 1.5f;
    chassisFixture.friction = 1.0f;
    m_chassis->CreateFixture(&chassisFixture);

    // --- Wheels ---
    float wheelRadius = 0.15f;       // Radius of the wheels
    float axleOffsetY = -0.4f;       // Vertical offset from the chassis to the axle

    for (int i = 0; i < 2; ++i) {    // Loop for left (0) and right (1) wheels
        b2BodyDef wheelBodyDef;
        wheelBodyDef.type = b2_dynamicBody;  // Wheels are dynamic (can move and rotate)

        // Position the wheels on left (-0.9) and right (+0.9) relative to chassis
        wheelBodyDef.position = position + b2Vec2((i == 0 ? -0.9f : 0.9f), axleOffsetY);

        // Create the wheel body in the Box2D world
        m_wheels[i] = world.CreateBody(&wheelBodyDef);

        // Define the circular shape of the wheel
        b2CircleShape wheelShape;
        wheelShape.m_radius = wheelRadius;

        // Create the fixture (physical properties) for the wheel
        b2FixtureDef wheelFixture;
        wheelFixture.shape = &wheelShape;
        wheelFixture.density = 1.0f;     // Mass density
        wheelFixture.friction = 2.0f;    // High friction for better traction
        m_wheels[i]->CreateFixture(&wheelFixture);

        // Define the joint that connects the wheel to the chassis
        b2WheelJointDef jointDef;
        jointDef.bodyA = m_chassis;       // Chassis is the base
        jointDef.bodyB = m_wheels[i];     // Wheel is the connected body
        jointDef.localAnchorA.Set((i == 0 ? -0.9f : 0.9f), axleOffsetY);  // Mounting point on chassis
        jointDef.localAnchorB.SetZero();  // Mounting point on wheel (center)
        jointDef.localAxisA.Set(0.0f, 1.0f); // Axis of suspension (vertical)

        // Enable motor to allow driving control
        jointDef.enableMotor = true;
        jointDef.motorSpeed = 0.0f;           // Initial speed is 0
        jointDef.maxMotorTorque = 100.0f;     // Maximum force to apply from motor

        // Create the joint in the Box2D world and store it
        m_wheelJoints[i] = (b2WheelJoint*)world.CreateJoint(&jointDef);
    }
}

void Vehicle::ApplyDriveForce(float force)
{
    for (int i = 0; i < 2; ++i) {
        m_wheels[i]->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
    }
}

void Vehicle::ApplySteering(float angle)
{
    // No-op: Steering not implemented in this model
}

b2Body* Vehicle::GetChassis() const
{
    return m_chassis;
}

b2Body* Vehicle::GetWheel(int index) const
{
    return (index >= 0 && index < 2) ? m_wheels[index] : nullptr;
}

void Vehicle::Reset(const b2Vec2& position) {
    const float forwardOffset = 2.0f;
    b2Vec2 newPosition(position.x + forwardOffset, position.y);

    // Reset chassis
    m_chassis->SetTransform(newPosition, 0.0f);
    m_chassis->SetLinearVelocity(b2Vec2_zero);
    m_chassis->SetAngularVelocity(0.0f);

    for (int i = 0; i < 2; ++i) {
        b2Vec2 wheelPos = newPosition;
        wheelPos.x += (i == 0 ? -0.9f : 0.9f);
        wheelPos.y -= 0.4f;

        m_wheels[i]->SetTransform(wheelPos, 0.0f);
        m_wheels[i]->SetLinearVelocity(b2Vec2_zero);
        m_wheels[i]->SetAngularVelocity(0.0f);
    }
}
