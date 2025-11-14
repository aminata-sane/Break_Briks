#include "Game.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string> // Pour std::to_string

// Fonction pour initialiser le jeu
void initializeGame(GameData& game, sf::RenderWindow& window) {
    // Création de la raquette
    game.paddle.setSize(sf::Vector2f(100.f, 20.f));
    game.paddle.setFillColor(sf::Color::White);
    game.paddle.setPosition(sf::Vector2f(game.WINDOW_WIDTH / 2 - 50, game.WINDOW_HEIGHT - 50));
    
    // Création de la balle
    game.ball.setRadius(10.f);
    game.ball.setFillColor(sf::Color::Yellow);
    game.ball.setPosition(sf::Vector2f(game.WINDOW_WIDTH / 2 - 10, game.WINDOW_HEIGHT / 2));
    
    // Vitesse initiale de la balle
    game.ballVelocity = sf::Vector2f(200.f, -200.f);
    
    // Réinitialiser les variables
    game.score = 0;
    game.lives = 3;
    game.leftPressed = false;
    game.rightPressed = false;
    
    // Création des briques
    game.bricks.clear();
    std::vector<sf::Color> brickColors = {
        sf::Color::Red, sf::Color::Green, sf::Color::Blue, 
        sf::Color::Yellow, sf::Color::Magenta
    };
    
    for (int row = 0; row < game.BRICK_ROWS; row++) {
        for (int col = 0; col < game.BRICK_COLS; col++) {
            sf::RectangleShape brick;
            brick.setSize(sf::Vector2f(game.BRICK_WIDTH, game.BRICK_HEIGHT));
            brick.setFillColor(brickColors[row]);
            
            float x = col * (game.BRICK_WIDTH + game.BRICK_SPACING) + game.BRICK_SPACING;
            float y = row * (game.BRICK_HEIGHT + game.BRICK_SPACING) + 50;
            brick.setPosition(sf::Vector2f(x, y));
            
            game.bricks.push_back(brick);
        }
    }

    // Mettre à jour le titre de la fenêtre
    window.setTitle("Break-Briks! - Score: " + std::to_string(game.score) + " - Vies: " + std::to_string(game.lives));
}

// Fonction pour gérer les événements du jeu
void handleGameEvents(sf::RenderWindow& window, const sf::Event& event, GameData& game) {
    if (event.is<sf::Event::KeyPressed>()) {
        auto keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (keyEvent->code == sf::Keyboard::Key::A || keyEvent->code == sf::Keyboard::Key::Left) {
            game.leftPressed = true;
        }
        if (keyEvent->code == sf::Keyboard::Key::D || keyEvent->code == sf::Keyboard::Key::Right) {
            game.rightPressed = true;
        }
    }
    else if (event.is<sf::Event::KeyReleased>()) {
        auto keyEvent = event.getIf<sf::Event::KeyReleased>();
        if (keyEvent->code == sf::Keyboard::Key::A || keyEvent->code == sf::Keyboard::Key::Left) {
            game.leftPressed = false;
        }
        if (keyEvent->code == sf::Keyboard::Key::D || keyEvent->code == sf::Keyboard::Key::Right) {
            game.rightPressed = false;
        }
    }
}

