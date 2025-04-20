#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>

// Enumeration for the various states of the game
enum GameState {
    MainMenu,
    Tutorial,
    Level1,
    Level2,
    Level3,
    GameOver,
    GameClear
};

// The GameManager class handles the game state, player score, and health.
class GameManager : public QObject
{
    Q_OBJECT
private:
    GameState m_gameState; // Current state of the game
    int m_score;           // Player's score
    int m_health;          // Player's health (e.g., max 100)
    int m_currentLevel;

    struct PlantTracking {
        int collected = 0;
        int total = 0;
    };

    std::map<QString, PlantTracking> m_plantsToCollect; // Tracks plants to collect by name
    int m_poisonousCollected = 0;                       // Count of poisonous plants collected
    int m_maxPoisonousAllowed = 3;                      // Maximum poisonous plants allowed
    QStringList m_levelPlants[3];

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

    // Set the game state to tutorial
    void tutorial();

    // Updates the player's score by a delta value
    void updateScore(int delta);

    // Reduces player's health by the specified amount
    void damage(int amount);

    void resetGame();

    void gameClear();

    void nextLevel();
    int currentLevel();
    void startSpecificLevel(int level);

    void setupLevelPlants(int level);
    bool collectPlant(const QString& plantName, bool isPoisonous);
    bool isLevelComplete() const;
    int poisonousCollected() const { return m_poisonousCollected; }
    int maxPoisonousAllowed() const { return m_maxPoisonousAllowed; }
    const std::map<QString, PlantTracking>& plantsToCollect() const { return m_plantsToCollect; }

public slots:
    // Called every frame (if needed for periodic updates)
    void update();

signals:
    // Emitted when the game state changes
    void stateChanged(GameState newState);
};

#endif // GAMEMANAGER_H
