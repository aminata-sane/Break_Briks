#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include "GameData.h"
#include "Game.h"
#include "Menu.h"
#include "EndScreen.h"

int main() {
    // Configuration de la fenêtre
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Break the Bricks!");
    
    // État du jeu et données
    GameState gameState = GameState::MENU;
    GameData game;
    
    // Initialiser le jeu
    initializeGame(game);
    
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
            else {
                // Gérer les événements selon l'état du jeu
                switch (gameState) {
                    case GameState::MENU:
                        handleMenuEvents(window, *event, gameState);
                        break;
                    case GameState::PLAYING:
                        handleGameEvents(window, *event, game);
                        break;
                    case GameState::VICTORY:
                    case GameState::DEFEAT:
                        handleEndScreenEvents(window, *event, gameState, game, gameState);
                        break;
                    case GameState::GAME_OVER:
                        // Ancienne compatibilité (peut être supprimée)
                        handleGameOverEvents(window, *event, gameState, game);
                        break;
                }
            }
        }
        
        // Mettre à jour selon l'état du jeu
        switch (gameState) {
            case GameState::MENU:
                updateMenu(deltaTime);
                break;
            case GameState::PLAYING:
                updateGame(game, deltaTime, window, gameState);
                break;
            case GameState::VICTORY:
            case GameState::DEFEAT:
                updateEndScreen(deltaTime);
                break;
            case GameState::GAME_OVER:
                // Ancienne compatibilité
                break;
        }
        
        // Dessiner selon l'état du jeu
        switch (gameState) {
            case GameState::MENU:
                drawMenu(window);
                break;
            case GameState::PLAYING:
                drawGame(window, game);
                break;
            case GameState::VICTORY:
                drawVictoryScreen(window, game);
                break;
            case GameState::DEFEAT:
                drawDefeatScreen(window, game);
                break;
            case GameState::GAME_OVER:
                // Ancienne compatibilité
                drawGameOver(window, game);
                break;
        }
    }
    return 0;
}
