#include "GameData.h"

// Définition des constantes statiques
const float GameData::paddleSpeed = 300.0f;
const float GameData::BRICK_WIDTH = 75.0f;
const float GameData::BRICK_HEIGHT = 25.0f;
const float GameData::BRICK_SPACING = 5.0f;

// Constructeur
GameData::GameData() : score(0), lives(3), leftPressed(false), rightPressed(false) {
    ballVelocity = sf::Vector2f(200.f, -200.f);
}