#ifndef WORLDRENDERER_H
#define WORLDRENDERER_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include "physicsworld.h"

class WorldRenderer : public QWidget {
    Q_OBJECT

private:
    PhysicsWorld* m_physicsWorld;
    QTimer* m_timer;
    float m_scale;  // Pixels per meter

public:
    WorldRenderer(QWidget* parent = nullptr);
    ~WorldRenderer();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    QPointF worldToScreen(float x, float y);
    QPointF worldToScreen(const b2Vec2& position);};

#endif // WORLDRENDERER_H
