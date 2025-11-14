#include "Game.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// Fonction pour initialiser le jeu
void initializeGame(GameData& game) {
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
}

// Fonction pour gérer les événements du jeu
void handleGameEvents(sf::RenderWindow& window, const sf::Event& event, GameData& game) {
    if (event.is<sf::Event::KeyPressed>()) {
        if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::A ||
            event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Left) {
            game.leftPressed = true;
        }
        if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::D ||
            event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Right) {
            game.rightPressed = true;
        }
    }
    else if (event.is<sf::Event::KeyReleased>()) {
        if (event.getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::A ||
            event.getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::Left) {
            game.leftPressed = false;
        }
        if (event.getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::D ||
            event.getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::Right) {
            game.rightPressed = false;
        }
    }
}

// Fonction pour mettre à jour la logique du jeu
void updateGame(GameData& game, float deltaTime, sf::RenderWindow& window, GameState& gameState) {
    // Contrôle de la raquette
    sf::Vector2f paddlePos = game.paddle.getPosition();
    if (game.leftPressed && paddlePos.x > 0) {
        game.paddle.move(sf::Vector2f(-game.paddleSpeed * deltaTime, 0));
    }
    if (game.rightPressed && paddlePos.x < game.WINDOW_WIDTH - 100) {
        game.paddle.move(sf::Vector2f(game.paddleSpeed * deltaTime, 0));
    }
    
    // Mouvement de la balle
    game.ball.move(sf::Vector2f(game.ballVelocity.x * deltaTime, game.ballVelocity.y * deltaTime));
    
    // Rebonds de la balle sur les bords
    sf::Vector2f ballPos = game.ball.getPosition();
    
    if (ballPos.x <= 0 || ballPos.x >= game.WINDOW_WIDTH - 20) {
        game.ballVelocity.x = -game.ballVelocity.x;
    }
    
    if (ballPos.y <= 0) {
        game.ballVelocity.y = -game.ballVelocity.y;
    }
    
    // Collision balle-raquette
    sf::FloatRect ballBounds = game.ball.getGlobalBounds();
    sf::FloatRect paddleBounds = game.paddle.getGlobalBounds();
    
    if (ballBounds.findIntersection(paddleBounds) && game.ballVelocity.y > 0) {
        float paddleCenter = paddleBounds.position.x + paddleBounds.size.x / 2;
        float ballCenter = ballBounds.position.x + ballBounds.size.x / 2;
        float hitPosition = (ballCenter - paddleCenter) / (paddleBounds.size.x / 2);
        
        hitPosition = std::max(-1.0f, std::min(1.0f, hitPosition));
        
        float speed = std::sqrt(game.ballVelocity.x * game.ballVelocity.x + game.ballVelocity.y * game.ballVelocity.y);
        game.ballVelocity.x = hitPosition * speed * 0.7f;
        game.ballVelocity.y = -std::abs(game.ballVelocity.y);
    }
    
    // Collision balle-briques
    for (auto it = game.bricks.begin(); it != game.bricks.end(); ) {
        sf::FloatRect brickBounds = it->getGlobalBounds();
        
        if (ballBounds.findIntersection(brickBounds)) {
            float ballCenterX = ballBounds.position.x + ballBounds.size.x / 2;
            float ballCenterY = ballBounds.position.y + ballBounds.size.y / 2;
            float brickCenterX = brickBounds.position.x + brickBounds.size.x / 2;
            float brickCenterY = brickBounds.position.y + brickBounds.size.y / 2;
            
            float deltaX = ballCenterX - brickCenterX;
            float deltaY = ballCenterY - brickCenterY;
            
            if (std::abs(deltaX / brickBounds.size.x) > std::abs(deltaY / brickBounds.size.y)) {
                game.ballVelocity.x = -game.ballVelocity.x;
            } else {
                game.ballVelocity.y = -game.ballVelocity.y;
            }
            
            it = game.bricks.erase(it);
            game.score += 10;
            
            window.setTitle("Break the Bricks! - Score: " + std::to_string(game.score) + " - Vies: " + std::to_string(game.lives));
            
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
            gameState = GameState::DEFEAT;
        } else {
            // Repositionner la balle
            game.ball.setPosition(sf::Vector2f(game.WINDOW_WIDTH / 2 - 10, game.WINDOW_HEIGHT / 2));
            game.ballVelocity = sf::Vector2f(200.f, -200.f);
            window.setTitle("Break the Bricks! - Score: " + std::to_string(game.score) + " - Vies: " + std::to_string(game.lives));
        }
    }
}

// Fonction pour dessiner le jeu
void drawGame(sf::RenderWindow& window, const GameData& game) {
    window.clear(sf::Color::Blue);
    
    for (const auto& brick : game.bricks) {
        window.draw(brick);
    }
    
    window.draw(game.paddle);
    window.draw(game.ball);
    
    window.display();
}

// Fonction pour gérer les événements du game over
void handleGameOverEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game) {
    if (event.is<sf::Event::KeyPressed>()) {
        if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::R) {
            initializeGame(game);
            gameState = GameState::PLAYING;
        }
        else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::M) {
            gameState = GameState::MENU;
        }
        else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
            window.close();
        }
    }
}

// Fonction pour dessiner l'écran de game over
void drawGameOver(sf::RenderWindow& window, const GameData& game) {
    window.clear(sf::Color::Black);
    
    // Pour l'instant, un écran simple
    // TODO: Améliorer l'affichage du game over avec du texte
    
    window.display();
}