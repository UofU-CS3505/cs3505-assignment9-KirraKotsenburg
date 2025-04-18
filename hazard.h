#ifndef HAZARD_H
#define HAZARD_H

#include <box2d/box2d.h>
#include <QString>
// Represents a hazardous object (e.g., poisonous plant) in the Box2D world
class Hazard
{
private:
    b2Body *m_body;  // The physical body representing the hazard
    float m_radius;
    QString m_type;
    QString m_description;
    QString m_imagePath;
    QString m_plantName;


public:
    Hazard(b2World &world, const b2Vec2 &position, float radius,
           const QString &type, const QString &desc, const QString &imgPath);

    // Destructor (does not destroy body; recommended to handle externally)
    ~Hazard();

    // Getter for the Box2D body
    b2Body* getBody() const { return m_body; }
    QString type() const { return m_type; }
    QString description() const { return m_description; }
    QString imagePath() const { return m_imagePath; }
    QString plantName() const { return m_plantName; }

    void reset();

};

#endif // HAZARD_H
