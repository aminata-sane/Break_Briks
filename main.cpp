#include <SFML/Graphics.hpp>
#include "GameStates.h"
#include "Menu.h"
#include "Game.h"
#include "EndScreen.h"
#include "GameData.h"

int main() {
    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode({GameData::WINDOW_WIDTH, GameData::WINDOW_HEIGHT}), "Break-Briks!");
    window.setFramerateLimit(60);

    // Initialisation des gestionnaires (en local, pas en global)
    MenuManager menuManager;
    EndScreenManager endScreenManager;
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
                    menuManager.handleEvents(window, *event, currentState);
                    break;
                case GameState::PLAYING:
                    handleGameEvents(window, *event, gameData);
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
                updateGame(gameData, deltaTime, currentState);
                
                // Mettre à jour le titre de la fenêtre si le score ou les vies changent
                if (gameData.lives != lastLives || gameData.score != lastScore) {
                    window.setTitle("Break-Briks! - Score: " + std::to_string(gameData.score) + " - Vies: " + std::to_string(gameData.lives));
                    lastLives = gameData.lives;
                    lastScore = gameData.score;
                }

                // Si le jeu vient de passer à la victoire/défaite, 
                // on réinitialise le jeu en arrière-plan
                if (currentState == GameState::VICTORY || currentState == GameState::GAME_OVER) {
                    // On garde le score final pour l'afficher
                    int finalScore = gameData.score;
                    initializeGame(gameData, window);
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
                drawGame(window, gameData);
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