/**
 * @file WorldRenderer.h
 * @brief Defines the WorldRenderer class that renders the game world and handles user interactions.
 *
 * @author Jason Chang
 *
 * Checked by
 */

#ifndef WORLDRENDERER_H
#define WORLDRENDERER_H

#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include "physicsworld.h"
#include "gameManager.h"
#include "gameContactListener.h"

/**
 * @brief WorldRenderer class responsible for rendering the game world and handling user input
 */
class WorldRenderer : public QWidget {
    Q_OBJECT

private:

    PhysicsWorld *m_physicsWorld;               // Physics simulation engine
    QTimer *m_timer;                            // Timer for rendering at fixed intervals (60 FPS)
    float m_scale;                              // Pixels per meter (used for rendering)

    GameManager *m_gameManager;                 // Game state manager
    GameContactListener *m_contactListener;     // Collision event handler
    QPixmap m_background;                       // Background image


public:

    /**
     * @brief Constructor for WorldRenderer
     * @param parent Parent widget
     */
    explicit WorldRenderer(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~WorldRenderer();

    /**
     * @brief Resets the game to its initial state.
     */
    void resetGame();

    /**
     * @brief Resumes the game timer.
     */
    void resumeGame();

    /**
     * @brief Pause the game timer.
     */
    void pauseGame();

    /**
     * @brief Accessor for the game manager
     * @return Pointer to the game manager
     */
    GameManager* gameManager() const { return m_gameManager; }


public slots:

    /**
     * @brief Updates game state and physics, called on timer tick
     */
    void updateGameState();

protected:

    /**
     * @brief Called when the screen needs to be repainted
     * @param event Paint event information
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Handle key press input
     * @param event Key event information
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief handle key release input
     * @param event Key event information
     */
    void keyReleaseEvent(QKeyEvent *event) override;

    /**
     * @brief Shows a plant popup when player contacts a plant
     * @param plant The plant object contacted
     */
    void showPlantPopup(Hazard* plant);

private:

    /**
     * @brief Convert world coordinates to screen coordinates.
     * @param x X coordinate in world space
     * @param y Y coordinate in world space
     * @return
     */
    QPointF worldToScreen(float x, float y);

    /**
     * @brief Convert world coordinates to screen (pixel) coordinates
     * @param position Vector in world space
     * @return Point in screen space
     */
    QPointF worldToScreen(const b2Vec2 &position);
};

#endif // WORLDRENDERER_H
