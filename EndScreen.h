#ifndef ENDSCREEN_H
#define ENDSCREEN_H

#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include "GameData.h"

// Gestionnaire des écrans de fin de partie
class EndScreenManager {
private:
    sf::Clock animationClock;
    int selectedOption;
    sf::RectangleShape playAgainButton;
    sf::RectangleShape menuButton;
    sf::RectangleShape quitButton;
    
    // Texte et police
    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text playAgainText;
    sf::Text menuText;
    sf::Text quitText;
    sf::Text instructionText;
    bool fontLoaded;
    
public:
    EndScreenManager();
    void handleEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game, GameState currentState);
    void update(float deltaTime);
    void drawVictory(sf::RenderWindow& window, const GameData& game);
    void drawDefeat(sf::RenderWindow& window, const GameData& game);
    
private:
    void drawTitle(sf::RenderWindow& window, const std::string& title, sf::Color color);
    void drawScore(sf::RenderWindow& window, int score);
    void drawButtons(sf::RenderWindow& window);
    sf::Color getGradientColor(float t);
};

// Instance globale
extern EndScreenManager endScreenManager;

// Fonctions de compatibilité
void handleEndScreenEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game, GameState currentState);
void updateEndScreen(float deltaTime);
void drawVictoryScreen(sf::RenderWindow& window, const GameData& game);
void drawDefeatScreen(sf::RenderWindow& window, const GameData& game);

#endif // ENDSCREEN_H