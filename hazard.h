#ifndef HAZARD_H
#define HAZARD_H

#include <box2d/box2d.h>

// Represents a hazardous object (e.g., poisonous plant) in the Box2D world
class Hazard
{
private:
    b2Body *m_body;  // The physical body representing the hazard

public:
    // Creates a static circular sensor body at the given position
    Hazard(b2World &world, const b2Vec2 &position, float radius = 0.5f);

    // Destructor (does not destroy body; recommended to handle externally)
    ~Hazard();

    // Getter for the Box2D body
    b2Body* getBody() const { return m_body; }

    void reset();
};

#endif // HAZARD_H
