#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>

// Enumeration for the various states of the game
enum GameState {
    MainMenu,
    Tutorial,
    Playing,
    GameOver
};

// The GameManager class handles the game state, player score, and health.
class GameManager : public QObject
{
    Q_OBJECT
private:
    GameState m_gameState; // Current state of the game
    int m_score;           // Player's score
    int m_health;          // Player's health (e.g., max 100)

public:
    explicit GameManager(QObject *parent = nullptr);

    // Getters
    GameState gameState() const { return m_gameState; }
    int score() const { return m_score; }
    int health() const { return m_health; }

    // Starts a new game by resetting state, score, and health
    void startGame();

    // Sets the game state to GameOver and emits state change
    void gameOver();

    // Updates the player's score by a delta value
    void updateScore(int delta);

    // Reduces player's health by the specified amount
    void damage(int amount);

    void resetGame();


public slots:
    // Called every frame (if needed for periodic updates)
    void update();

signals:
    // Emitted when the game state changes
    void stateChanged(GameState newState);
};

#endif // GAMEMANAGER_H
