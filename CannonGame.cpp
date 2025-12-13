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
    projectiles.clear();
    
    // Réinitialiser le shake
    shakeTimer = 0.0f;
    
    // Créer la grille de briques
    createBricks();
    
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
        }
    }
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
            // Tirer un projectile si aucun n'est actif
            bool hasActiveProjectile = false;
            for (const auto& projectile : projectiles) {
                if (projectile.active) {
                    hasActiveProjectile = true;
                    break;
                }
            }
            
            if (!hasActiveProjectile && shootCooldown.getElapsedTime().asSeconds() > SHOOT_DELAY) {
                // Créer un nouveau projectile
                sf::Vector2f startPos = cannon.getBarrelEnd();
                sf::Vector2f velocity(
                    PROJECTILE_SPEED * std::cos(cannon.angle),
                    PROJECTILE_SPEED * std::sin(cannon.angle)
                );
                
                projectiles.emplace_back(startPos, velocity);
                shootCooldown.restart();
                
                std::cout << "Projectile tiré à l'angle " << (cannon.angle * 180.f / M_PI) << "°" << std::endl;
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
    
    updateProjectiles(deltaTime, gameState);
    checkCollisions(gameData, gameState);
    removeDestroyedObjects();
    
    // Vérifier la victoire
    if (isVictorious()) {
        gameState = GameState::VICTORY;
    }
}

void CannonGame::updateProjectiles(float deltaTime, GameState& gameState) {
    for (auto& projectile : projectiles) {
        if (!projectile.active) continue;
        
        // Mettre à jour la position
        sf::Vector2f newPos = projectile.shape.getPosition();
        newPos += projectile.velocity * deltaTime;
        projectile.shape.setPosition(newPos);
        
        // Vérifier les rebonds sur les murs
        if (newPos.x <= projectile.shape.getRadius() || newPos.x >= 800.f - projectile.shape.getRadius()) {
            projectile.velocity.x = -projectile.velocity.x;
            // Repositionner pour éviter les chevauchements
            if (newPos.x <= projectile.shape.getRadius()) {
                newPos.x = projectile.shape.getRadius();
            } else {
                newPos.x = 800.f - projectile.shape.getRadius();
            }
            projectile.shape.setPosition(newPos);
        }
        
        // Rebond sur le mur du haut
        if (newPos.y <= projectile.shape.getRadius()) {
            projectile.velocity.y = -projectile.velocity.y;
            newPos.y = projectile.shape.getRadius();
            projectile.shape.setPosition(newPos);
        }
        
        // Disparaît si sort par le bas
        if (newPos.y >= 600.f) {
            projectile.active = false;
            cannon.canShoot = true; // Permettre un nouveau tir
        }
    }
}

void CannonGame::checkCollisions(GameData& gameData, GameState& gameState) {
    for (auto& projectile : projectiles) {
        if (!projectile.active) continue;
        
        // Vérifier les collisions avec les briques
        for (auto& brick : bricks) {
            if (brick.destroyed) continue;
            
            if (brick.shape.getGlobalBounds().findIntersection(projectile.shape.getGlobalBounds())) {
                // Collision détectée
                brick.takeDamage();
                gameData.score += 10;
                
                // Déclencher le Screen Shake !
                shakeTimer = 0.2f; // Tremble pendant 0.2 secondes
                
                // Créer une explosion à la position de la brique
                sf::Vector2f brickPos = brick.shape.getPosition() + brick.shape.getSize() / 2.f;
                createExplosion(brickPos.x, brickPos.y, brick.shape.getFillColor());
                
                // Effet de rebond du projectile
                sf::Vector2f brickCenter = brick.shape.getPosition() + brick.shape.getSize() / 2.f;
                sf::Vector2f projectileCenter = projectile.shape.getPosition();
                sf::Vector2f direction = projectileCenter - brickCenter;
                
                // Normaliser la direction
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (length > 0) {
                    direction /= length;
                    projectile.velocity = direction * PROJECTILE_SPEED * 0.8f; // Réduire un peu la vitesse
                }
                
                std::cout << "Brique touchée ! Score: " << gameData.score << std::endl;
                break; // Un projectile ne peut toucher qu'une brique à la fois
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
    
    // Supprimer les projectiles inactifs
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& proj) { return !proj.active; }),
        projectiles.end()
    );
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
    
    // Dessiner les projectiles
    for (const auto& projectile : projectiles) {
        if (projectile.active) {
            window.draw(projectile.shape);
            
            // Effet de traînée (trail)
            sf::CircleShape trail = projectile.shape;
            trail.setRadius(2.f);
            trail.setFillColor(sf::Color(255, 255, 0, 100));
            trail.setOrigin(sf::Vector2f(2.f, 2.f));
            sf::Vector2f trailPos = projectile.shape.getPosition();
            trailPos -= projectile.velocity * 0.02f; // Position légèrement en arrière
            trail.setPosition(trailPos);
            window.draw(trail);
        }
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