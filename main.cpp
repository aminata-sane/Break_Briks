#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include "Menu.h"
#include "Game.h"
#include "CannonGame.h"
#include "EndScreen.h"
#include "GameData.h"

int main() {
    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode({GameData::WINDOW_WIDTH, GameData::WINDOW_HEIGHT}), "Break-Briks!");
    window.setFramerateLimit(60);

    // Initialisation des gestionnaires (en local, pas en global)
    MenuManager menuManager;
    EndScreenManager endScreenManager;
    CannonGame cannonGame;
    GameData gameData; // Contient toutes les données du jeu

    // Initialisation de l'état du jeu
    GameState currentState = GameState::MENU;
    sf::Clock clock;
    
    // Initialiser le jeu une première fois pour le titre
    initializeGame(gameData, window);
    int lastLives = gameData.lives;
    int lastScore = gameData.score;

    // Boucle de jeu principale
    while (window.isOpen()) {
        // Gérer le temps
        float deltaTime = clock.restart().asSeconds();

        // 1. GESTION DES ÉVÉNEMENTS
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Transmettre les événements au bon gestionnaire
            switch (currentState) {
                case GameState::MENU:
                    menuManager.handleEvents(window, *event, currentState, gameData.currentMode);
                    break;
                case GameState::PLAYING:
                    if (gameData.currentMode == GameMode::PADDLE) {
                        handleGameEvents(window, *event, gameData);
                    } else if (gameData.currentMode == GameMode::CANNON) {
                        cannonGame.handleEvents(window, *event, gameData);
                    }
                    break;
                case GameState::VICTORY:
                case GameState::GAME_OVER:
                    endScreenManager.handleEvents(window, *event, currentState, gameData);
                    break;
            }
        }

        // 2. MISE À JOUR DE LA LOGIQUE
        switch (currentState) {
            case GameState::MENU:
                menuManager.update(deltaTime);
                break;
            case GameState::PLAYING:
                // Initialiser le mode approprié si on vient de quitter le menu
                static GameMode lastMode = GameMode::PADDLE;
                if (lastMode != gameData.currentMode) {
                    if (gameData.currentMode == GameMode::PADDLE) {
                        initializeGame(gameData, window);
                    } else if (gameData.currentMode == GameMode::CANNON) {
                        cannonGame.initialize(window);
                        gameData.score = 0; // Reset du score
                    }
                    lastMode = gameData.currentMode;
                }
                
                // Mettre à jour selon le mode
                if (gameData.currentMode == GameMode::PADDLE) {
                    updateGame(gameData, deltaTime, currentState);
                } else if (gameData.currentMode == GameMode::CANNON) {
                    cannonGame.update(deltaTime, currentState, gameData, window);
                }
                
                // Mettre à jour le titre de la fenêtre si le score ou les vies changent
                if (gameData.lives != lastLives || gameData.score != lastScore) {
                    std::string modeStr = (gameData.currentMode == GameMode::PADDLE) ? "RAQUETTE" : "CANON";
                    window.setTitle("Break-Briks [" + modeStr + "] - Score: " + std::to_string(gameData.score) + " - Vies: " + std::to_string(gameData.lives));
                    lastLives = gameData.lives;
                    lastScore = gameData.score;
                }

                // Si le jeu vient de passer à la victoire/défaite, 
                // on réinitialise le jeu en arrière-plan
                if (currentState == GameState::VICTORY || currentState == GameState::GAME_OVER) {
                    // On garde le score final pour l'afficher
                    int finalScore = gameData.score;
                    if (gameData.currentMode == GameMode::PADDLE) {
                        initializeGame(gameData, window);
                    } else {
                        cannonGame.initialize(window);
                    }
                    gameData.score = finalScore; // Rétablir le score pour l'écran de fin
                }
                break;
            case GameState::VICTORY:
            case GameState::GAME_OVER:
                endScreenManager.update(deltaTime);
                break;
        }

        // 3. AFFICHAGE
        window.clear(sf::Color::Black); // Nettoyer UNE SEULE FOIS

        switch (currentState) {
            case GameState::MENU:
                menuManager.draw(window);
                break;
            case GameState::PLAYING:
                if (gameData.currentMode == GameMode::PADDLE) {
                    drawGame(window, gameData);
                } else if (gameData.currentMode == GameMode::CANNON) {
                    cannonGame.draw(window, gameData);
                }
                break;
            case GameState::VICTORY:
                endScreenManager.drawVictory(window, gameData);
                break;
            case GameState::GAME_OVER:
                endScreenManager.drawDefeat(window, gameData);
                break;
        }

        window.display(); // Afficher UNE SEULE FOIS
    }

    return 0;
}