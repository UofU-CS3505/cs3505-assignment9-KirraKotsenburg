/**
 * @file GameManager.cpp
 * @brief Implementation of the GameManager class
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Kirra Kotsenburg, Jay Lee
 */

#include "gameManager.h"

GameManager::GameManager(QObject *parent): QObject(parent),
                                            m_gameState(MainMenu),
                                            m_score(0),
                                            m_health(1),
                                            m_currentLevel(1) { }

void GameManager::startGame() {
    m_gameState = Level1;
    m_currentLevel = 1;
    m_score = 0;
    m_health = 1;
    emit stateChanged(m_gameState);
}

void GameManager::gameOver() {
    m_gameState = GameOver;
    emit stateChanged(m_gameState);
}

void GameManager::tutorial() {
    m_gameState = Tutorial;
    emit stateChanged(m_gameState);
}

void GameManager::updateScore(int delta) {
    m_score += delta;
}

void GameManager::damage(int amount) {
    m_health -= amount;
    if (m_health <= 0) {
        m_health = 0; // Prevent negative health
        gameOver();   // Trigger game over state
    }
}

void GameManager::update() { }

void GameManager::resetGame() {
    startGame(); // Reset by starting a new game
}

void GameManager::gameClear() {
    m_gameState = GameClear;
    emit stateChanged(m_gameState);
}

void GameManager::startSpecificLevel(int level) {
    m_currentLevel = level;

    // Set appropriate game state based on level number
    switch(level) {
    case 1: m_gameState = Level1; break;
    case 2: m_gameState = Level2; break;
    case 3: m_gameState = Level3; break;
    default: m_gameState = Level1; break; // Default to level 1 if invalid
    }

    m_score = 0;    // Reset score for new level
    m_health = 1;   // Reset health for new level
    setupLevelPlants(level); // Set up the plants for this level
    emit stateChanged(m_gameState);
}

void GameManager::nextLevel() {
    m_currentLevel++;

    // Cap at level 3
    if (m_currentLevel > 3) {
        m_currentLevel = 3;
    }

    // Update game state based on current state
    switch(m_gameState) {
    case Level1: m_gameState = Level2; break;
    case Level2: m_gameState = Level3; break;
    default: break;  // Already at max level or not in a level state
    }

    // Keep score and health from previous level
    emit stateChanged(m_gameState);
}

int GameManager::currentLevel() {
    return m_currentLevel;
}

void GameManager::setupLevelPlants(int level) {
    // Clear previous tracking
    m_plantsToCollect.clear();
    m_poisonousCollected = 0;

    // Initialize level plants only once
    if (m_levelPlants[0].isEmpty()) {
        m_levelPlants[0] = {"Golden Currant", "Mormon Tea", "Creosote Bush"};  // Level 1 plants
        m_levelPlants[1] = {"Golden Currant", "Mormon Tea", "Creosote Bush", "Osha", "Prairie Flax"};  // Level 2 plants
        m_levelPlants[2] = {"Golden Currant", "Mormon Tea", "Creosote Bush", "Osha", "Prairie Flax",
                            "Prickly Pear Cactus", "Sagebrush"};  // Level 3 plants
    }

    // Get appropriate plant list for current level (0-based index)
    QStringList& plantsForLevel = m_levelPlants[level - 1];

    // Initialize tracking for these plants
    for (const QString& plantName : plantsForLevel) {
        m_plantsToCollect[plantName] = {0, 1};  // Initially 0 collected out of 1 required
    }
}

bool GameManager::collectPlant(const QString& plantName, bool isPoisonous) {
    if (isPoisonous) {
        m_poisonousCollected++;

        // Check if exceeding poison limit - game over condition
        if (m_poisonousCollected >= m_maxPoisonousAllowed) {
            gameOver();
            return false;
        }
        return true;
    }

    // Update collection status for the plant
    if (m_plantsToCollect.count(plantName) > 0) {
        m_plantsToCollect[plantName].collected++;

        // Give points based on current level
        updateScore(10 * m_currentLevel);
        return true;
    }

    return false; // Plant not in collection list
}

bool GameManager::isLevelComplete() const {
    // Check if all required plants have been collected
    for (const auto& pair : m_plantsToCollect) {
        if (pair.second.collected < pair.second.total) {
            return false; // At least one plant not fully collected
        }
    }
    return true; // All plants collected
}
