#include "gameManager.h"

GameManager::GameManager(QObject *parent)
    : QObject(parent),
    m_gameState(MainMenu),
    m_score(0),
    m_health(1),
    m_currentLevel(1)  // Initialize to level 1
{
}

// Start or restart the game: reset score and health, switch to Playing state
void GameManager::startGame() {
    m_gameState = Level1;
    m_currentLevel = 1;
    m_score = 0;
    m_health = 1;
    emit stateChanged(m_gameState);
}

// End the game and change state to GameOver
void GameManager::gameOver() {
    m_gameState = GameOver;
    emit stateChanged(m_gameState);
}

void GameManager::tutorial() {
    m_gameState = Tutorial;
    emit stateChanged(m_gameState);
}

// Increase or decrease the score
void GameManager::updateScore(int delta) {
    m_score += delta;
}

// Decrease health, trigger game over if health drops to 0 or below
void GameManager::damage(int amount) {
    m_health -= amount;
    if (m_health <= 0) {
        m_health = 0;
        gameOver();
    }
}

// Per-frame logic can go here, currently unused
void GameManager::update() {
}

void GameManager::resetGame() {
    startGame();
}

void GameManager::gameClear() {
    m_gameState = GameClear;
    emit stateChanged(m_gameState);
}

void GameManager::startSpecificLevel(int level) {
    m_currentLevel = level;

    switch(level) {
        case 1: m_gameState = Level1; break;
        case 2: m_gameState = Level2; break;
        case 3: m_gameState = Level3; break;
        default: m_gameState = Level1; break;
    }

    m_score = 0;
    m_health = 1;
    emit stateChanged(m_gameState);
}

// Advance to the next level
void GameManager::nextLevel() {
    m_currentLevel++;

    if (m_currentLevel >3){
        m_currentLevel = 3;
    }

    switch(m_gameState) {
        case Level1: m_gameState = Level2; break;
        case Level2: m_gameState = Level3; break;
        default: break;  // Already at max level or not in a level state
    }

    // Keep score and health from previous level
    emit stateChanged(m_gameState);
}

// Get the current level number (1, 2, or 3)
int GameManager::currentLevel() {
    return m_currentLevel;
}
