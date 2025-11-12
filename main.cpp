#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>

int main() {
    // Configuration de la fenêtre
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Break the Bricks!");
    
    // Création de la raquette
    sf::RectangleShape paddle;
    paddle.setSize(sf::Vector2f(100.f, 20.f)); // Largeur: 100px, Hauteur: 20px
    paddle.setFillColor(sf::Color::White);
    paddle.setPosition(sf::Vector2f(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 50)); // Centrée en bas
    
    // Création de la balle
    sf::CircleShape ball(10.f); // Rayon de 10 pixels
    ball.setFillColor(sf::Color::Yellow);
    ball.setPosition(sf::Vector2f(WINDOW_WIDTH / 2 - 10, WINDOW_HEIGHT / 2)); // Centre de l'écran
    
    // Vitesses
    float paddleSpeed = 300.f; // pixels par seconde
    sf::Vector2f ballVelocity(200.f, -200.f); // Vitesse initiale de la balle (x, y)
    
    // Création des briques
    const int BRICK_ROWS = 5;
    const int BRICK_COLS = 10;
    const float BRICK_WIDTH = 75.f;
    const float BRICK_HEIGHT = 25.f;
    const float BRICK_SPACING = 5.f;
    
    std::vector<sf::RectangleShape> bricks;
    std::vector<sf::Color> brickColors = {
        sf::Color::Red, sf::Color::Green, sf::Color::Blue, 
        sf::Color::Yellow, sf::Color::Magenta
    };
    
    // Génération de la grille de briques
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            sf::RectangleShape brick;
            brick.setSize(sf::Vector2f(BRICK_WIDTH, BRICK_HEIGHT));
            brick.setFillColor(brickColors[row]); // Couleur selon la rangée
            
            float x = col * (BRICK_WIDTH + BRICK_SPACING) + BRICK_SPACING;
            float y = row * (BRICK_HEIGHT + BRICK_SPACING) + 50; // 50px du haut
            brick.setPosition(sf::Vector2f(x, y));
            
            bricks.push_back(brick);
        }
    }
    
    // Système de score
    int score = 0;
    
    // Variables de contrôle
    bool leftPressed = false;
    bool rightPressed = false;
    
    // Horloge pour gérer le temps
    sf::Clock clock;
    
    while (window.isOpen()) {
        // Calcul du temps écoulé
        float deltaTime = clock.restart().asSeconds();
        
        // Gestion des événements
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (event->is<sf::Event::KeyPressed>()) {
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::A ||
                    event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Left) {
                    leftPressed = true;
                }
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::D ||
                    event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Right) {
                    rightPressed = true;
                }
            }
            else if (event->is<sf::Event::KeyReleased>()) {
                if (event->getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::A ||
                    event->getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::Left) {
                    leftPressed = false;
                }
                if (event->getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::D ||
                    event->getIf<sf::Event::KeyReleased>()->code == sf::Keyboard::Key::Right) {
                    rightPressed = false;
                }
            }
        }
        
        // Contrôle de la raquette
        sf::Vector2f paddlePos = paddle.getPosition();
        if (leftPressed && paddlePos.x > 0) {
            paddle.move(sf::Vector2f(-paddleSpeed * deltaTime, 0));
        }
        if (rightPressed && paddlePos.x < WINDOW_WIDTH - 100) {
            paddle.move(sf::Vector2f(paddleSpeed * deltaTime, 0));
        }
        
        // Mouvement de la balle
        ball.move(sf::Vector2f(ballVelocity.x * deltaTime, ballVelocity.y * deltaTime));
        
        // Rebonds de la balle sur les bords
        sf::Vector2f ballPos = ball.getPosition();
        
        // Rebond sur les murs gauche et droite
        if (ballPos.x <= 0 || ballPos.x >= WINDOW_WIDTH - 20) {
            ballVelocity.x = -ballVelocity.x;
        }
        
        // Rebond sur le mur du haut
        if (ballPos.y <= 0) {
            ballVelocity.y = -ballVelocity.y;
        }
        
        // Collision balle-raquette
        sf::FloatRect ballBounds = ball.getGlobalBounds();
        sf::FloatRect paddleBounds = paddle.getGlobalBounds();
        
        if (ballBounds.findIntersection(paddleBounds) && ballVelocity.y > 0) {
            // Calcul de l'angle de rebond basé sur où la balle touche la raquette
            float paddleCenter = paddleBounds.position.x + paddleBounds.size.x / 2;
            float ballCenter = ballBounds.position.x + ballBounds.size.x / 2;
            float hitPosition = (ballCenter - paddleCenter) / (paddleBounds.size.x / 2);
            
            // Limiter la position de frappe entre -1 et 1
            hitPosition = std::max(-1.0f, std::min(1.0f, hitPosition));
            
            // Nouvelle direction de la balle
            float speed = std::sqrt(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
            ballVelocity.x = hitPosition * speed * 0.7f; // Composante horizontale
            ballVelocity.y = -std::abs(ballVelocity.y);   // Toujours vers le haut
        }
        
        // Collision balle-briques
        for (auto it = bricks.begin(); it != bricks.end(); ) {
            sf::FloatRect brickBounds = it->getGlobalBounds();
            
            if (ballBounds.findIntersection(brickBounds)) {
                // Déterminer le côté de collision
                float ballCenterX = ballBounds.position.x + ballBounds.size.x / 2;
                float ballCenterY = ballBounds.position.y + ballBounds.size.y / 2;
                float brickCenterX = brickBounds.position.x + brickBounds.size.x / 2;
                float brickCenterY = brickBounds.position.y + brickBounds.size.y / 2;
                
                float deltaX = ballCenterX - brickCenterX;
                float deltaY = ballCenterY - brickCenterY;
                
                // Rebond selon le côté touché
                if (std::abs(deltaX / brickBounds.size.x) > std::abs(deltaY / brickBounds.size.y)) {
                    ballVelocity.x = -ballVelocity.x; // Collision sur les côtés
                } else {
                    ballVelocity.y = -ballVelocity.y; // Collision haut/bas
                }
                
                // Supprimer la brique et augmenter le score
                it = bricks.erase(it);
                score += 10; // 10 points par brique
                
                // Mettre à jour le titre de la fenêtre avec le score
                window.setTitle("Break the Bricks! - Score: " + std::to_string(score));
                
                break; // Une seule collision à la fois
            } else {
                ++it;
            }
        }
        
        // Vérifier si toutes les briques sont détruites (victoire)
        if (bricks.empty()) {
            window.setTitle("Break the Bricks! - VICTOIRE! Score final: " + std::to_string(score));
            // Arrêter la balle pour marquer la fin du jeu
            ballVelocity = sf::Vector2f(0, 0);
        }
        
        // Si la balle sort par le bas, on la remet au centre (plus tard: perte de vie)
        if (ballPos.y >= WINDOW_HEIGHT) {
            ball.setPosition(sf::Vector2f(WINDOW_WIDTH / 2 - 10, WINDOW_HEIGHT / 2));
            ballVelocity = sf::Vector2f(200.f, -200.f);
        }

        // Rendu
        window.clear(sf::Color::Blue);
        
        // Dessiner toutes les briques
        for (const auto& brick : bricks) {
            window.draw(brick);
        }
        
        window.draw(paddle);
        window.draw(ball);
        window.display();
    }
    return 0;
}
