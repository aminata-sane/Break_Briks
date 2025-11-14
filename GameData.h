#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <SFML/Graphics.hpp>
#include <vector>

// Enum pour les différents modes de jeu
enum class GameMode {
    PADDLE,  // Mode raquette classique
    CANNON   // Mode canon avec tir de projectiles
};

// Structure pour gérer les données du jeu
struct GameData {
    // Mode de jeu sélectionné
    GameMode currentMode;
    
    // Éléments du jeu
    sf::RectangleShape paddle;
    sf::CircleShape ball;
    std::vector<sf::RectangleShape> bricks;
    
    // Variables de jeu
    sf::Vector2f ballVelocity;
    int score;
    int lives;
    bool leftPressed;
    bool rightPressed;
    
    // Configuration
    static const float paddleSpeed;
    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;
    static const int BRICK_ROWS = 5;
    static const int BRICK_COLS = 10;
    static const float BRICK_WIDTH;
    static const float BRICK_HEIGHT;
    static const float BRICK_SPACING;
    
    // Constructeur
    GameData();
};

#endif // GAMEDATA_H