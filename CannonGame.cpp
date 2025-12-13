#include "CannonGame.h"
#include <iostream>
#include <cstdint>

CannonGame::CannonGame() {
    // Le constructeur de base, l'initialisation se fait dans initialize()
}

void CannonGame::initialize(sf::RenderWindow& window) {
    // Initialiser la view (caméra)
    view = window.getDefaultView();
    
    // Réinitialiser le canon
    cannon = Cannon();
    
    // Vider les projectiles
    balls.clear();
    
    // Réinitialiser le shake
    shakeTimer = 0.0f;
    
    // Créer la grille de briques
    createBricks();
    
    // Initialiser le HUD Score
    if (!hudFont.openFromFile("arial.ttf")) {
        std::cerr << "Erreur : Impossible de charger arial.ttf pour le HUD" << std::endl;
    }
    hudScoreText = std::make_unique<sf::Text>(hudFont);
    hudScoreText->setCharacterSize(28);
    hudScoreText->setFillColor(sf::Color::Cyan);
    hudScoreText->setPosition(sf::Vector2f(15.f, 15.f));
    hudScoreText->setOutlineThickness(2.f);
    hudScoreText->setOutlineColor(sf::Color::Black);
    
    std::cout << "Mode Canon initialisé avec " << bricks.size() << " briques !" << std::endl;
}

void CannonGame::createBricks() {
    bricks.clear();
    
    // Créer une grille de briques dans la moitié supérieure de l'écran
    const float brickWidth = 75.f;
    const float brickHeight = 25.f;
    const float spacing = 5.f;
    const int rows = 8;
    const int cols = 10;
    
    // Calculer la position de départ pour centrer la grille
    float totalWidth = cols * brickWidth + (cols - 1) * spacing;
    float startX = (800.f - totalWidth) / 2.f;
    float startY = 50.f;
    
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            sf::Vector2f position(
                startX + col * (brickWidth + spacing),
                startY + row * (brickHeight + spacing)
            );
            
            // Varier les points de vie selon la rangée
            int hitPoints = 1 + (row / 2); // 1-4 points de vie
            bricks.emplace_back(position, hitPoints);
            
            int randomValue = rand() % 100;
            
            // 10% de chance qu'une brique soit TNT
            if (randomValue < 10) {
                bricks.back().isTNT = true;
                bricks.back().updateColor(); // Appliquer la couleur TNT
            }
            // 5% de chance qu'une brique soit SHOTGUN (et pas TNT)
            else if (randomValue < 15) {
                bricks.back().isBonusShotgun = true;
                bricks.back().updateColor(); // Appliquer la couleur cyan
            }
        }
    }
    
    std::cout << "Grille créée avec " << bricks.size() << " briques (10% TNT)" << std::endl;
}

void CannonGame::handleEvents(sf::RenderWindow& window, const sf::Event& event, GameData& gameData) {
    if (event.is<sf::Event::MouseMoved>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseMoved>();
        sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), 
                             static_cast<float>(mouseEvent->position.y));
        cannon.aimAt(mousePos);
    }
    
    if (event.is<sf::Event::MouseButtonPressed>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left && cannon.canShoot) {
            // Permettre jusqu'à 3 balles en même temps (Invasion de Balles !)
            if (balls.size() < 3 && shootCooldown.getElapsedTime().asSeconds() > SHOOT_DELAY) {
                // Créer une nouvelle balle
                sf::Vector2f startPos = cannon.getBarrelEnd();
                sf::Vector2f velocity(
                    PROJECTILE_SPEED * std::cos(cannon.angle),
                    PROJECTILE_SPEED * std::sin(cannon.angle)
                );
                
                balls.emplace_back(startPos, velocity);
                shootCooldown.restart();
                
                std::cout << "🔵 Balle " << balls.size() << " tirée à l'angle " << (cannon.angle * 180.f / M_PI) << "°" << std::endl;
            }
        }
    }
    
    if (event.is<sf::Event::KeyPressed>()) {
        auto keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (keyEvent->code == sf::Keyboard::Key::Escape) {
            // Retourner au menu
            window.close();
        }
    }
}

