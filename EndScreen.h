#ifndef ENDSCREEN_H
#define ENDSCREEN_H

#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include "GameData.h"
#include <memory>

// Gestionnaire des écrans de fin de partie
class EndScreenManager {
private:
    sf::Clock animationClock;
    int selectedOption;
    sf::RectangleShape playAgainButton;
    sf::RectangleShape menuButton;
    sf::RectangleShape quitButton;
    
    // Police et textes
    sf::Font font;
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> scoreText;
    std::unique_ptr<sf::Text> playAgainText;
    std::unique_ptr<sf::Text> menuText;
    std::unique_ptr<sf::Text> quitText;
    std::unique_ptr<sf::Text> instructionText;
    bool fontLoaded;
    
public:
    EndScreenManager();
    // Paramètre 'currentState' supprimé, il est inutile
    void handleEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game);
    void update(float deltaTime);
    void drawVictory(sf::RenderWindow& window, const GameData& game);
    void drawDefeat(sf::RenderWindow& window, const GameData& game);
    
private:
    void drawCommon(sf::RenderWindow& window, const GameData& game, const std::string& title, sf::Color color);
    void drawButtons(sf::RenderWindow& window);
    sf::Color getGradientColor(float t); // Copié depuis MenuManager
};

// **Suppression des fonctions de compatibilité et de l'instance globale**

#endif // ENDSCREEN_H