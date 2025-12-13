#ifndef CANNONGAME_H
#define CANNONGAME_H

#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include "GameData.h"
#include <vector>
#include <cmath>

// Structure pour les projectiles
struct Projectile {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool active;
    
    Projectile(sf::Vector2f position, sf::Vector2f vel) 
        : velocity(vel), active(true) {
        shape.setRadius(4.f);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(sf::Vector2f(4.f, 4.f)); // Centre du cercle
    }
};

// Structure pour les briques avec vie
struct CannonBrick {
    sf::RectangleShape shape;
    int hitPoints;
    int maxHitPoints;
    bool destroyed;
    
    CannonBrick(sf::Vector2f position, int hp = 3) 
        : hitPoints(hp), maxHitPoints(hp), destroyed(false) {
        shape.setSize(sf::Vector2f(75.f, 25.f));
        shape.setPosition(position);
        updateColor();
    }
    
    void takeDamage() {
        if (hitPoints > 0) {
            hitPoints--;
            updateColor();
            if (hitPoints <= 0) {
                destroyed = true;
            }
        }
    }
    
    void updateColor() {
        // Couleur selon les points de vie
        if (hitPoints >= maxHitPoints) {
            shape.setFillColor(sf::Color::Red);      // Pleine vie
        } else if (hitPoints >= maxHitPoints * 0.66f) {
            shape.setFillColor(sf::Color(255, 165, 0)); // Orange
        } else if (hitPoints >= maxHitPoints * 0.33f) {
            shape.setFillColor(sf::Color::Yellow);   // Jaune
        } else if (hitPoints > 0) {
            shape.setFillColor(sf::Color::Green);    // Presque détruite
        }
        
        // Contour noir pour la visibilité
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color::Black);
    }
};

// Structure pour le canon
struct Cannon {
    sf::RectangleShape base;     // Base du canon
    sf::RectangleShape barrel;   // Canon (tube)
    sf::Vector2f position;
    float angle;                 // Angle de rotation en radians
    bool canShoot;              // Peut tirer (pas de projectile actif)
    
    Cannon() : angle(0.f), canShoot(true) {
        // Base du canon
        base.setSize(sf::Vector2f(40.f, 20.f));
        base.setFillColor(sf::Color(100, 100, 100));
        base.setOrigin(sf::Vector2f(20.f, 10.f));
        
        // Tube du canon
        barrel.setSize(sf::Vector2f(50.f, 8.f));
        barrel.setFillColor(sf::Color(60, 60, 60));
        barrel.setOrigin(sf::Vector2f(0.f, 4.f)); // Pivot au début du tube
        
        // Position initiale en bas de l'écran
        position = sf::Vector2f(400.f, 550.f);
        updatePosition();
    }
    
    void updatePosition() {
        base.setPosition(position);
        barrel.setPosition(position);
        barrel.setRotation(sf::degrees(angle * 180.f / M_PI)); // Conversion radians -> degrés
    }
    
    void aimAt(sf::Vector2f mousePos) {
        // Calculer l'angle vers la position de la souris
        sf::Vector2f direction = mousePos - position;
        angle = std::atan2(direction.y, direction.x);
        
        // Limiter l'angle vers le haut seulement (de -170° à -10°)
        float minAngle = -170.f * M_PI / 180.f;
        float maxAngle = -10.f * M_PI / 180.f;
        if (angle > 0) {
            angle = (angle > M_PI/2) ? minAngle : maxAngle;
        }
        angle = std::max(minAngle, std::min(maxAngle, angle));
        
        updatePosition();
    }
    
    sf::Vector2f getBarrelEnd() const {
        // Position de l'extrémité du canon pour spawner les projectiles
        float barrelLength = 50.f;
        return sf::Vector2f(
            position.x + barrelLength * std::cos(angle),
            position.y + barrelLength * std::sin(angle)
        );
    }
};

// Classe principale pour le mode canon
class CannonGame {
public:
    CannonGame();
    
    void initialize(sf::RenderWindow& window);
    void handleEvents(sf::RenderWindow& window, const sf::Event& event, GameData& gameData);
    void update(float deltaTime, GameState& gameState, GameData& gameData, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window, const GameData& gameData);
    
private:
    Cannon cannon;
    std::vector<Projectile> projectiles;
    std::vector<CannonBrick> bricks;
    
    sf::Clock shootCooldown;
    const float PROJECTILE_SPEED = 400.f;
    const float SHOOT_DELAY = 0.1f; // Délai entre les tirs en secondes
    
    // Variables pour le Screen Shake (tremblement d'écran)
    sf::View view;
    float shakeTimer = 0.0f;
    const float shakeIntensity = 5.0f;
    
    void createBricks();
    void updateProjectiles(float deltaTime, GameState& gameState);
    void checkCollisions(GameData& gameData, GameState& gameState);
    void removeDestroyedObjects();
    bool isVictorious() const;
};

#endif // CANNONGAME_H