void CannonGame::update(float deltaTime, GameState& gameState, GameData& gameData, sf::RenderWindow& window) {
    // Gestion du Screen Shake
    if (shakeTimer > 0) {
        shakeTimer -= deltaTime;
        // Génère un décalage aléatoire entre -Intensité et +Intensité
        float offsetX = (rand() % 100 - 50) / 50.0f * shakeIntensity;
        float offsetY = (rand() % 100 - 50) / 50.0f * shakeIntensity;
        
        // Applique le décalage à la vue
        sf::View shakenView = window.getDefaultView();
        shakenView.move(sf::Vector2f(offsetX, offsetY));
        window.setView(shakenView);
    } else {
        // Remet la caméra normale quand c'est fini
        window.setView(window.getDefaultView());
    }
    
    // Gestion des particules d'explosion
    for (int i = 0; i < particles.size(); i++) {
        particles[i].shape.move(particles[i].velocity * deltaTime);
        particles[i].lifetime -= deltaTime;
        
        // Fade out : réduit la transparence au fil du temps
        sf::Color c = particles[i].shape.getFillColor();
        c.a = static_cast<std::uint8_t>(255 * particles[i].lifetime);
        particles[i].shape.setFillColor(c);
        
        // Supprime les particules mortes
        if (particles[i].lifetime <= 0) {
            particles.erase(particles.begin() + i);
            i--;
        }
    }
    
    updateBalls(deltaTime, gameState);
    checkCollisions(gameData, gameState);
    removeDestroyedObjects();
    
    // Mettre à jour le HUD Score en temps réel
    if (hudScoreText) {
        hudScoreText->setString("Score: " + std::to_string(gameData.score));
    }
    
    // Vérifier la victoire
    if (isVictorious()) {
        gameState = GameState::VICTORY;
    }
}

void CannonGame::updateBalls(float deltaTime, GameState& gameState) {
    for (int i = 0; i < balls.size(); i++) {
        Ball& ball = balls[i];
        if (!ball.active) continue;
        
        // Mise à jour physique - Mettre à jour la position
        sf::Vector2f newPos = ball.shape.getPosition();
        newPos += ball.velocity * deltaTime;
        ball.shape.setPosition(newPos);
        
        // Gestion des rebonds sur les murs latéraux
        if (newPos.x <= ball.shape.getRadius() || newPos.x >= 800.f - ball.shape.getRadius()) {
            ball.velocity.x = -ball.velocity.x;
            // Repositionner pour éviter les chevauchements
            if (newPos.x <= ball.shape.getRadius()) {
                newPos.x = ball.shape.getRadius();
            } else {
                newPos.x = 800.f - ball.shape.getRadius();
            }
            ball.shape.setPosition(newPos);
        }
        
        // Rebond sur le mur du haut
        if (newPos.y <= ball.shape.getRadius()) {
            ball.velocity.y = -ball.velocity.y;
            newPos.y = ball.shape.getRadius();
            ball.shape.setPosition(newPos);
        }
        
        // Décrémenter la durée de vie
        ball.lifetime -= deltaTime;
        
        // Nettoyage : Si la balle sort par le bas ou durée de vie expirée
        if (newPos.y >= 600.f || ball.lifetime <= 0.f) {
            std::cout << "🗑️  Balle " << (i + 1) << " supprimée (hors écran)" << std::endl;
            balls.erase(balls.begin() + i);
            i--; // Décrémente i car la liste a rétréci
        }
    }
}

