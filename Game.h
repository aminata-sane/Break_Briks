#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "GameData.h"
#include "GameStates.h"

// Fonctions pour la gestion du jeu principal
void initializeGame(GameData& game, sf::RenderWindow& window);
void handleGameEvents(sf::RenderWindow& window, const sf::Event& event, GameData& game);
void updateGame(GameData& game, float deltaTime, GameState& gameState);
void drawGame(sf::RenderWindow& window, const GameData& game);

#endif // GAME_H