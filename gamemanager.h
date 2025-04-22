/**
 * @file GameManager.h
 * @brief Defines the GameManager class that handles game state and player progression
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Kirra Kotsenburg
 */

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>

/**
 * @brief Enumeration for the various states of the game.
 */
enum GameState {
    MainMenu,
    Tutorial,
    Level1,
    Level2,
    Level3,
    GameOver,
    GameClear
};

/**
 * @brief The GameManager class handles the game state, player score, and health.
 */
class GameManager : public QObject {
    Q_OBJECT

private:

    GameState m_gameState; // Current state of the game
    int m_score;           // Player's score
    int m_health;          // Player's health
    int m_currentLevel;    // Current level number

    /**
     * @brief Structure to track plant collection progress.
     */
    struct plantTracking {
        int collected = 0;
        int total = 0;
    };

    std::map<QString, plantTracking> m_plantsToCollect; // Tracks plants to collect by name
    int m_poisonousCollected = 0;                       // Count of poisonous plants collected
    int m_maxPoisonousAllowed = 3;                      // Maximum poisonous plants allowed
    QStringList m_levelPlants[3];                       // Plants for each level

public:

    /**
     * @brief Constructor for the Game Manager.
     * @param parent parent QObject.
     */
    explicit GameManager(QObject *parent = nullptr);

    /**
     * @brief Get the current game state.
     * @return Current game state.
     */
    GameState gameState() const { return m_gameState; }

    /**
     * @brief Get the player's score.
     * @return Current score.
     */
    int score() const { return m_score; }

    /**
     * @brief Get the player's health.
     * @return Current health.
     */
    int health() const { return m_health; }

    /**
     * @brief Starts a new game by resetting state, score, and health.
     */
    void startGame();

    /**
     * @brief Sets the game state to GameOver and emits state change.
     */
    void gameOver();

    /**
     * @brief Set the game state to tutorial.
     */
    void tutorial();

    /**
     * @brief Updates the player's score.
     * @param delta Amount of damage to apply.
     */
    void updateScore(int delta);

    /**
     * @brief Reduce player's health by specific amount.
     * @param amount Amount of damage to apply.
     */
    void damage(int amount);

    /**
     * @brief Reset the game to its initial state.
     */
    void resetGame();

    /**
     * @brief Set the game state to GameOver.
     */
    void gameClear();

    /**
     * @brief Advance to next level.
     */
    void nextLevel();

    /**
     * @brief Get the current level number.
     * @return Current level number.
     */
    int currentLevel();

    /**
     * @brief Start game in specific level. Default 1
     * @param level Level to start.
     */
    void startSpecificLevel(int level);

    /**
     * @brief Setup plants list for specific level.
     * @param level Level number.
     */
    void setupLevelPlants(int level);

    /**
     * @brief Record a plant collection event.
     * @param plantName name of the collected plant.
     * @param isPoisonous Check whether the plant is poisonous.
     * @return True if collection was successful. False otherwise.
     */
    bool collectPlant(const QString& plantName, bool isPoisonous);

    /**
     * @brief Check if all required plants for the level have been collected.
     * @return True if current level cleared. False otherwise.
     */
    bool isLevelComplete() const;

    /**
     * @brief Check if any poisonous plants collected and get the number of poisonous plants collected.
     * @return Number of poisonous plants collected.
     */
    int poisonousCollected() const { return m_poisonousCollected; }

    /**
     * @brief Get the maximum number of poisonous plants allowed.
     * @return Maximum poisonous plants allowed
     */
    int maxPoisonousAllowed() const { return m_maxPoisonousAllowed; }

    /**
     * @brief Get the map of plants that need to be collected.
     * @return Reference to the plant collection tracking map
     */
    const std::map<QString, plantTracking>& plantsToCollect() const { return m_plantsToCollect; }

public slots:

    /**
     * @brief Called every frame for periodic updates
     */
    void update();

signals:

    /**
     * @brief Emitted when the game state changes.
     * @param newState The new game state.
     */
    void stateChanged(GameState newState);
};

#endif // GAMEMANAGER_H
