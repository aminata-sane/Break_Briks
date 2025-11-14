#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "GameData.h"
#include "GameStates.h"

// Fonctions pour la gestion du jeu principal
void initializeGame(GameData& game);
void handleGameEvents(sf::RenderWindow& window, const sf::Event& event, GameData& game);
void updateGame(GameData& game, float deltaTime, sf::RenderWindow& window, GameState& gameState);
void drawGame(sf::RenderWindow& window, const GameData& game);

// Fonctions pour l'écran de game over
void handleGameOverEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game);
void drawGameOver(sf::RenderWindow& window, const GameData& game);

#endif // GAME_H