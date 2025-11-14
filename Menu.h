#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include "GameData.h"
#include <vector>
#include <cmath>

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
    
    // Textes
    sf::Font font;
    sf::Text titleText;
    sf::Text raquetteText;
    sf::Text canonText;
    bool fontLoaded;
    
    // Animation du titre
    float titleScale;
    float titleRotation;
    
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

// Fonctions pour la compatibilité
void handleMenuEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState);
void updateMenu(float deltaTime);
void drawMenu(sf::RenderWindow& window);

// Instance globale du gestionnaire de menu
extern MenuManager menuManager;

#endif // MENU_H