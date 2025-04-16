#include "gameManager.h"

GameManager::GameManager(QObject *parent)
    : QObject(parent),
    m_gameState(MainMenu),
    m_score(0),
    m_health(10)
{
    m_collisionCooldown.setSingleShot(true);
    m_collisionCooldown.setInterval(1000); // 1 second cooldown
    connect(&m_collisionCooldown, &QTimer::timeout, [this](){
        m_isInCooldown = false;
    });
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
    if(m_isInCooldown) return;

    m_health -= amount;
    m_isInCooldown = true;
    m_collisionCooldown.start();

    if(m_health <= 0) {
        gameOver();
    }
}

// Per-frame logic can go here, currently unused
void GameManager::update() {
}

void GameManager::resetGame() {
    startGame();
}
