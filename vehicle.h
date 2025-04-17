#ifndef VEHICLE_H
#define VEHICLE_H

#include <box2d/box2d.h>

// Represents a simple two-wheeled vehicle with steering and drive functionality
class Vehicle
{
private:
    b2Body *m_chassis;          // The main body of the vehicle
    b2Body *m_wheels[2];        // Wheels: index 0 = front, index 1 = rear
    b2Joint *m_wheelJoints[2];  // Joints connecting wheels to chassis

public:
    // Constructor: creates the chassis and wheels at the given position
    Vehicle(b2World &world, const b2Vec2 &position);

    // Applies horizontal drive force to the wheels
    void ApplyDriveForce(float force);

    // Applies steering by updating front wheel joint's user data
    void ApplySteering(float angle);

    // Accessor for the chassis body
    b2Body *GetChassis() const;

    // Accessor for a specific wheel (0 = front, 1 = rear)
    b2Body *GetWheel(int index) const;

    void Reset(const b2Vec2& position);

};

#endif // VEHICLE_H
