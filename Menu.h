#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include <vector>
#include <cmath>
#include <random> // Ajouté
#include <cstdint> // Ajouté
#include <memory>

// Structure pour les particules d'animation
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float life;
    float maxLife;
    
    Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float lifetime)
        : position(pos), velocity(vel), color(col), life(lifetime), maxLife(lifetime) {}
};

// Classe pour gérer le menu avec animations
class MenuManager {
private:
    std::vector<Particle> particles;
    sf::Clock animationClock;
    int selectedButton;
    
    // Éléments visuels
    sf::RectangleShape startButton;
    sf::RectangleShape quitButton;
    sf::RectangleShape background;
    
    // Police et textes
    sf::Font font;
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> jouerText;
    std::unique_ptr<sf::Text> quitterText;
    std::unique_ptr<sf::Text> instructionText;
    bool fontLoaded;
    
    // Animation du titre
    float titleScale;
    float titleRotation;

    // Générateur de nombres aléatoires
    std::mt19937 gen;
    
public:
    MenuManager();
    void handleEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
private:
    void createParticles();
    void updateParticles(float deltaTime);
    void drawParticles(sf::RenderWindow& window);
    void drawTitle(sf::RenderWindow& window);
    void drawButtons(sf::RenderWindow& window);
    sf::Color getGradientColor(float t);
};

// **Suppression des fonctions de compatibilité et de l'instance globale**

#endif // MENU_H