void CannonGame::checkCollisions(GameData& gameData, GameState& gameState) {
    for (int ballIdx = 0; ballIdx < balls.size(); ballIdx++) {
        Ball& ball = balls[ballIdx];
        if (!ball.active) continue;
        
        // Vérifier les collisions avec les briques
        bool ballHit = false;
        for (int i = 0; i < bricks.size(); i++) {
            auto& brick = bricks[i];
            if (brick.destroyed) continue;
            
            if (brick.shape.getGlobalBounds().findIntersection(ball.shape.getGlobalBounds())) {
                // Collision détectée
                brick.takeDamage();
                gameData.score += 10;
                ballHit = true;
                
                // Créer une explosion à la position de la brique
                sf::Vector2f brickPos = brick.shape.getPosition() + brick.shape.getSize() / 2.f;
                createExplosion(brickPos.x, brickPos.y, brick.shape.getFillColor());
                
                // Si c'est une brique TNT, déclencher une chaîne d'explosions !
                if (brick.isTNT && brick.destroyed) {
                    // Screen Shake plus violent !
                    shakeTimer = 0.5f; // Plus long
                    shakeIntensity = 10.0f; // Plus intense
                    
                    std::cout << "🔥 EXPLOSION TNT !" << std::endl;
                    
                    // Chercher et détruire les briques adjacentes (3x3 autour)
                    const int cols = 10; // Nombre de colonnes de la grille
                    const int rows = 8;  // Nombre de lignes
                    const float brickWidth = 75.f + 5.f; // Largeur + espacement
                    const float brickHeight = 25.f + 5.f; // Hauteur + espacement
                    
                    sf::Vector2f brickGridPos = brick.shape.getPosition();
                    int brickCol = static_cast<int>((brickGridPos.x - 50.f) / brickWidth); // Position approximative
                    int brickRow = static_cast<int>((brickGridPos.y - 50.f) / brickHeight);
                    
                    // Détruire les voisins (3x3)
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            int neighborCol = brickCol + dx;
                            int neighborRow = brickRow + dy;
                            
                            // Vérifier les limites
                            if (neighborCol >= 0 && neighborCol < cols && neighborRow >= 0 && neighborRow < rows) {
                                // Chercher la brique adjacente
                                for (int j = 0; j < bricks.size(); j++) {
                                    if (i == j || bricks[j].destroyed) continue;
                                    
                                    // Vérifier si elle est proche (dans la zone 3x3)
                                    float dist = std::sqrt(
                                        std::pow(bricks[j].shape.getPosition().x - brickGridPos.x, 2) +
                                        std::pow(bricks[j].shape.getPosition().y - brickGridPos.y, 2)
                                    );
                                    
                                    if (dist < 120.f) { // Rayon d'explosion
                                        // Explosion en chaîne !
                                        bricks[j].destroyed = true;
                                        gameData.score += 20; // Bonus pour chaîne
                                        
                                        // Créer une explosion pour chaque voisin aussi
                                        sf::Vector2f neighborPos = bricks[j].shape.getPosition() + bricks[j].shape.getSize() / 2.f;
                                        createExplosion(neighborPos.x, neighborPos.y, bricks[j].shape.getFillColor());
                                        
                                        std::cout << "  → Brique voisine détruite en chaîne !" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
                // Si c'est une brique SHOTGUN BONUS !
                else if (brick.isBonusShotgun && brick.destroyed) {
                    // Screen Shake moyen
                    shakeTimer = 0.3f;
                    shakeIntensity = 8.0f;
                    
                    std::cout << "🔫 SHOTGUN BONUS DÉCLENCHÉ !" << std::endl;
                    
                    // Créer 5 balles en cône depuis la position de la brique
                    sf::Vector2f brickCenter = brick.shape.getPosition() + brick.shape.getSize() / 2.f;
                    
                    // Angle initial de la balle qui a touché
                    float baseAngle = std::atan2(ball.velocity.y, ball.velocity.x);
                    
                    for (int k = -2; k <= 2; k++) {
                        // Copier les propriétés de la balle qui a déclenché le bonus
                        Ball newBall = ball;
                        
                        // Créer un cône de balles
                        float spreadAngle = baseAngle + (k * 10.0f * M_PI / 180.0f); // Spread de 10° entre chaque balle
                        float speed = PROJECTILE_SPEED * 0.9f;
                        
                        // Appliquer la nouvelle direction
                        newBall.velocity.x = speed * std::cos(spreadAngle);
                        newBall.velocity.y = speed * std::sin(spreadAngle);
                        
                        // Position de départ à la position de la brique
                        newBall.shape.setPosition(brickCenter);
                        newBall.lifetime = 10.f; // Durée de vie normale
                        newBall.active = true;
                        
                        balls.push_back(newBall);
                    }
                    
                    gameData.score += 100; // Bonus massif pour le SHOTGUN !
                    std::cout << "💥 +100 BONUS SHOTGUN !" << std::endl;
                } else {
                    // Shake normal pour les briques non-TNT/non-SHOTGUN
                    shakeTimer = 0.2f;
                    shakeIntensity = 5.0f;
                }
                
                // Effet de rebond de la balle
                sf::Vector2f brickCenter = brick.shape.getPosition() + brick.shape.getSize() / 2.f;
                sf::Vector2f ballCenter = ball.shape.getPosition();
                sf::Vector2f direction = ballCenter - brickCenter;
                
                // Normaliser la direction
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (length > 0) {
                    direction /= length;
                    ball.velocity = direction * PROJECTILE_SPEED * 0.8f; // Réduire un peu la vitesse
                }
                
                std::cout << "🎯 Brique touchée ! Score: " << gameData.score << std::endl;
                break; // Une balle ne peut toucher qu'une brique à la fois
            }
        }
    }
}

void CannonGame::removeDestroyedObjects() {
    // Supprimer les briques détruites
    bricks.erase(
        std::remove_if(bricks.begin(), bricks.end(),
            [](const CannonBrick& brick) { return brick.destroyed; }),
        bricks.end()
    );
    
    // Les balles inactives sont maintenant supprimées directement dans updateBalls()
    // pour éviter les problèmes d'indexation
}

bool CannonGame::isVictorious() const {
    // Victoire si toutes les briques sont détruites
    return bricks.empty();
}

void CannonGame::draw(sf::RenderWindow& window, const GameData& gameData) {
    // Fond
    window.clear(sf::Color(10, 10, 30));
    
    // Dessiner les briques
    for (const auto& brick : bricks) {
        if (!brick.destroyed) {
            window.draw(brick.shape);
            
            // Afficher les points de vie sur la brique
            // (Optionnel : on pourrait ajouter du texte ici)
        }
    }
    
    // Dessiner toutes les balles actives
    for (const auto& b : balls) {
        window.draw(b.shape);
        
        // Effet de traînée (trail) pour chaque balle
        sf::CircleShape trail = b.shape;
        trail.setRadius(2.f);
        trail.setFillColor(sf::Color(255, 255, 0, 100));
        trail.setOrigin(sf::Vector2f(2.f, 2.f));
        sf::Vector2f trailPos = b.shape.getPosition();
        trailPos -= b.velocity * 0.02f; // Position légèrement en arrière
        trail.setPosition(trailPos);
        window.draw(trail);
    }
    
    // Dessiner le canon
    window.draw(cannon.base);
    window.draw(cannon.barrel);
    
    // Dessiner une ligne de visée (optionnel)
    sf::Vector2f cannonPos = cannon.position;
    sf::Vector2f aimDirection(
        std::cos(cannon.angle) * 100.f,
        std::sin(cannon.angle) * 100.f
    );
    
    sf::Vertex aimLine[] = {
        sf::Vertex{cannonPos, sf::Color(255, 255, 255, 80)},
        sf::Vertex{cannonPos + aimDirection, sf::Color(255, 255, 255, 40)}
    };
    window.draw(aimLine, 2, sf::PrimitiveType::Lines);
    
    // Dessiner les particules d'explosion
    for (auto& p : particles) {
        window.draw(p.shape);
    }
    
    // Dessiner le HUD Score en haut à gauche
    if (hudScoreText) {
        window.draw(*hudScoreText);
    }
}

void CannonGame::createExplosion(float x, float y, sf::Color color) {
    for (int i = 0; i < 15; i++) { // 15 petits morceaux
        CannonParticle p(
            sf::Vector2f(x, y),
            sf::Vector2f(0.f, 0.f),
            1.0f, // Durée de vie
            color
        );
        
        p.shape.setSize(sf::Vector2f(5.f, 5.f)); // Taille des morceaux
        
        // Vitesse aléatoire dans toutes les directions
        float angle = (rand() % 360) * M_PI / 180.0f;
        float speed = (rand() % 200 + 100);
        p.velocity = sf::Vector2f(
            speed * std::cos(angle),
            speed * std::sin(angle)
        );
        
        particles.push_back(p);
    }
}