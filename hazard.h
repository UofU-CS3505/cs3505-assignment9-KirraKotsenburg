#ifndef HAZARD_H
#define HAZARD_H

#include <box2d/box2d.h>

// Represents a hazardous object (e.g., poisonous plant) in the Box2D world
// hazard.h
#include <QString>

class Hazard {
public:
    Hazard(b2World &world, const b2Vec2 &position, float radius,
           const QString &type, const QString &desc, const QString &imgPath);

    ~Hazard();
    b2Body *getBody() const { return m_body; }

    QString type() const { return m_type; }
    QString description() const { return m_description; }
    QString imagePath() const { return m_imagePath; }
    QString plantName() const { return m_plantName; }

private:
    b2Body *m_body;
    QString m_type;        // "poisonous" or "safe"
    QString m_description;
    QString m_imagePath;
    QString m_plantName;
};


#endif // HAZARD_H

