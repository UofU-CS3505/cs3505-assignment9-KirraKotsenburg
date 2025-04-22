/**
 * @file Vehicle.cpp
 * @brief Defines the Vehicle class that manages Box2D vehicle object.
 *
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Jay Lee
 */

#ifndef VEHICLE_H
#define VEHICLE_H

#include <box2d/box2d.h>

/**
 * @brief Represents a simple two-wheeled vehicle with drive functionality.
 */
class Vehicle {

private:

    b2Body *m_chassis;          // The main body of the vehicle
    b2Body *m_wheels[2];        // Wheels: index 0 = front, index 1 = rear
    b2Joint *m_wheelJoints[2];  // Joints connecting wheels to chassis

public:

    /**
     * @brief Constructor: creates the chassis and wheels at the given position
     * @param world Box2D world where the vehicle will be created
     * @param position Initial position for the vehicle
     */
    Vehicle(b2World &world, const b2Vec2 &position);

    /**
     * @brief Applies horizontal drive force to the wheels
     * @param force Amount of force to apply
     */
    void applyDriveForce(float force);

    /**
     * @brief Accessor for the chassis body
     * @return Pointer to the chassis body
     */
    b2Body *getChassis() const;

    /**
     * @brief Accessor for a specific wheel
     * @param index Wheel index (0 = left, 1 = right)
     * @return Pointer to the wheel body
     */
    b2Body *getWheel(int index) const;

    /**
     * @brief Resets the vehicle to a specific position
     * @param position New position for the vehicle
     */
    void reset(const b2Vec2& position);

};

#endif // VEHICLE_H