// Fonction pour mettre à jour la logique du jeu
void updateGame(GameData& game, float deltaTime, GameState& gameState) {
    // Contrôle de la raquette
    sf::Vector2f paddlePos = game.paddle.getPosition();
    if (game.leftPressed && paddlePos.x > 0) {
        game.paddle.move(sf::Vector2f(-game.paddleSpeed * deltaTime, 0));
    }
    // Correction: utiliser getSize() au lieu de 100
    if (game.rightPressed && paddlePos.x < game.WINDOW_WIDTH - game.paddle.getSize().x) {
        game.paddle.move(sf::Vector2f(game.paddleSpeed * deltaTime, 0));
    }
    
    // Mouvement de la balle
    game.ball.move(game.ballVelocity * deltaTime);
    
    // Rebonds de la balle sur les bords
    sf::Vector2f ballPos = game.ball.getPosition();
    float ballRadius = game.ball.getRadius();
    
    // Correction: utiliser (radius * 2) au lieu de 20
    if (ballPos.x <= 0 || ballPos.x >= game.WINDOW_WIDTH - (ballRadius * 2)) {
        game.ballVelocity.x = -game.ballVelocity.x;
    }
    
    if (ballPos.y <= 0) {
        game.ballVelocity.y = -game.ballVelocity.y;
    }
    
    // Collision balle-raquette
    sf::FloatRect ballBounds = game.ball.getGlobalBounds();
    sf::FloatRect paddleBounds = game.paddle.getGlobalBounds();
    
    // Vérifier intersection
    if (ballBounds.findIntersection(paddleBounds) && game.ballVelocity.y > 0) {
        float paddleCenter = paddleBounds.position.x + paddleBounds.size.x / 2;
        float ballCenter = ballBounds.position.x + ballBounds.size.x / 2;
        float hitPosition = (ballCenter - paddleCenter) / (paddleBounds.size.x / 2);
        
        hitPosition = std::max(-1.0f, std::min(1.0f, hitPosition));
        
        float speed = std::sqrt(game.ballVelocity.x * game.ballVelocity.x + game.ballVelocity.y * game.ballVelocity.y);
        game.ballVelocity.x = hitPosition * speed * 0.7f;
        game.ballVelocity.y = -std::abs(game.ballVelocity.y);
        
        // S'assurer que la balle est au-dessus de la raquette après le rebond
        game.ball.setPosition(sf::Vector2f(ballPos.x, paddleBounds.position.y - ballBounds.size.y - 1.f));
    }
    
    // Collision balle-briques
    for (auto it = game.bricks.begin(); it != game.bricks.end(); ) {
        sf::FloatRect brickBounds = it->getGlobalBounds();
        
        // Vérifier intersection
        if (ballBounds.findIntersection(brickBounds)) {
            // Logique de rebond simple (inverser Y)
            game.ballVelocity.y = -game.ballVelocity.y;
            
            it = game.bricks.erase(it);
            game.score += 10;
            // Pas de mise à jour du titre ici, c'est trop fréquent
            
            break; 
        } else {
            ++it;
        }
    }
    
    // Vérifier si toutes les briques sont détruites (victoire)
    if (game.bricks.empty()) {
        gameState = GameState::VICTORY;
    }
    
    // Si la balle sort par le bas
    if (ballPos.y >= game.WINDOW_HEIGHT) {
        game.lives--;
        if (game.lives <= 0) {
            gameState = GameState::GAME_OVER;
        } else {
            // Repositionner la balle
            game.ball.setPosition(sf::Vector2f(game.WINDOW_WIDTH / 2 - 10, game.WINDOW_HEIGHT / 2));
            game.ballVelocity = sf::Vector2f(200.f, -200.f);
            // On met à jour le titre de la fenêtre SEULEMENT ici (et à l'init)
            // (on a besoin de la window pour ça... on va le faire dans le main)
        }
    }
}

// Fonction pour dessiner le jeu
void drawGame(sf::RenderWindow& window, const GameData& game) {
    // **Correction**
    // 1. Dessiner le fond d'écran
    sf::RectangleShape background(sf::Vector2f(GameData::WINDOW_WIDTH, GameData::WINDOW_HEIGHT));
    background.setFillColor(sf::Color(0, 0, 50)); // Bleu nuit
    window.draw(background);
    
    // 2. Dessiner les éléments
    for (const auto& brick : game.bricks) {
        window.draw(brick);
    }
    
    window.draw(game.paddle);
    window.draw(game.ball);
    
    // 3. NE PAS appeler clear() ou display()
}