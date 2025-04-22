/**
 * @file Hazard.h
 * @brief Defines the Hazard class that represents hazardous objects in the Box2D world
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Kirra Kotsenburg, Jay Lee
 */

#ifndef HAZARD_H
#define HAZARD_H

#include <box2d/box2d.h>
#include <QString>

/**
 * @brief Represents a hazardous object(poisonous plant) in the Box2D world
 */
class Hazard {

private:

    b2Body *m_body;        // The physical body representing the hazard
    float m_radius;        // Radius of the circular collision shape
    QString m_type;        // Type of hazard (e.g. "poisonous", "herbal")
    QString m_description; // Description of the hazard
    QString m_imagePath;   // Path to the image file for this hazard
    QString m_plantName;   // Name of the plant this hazard represents

public:

    /**
     * @brief Constructor for creating a hazard
     * @param world The Box2D world to create the hazard in
     * @param position The position of the hazard
     * @param radius The radius of the circular collision shape
     * @param type The type of the hazard
     * @param plantName The name of the plant
     * @param desc Description of the hazard
     * @param imgPath Path to the image file
     */
    Hazard(b2World &world, const b2Vec2 &position, float radius,
           const QString &type, const QString &plantName, const QString &desc, const QString &imgPath);

    /**
     * @brief Destructor of Hazard class
     */
    ~Hazard();

    /**
     * @brief Get the Box2D body of this hazard
     * @return Pointer to the Box2D body
     */
    b2Body* getBody() const { return m_body; }

    /**
     * @brief Get the type of this hazard
     * @return The hazard type
     */
    QString type() const { return m_type; }

    /**
     * @brief Get the description of this hazard
     * @return The hazard description
     */
    QString description() const { return m_description; }

    /**
     * @brief Get the image path for this hazard
     * @return The path to the image file
     */
    QString imagePath() const { return m_imagePath; }

    /**
     * @brief Get the plant name for this hazard
     * @return The name of the plant
     */
    QString plantName() const { return m_plantName; }

    /**
     * @brief Reset the hazard to its original state
     */
    void reset();
};

#endif // HAZARD_H
