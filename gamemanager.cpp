#include "gameManager.h"

GameManager::GameManager(QObject *parent)
    : QObject(parent),
    m_gameState(MainMenu),
    m_score(0),
    m_health(10)
{
}

// Start or restart the game: reset score and health, switch to Playing state
void GameManager::startGame() {
    m_gameState = Playing;
    m_score = 0;
    m_health = 10;
    emit stateChanged(m_gameState);
}

// End the game and change state to GameOver
void GameManager::gameOver() {
    m_gameState = GameOver;
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
