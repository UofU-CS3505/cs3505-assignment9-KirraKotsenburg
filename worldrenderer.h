#ifndef WORLDRENDERER_H
#define WORLDRENDERER_H

#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include "physicsworld.h"
#include "gameManager.h"          // Game state manager
#include "gameContactListener.h"  // Collision event handler

// Responsible for rendering the game world and handling user input
class WorldRenderer : public QWidget
{
    Q_OBJECT

private:
    PhysicsWorld *m_physicsWorld; // Physics simulation engine
    QTimer *m_timer;              // Timer for rendering at fixed intervals (60 FPS)
    float m_scale;                // Pixels per meter (used for rendering)

    GameManager *m_gameManager;   // Game state manager
    GameContactListener *m_contactListener;
    QPixmap m_background;


public:
    explicit WorldRenderer(QWidget *parent = nullptr);
    ~WorldRenderer();
    void resetGame();
    void resumeGame();
    void pauseGame();
    GameManager* gameManager() const { return m_gameManager; }


public slots:
    void updateGameState();

protected:
    void paintEvent(QPaintEvent *event) override;     // Called when the screen needs to be repainted
    void keyPressEvent(QKeyEvent *event) override;     // Handle key press input
    void keyReleaseEvent(QKeyEvent *event) override;   // Handle key release input
    void showPlantPopup(Hazard* hazard);

private:
    // Convert world coordinates to screen (pixel) coordinates
    QPointF worldToScreen(float x, float y);
    QPointF worldToScreen(const b2Vec2 &position);
};

#endif // WORLDRENDERER_H
