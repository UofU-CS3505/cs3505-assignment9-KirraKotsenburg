// vehicle.h
#ifndef VEHICLE_H
#define VEHICLE_H

#include <box2d/box2d.h>

class Vehicle {
private:
    b2Body* m_chassis;
    b2Body* m_wheels[2]; // Front and rear wheels
    b2Joint* m_wheelJoints[2];

public:
    Vehicle(b2World& world, const b2Vec2& position);

    void ApplyDriveForce(float force);
    void ApplySteering(float angle);

    b2Body* GetChassis() const;
    b2Body* GetWheel(int index) const;
};

#endif // VEHICLE_H
