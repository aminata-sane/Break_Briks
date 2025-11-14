#include "EndScreen.h"
#include "Game.h"
#include <cmath>
#include <string>
#include <iostream>

// Instance globale du gestionnaire d'écran de fin
EndScreenManager endScreenManager;

// Constructeur
EndScreenManager::EndScreenManager() : selectedOption(0), fontLoaded(false) {
    // Essayer de charger une police système
    if (!font.loadFromFile("/System/Library/Fonts/Arial.ttf") && 
        !font.loadFromFile("/System/Library/Fonts/Helvetica.ttc") &&
        !font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
        std::cout << "Impossible de charger une police pour EndScreen, utilisation des rectangles" << std::endl;
        fontLoaded = false;
    } else {
        fontLoaded = true;
        std::cout << "Police chargée avec succès pour EndScreen" << std::endl;
    }
    
    // Configuration des boutons
    playAgainButton.setSize(sf::Vector2f(200.f, 60.f));
    playAgainButton.setPosition(sf::Vector2f(300.f, 350.f));
    playAgainButton.setFillColor(sf::Color(50, 150, 50, 200)); // Vert semi-transparent
    
    menuButton.setSize(sf::Vector2f(200.f, 60.f));
    menuButton.setPosition(sf::Vector2f(300.f, 430.f));
    menuButton.setFillColor(sf::Color(70, 130, 180, 200)); // Bleu semi-transparent
    
    quitButton.setSize(sf::Vector2f(200.f, 60.f));
    quitButton.setPosition(sf::Vector2f(300.f, 510.f));
    quitButton.setFillColor(sf::Color(220, 20, 60, 200)); // Rouge semi-transparent
    
    // Configuration des textes si la police est chargée
    if (fontLoaded) {
        titleText.setFont(font);
        titleText.setCharacterSize(60);
        titleText.setPosition(sf::Vector2f(300.f, 120.f));
        
        scoreText.setFont(font);
        scoreText.setCharacterSize(30);
        scoreText.setPosition(sf::Vector2f(350.f, 200.f));
        scoreText.setFillColor(sf::Color::White);
        
        playAgainText.setFont(font);
        playAgainText.setString("REJOUER");
        playAgainText.setCharacterSize(24);
        playAgainText.setPosition(sf::Vector2f(350.f, 365.f));
        
        menuText.setFont(font);
        menuText.setString("MENU");
        menuText.setCharacterSize(24);
        menuText.setPosition(sf::Vector2f(365.f, 445.f));
        
        quitText.setFont(font);
        quitText.setString("QUITTER");
        quitText.setCharacterSize(24);
        quitText.setPosition(sf::Vector2f(345.f, 525.f));
        
        instructionText.setFont(font);
        instructionText.setString("Flèches pour naviguer, ENTREE pour valider");
        instructionText.setCharacterSize(16);
        instructionText.setPosition(sf::Vector2f(250.f, 570.f));
        instructionText.setFillColor(sf::Color::White);
    }
}

void EndScreenManager::handleEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game, GameState currentState) {
    if (event.is<sf::Event::KeyPressed>()) {
        auto keyEvent = event.getIf<sf::Event::KeyPressed>();
        
        if (keyEvent->code == sf::Keyboard::Key::Up || keyEvent->code == sf::Keyboard::Key::W) {
            selectedOption = (selectedOption - 1 + 3) % 3;
        }
        else if (keyEvent->code == sf::Keyboard::Key::Down || keyEvent->code == sf::Keyboard::Key::S) {
            selectedOption = (selectedOption + 1) % 3;
        }
        else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Space) {
            if (selectedOption == 0) {
                // Rejouer
                initializeGame(game);
                gameState = GameState::PLAYING;
            } else if (selectedOption == 1) {
                // Retour au menu
                gameState = GameState::MENU;
            } else {
                // Quitter
                window.close();
            }
        }
        else if (keyEvent->code == sf::Keyboard::Key::R) {
            // Raccourci pour rejouer
            initializeGame(game);
            gameState = GameState::PLAYING;
        }
        else if (keyEvent->code == sf::Keyboard::Key::M) {
            // Raccourci pour retour au menu
            gameState = GameState::MENU;
        }
        else if (keyEvent->code == sf::Keyboard::Key::Escape) {
            window.close();
        }
    }
    
    // Gestion de la souris
    if (event.is<sf::Event::MouseMoved>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseMoved>();
        sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), 
                             static_cast<float>(mouseEvent->position.y));
        
        if (playAgainButton.getGlobalBounds().contains(mousePos)) {
            selectedOption = 0;
        } else if (menuButton.getGlobalBounds().contains(mousePos)) {
            selectedOption = 1;
        } else if (quitButton.getGlobalBounds().contains(mousePos)) {
            selectedOption = 2;
        }
    }
    
    if (event.is<sf::Event::MouseButtonPressed>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), 
                                 static_cast<float>(mouseEvent->position.y));
            
            if (playAgainButton.getGlobalBounds().contains(mousePos)) {
                initializeGame(game);
                gameState = GameState::PLAYING;
            } else if (menuButton.getGlobalBounds().contains(mousePos)) {
                gameState = GameState::MENU;
            } else if (quitButton.getGlobalBounds().contains(mousePos)) {
                window.close();
            }
        }
    }
}

void EndScreenManager::update(float deltaTime) {
    // Animation des boutons
    if (selectedOption == 0) {
        playAgainButton.setFillColor(sf::Color(80, 200, 80, 255));
        menuButton.setFillColor(sf::Color(70, 130, 180, 200));
        quitButton.setFillColor(sf::Color(220, 20, 60, 200));
    } else if (selectedOption == 1) {
        playAgainButton.setFillColor(sf::Color(50, 150, 50, 200));
        menuButton.setFillColor(sf::Color(100, 160, 220, 255));
        quitButton.setFillColor(sf::Color(220, 20, 60, 200));
    } else {
        playAgainButton.setFillColor(sf::Color(50, 150, 50, 200));
        menuButton.setFillColor(sf::Color(70, 130, 180, 200));
        quitButton.setFillColor(sf::Color(255, 50, 90, 255));
    }
}

void EndScreenManager::drawVictory(sf::RenderWindow& window, const GameData& game) {
    // Fond dégradé doré pour la victoire
    for (int i = 0; i < 600; i += 2) {
        float ratio = static_cast<float>(i) / 600.0f;
        sf::Color topColor(255, 215, 0, 100);    // Or
        sf::Color bottomColor(255, 140, 0, 150); // Orange doré
        
        sf::Color currentColor(
            static_cast<std::uint8_t>(topColor.r * (1 - ratio) + bottomColor.r * ratio),
            static_cast<std::uint8_t>(topColor.g * (1 - ratio) + bottomColor.g * ratio),
            static_cast<std::uint8_t>(topColor.b * (1 - ratio) + bottomColor.b * ratio),
            static_cast<std::uint8_t>(topColor.a * (1 - ratio) + bottomColor.a * ratio)
        );
        
        sf::RectangleShape line(sf::Vector2f(800.f, 2.f));
        line.setPosition(sf::Vector2f(0.f, static_cast<float>(i)));
        line.setFillColor(currentColor);
        window.draw(line);
    }
    
    drawTitle(window, "VICTOIRE!", sf::Color(255, 215, 0));
    drawScore(window, game.score);
    drawButtons(window);
}

void EndScreenManager::drawDefeat(sf::RenderWindow& window, const GameData& game) {
    // Fond dégradé rouge pour la défaite
    for (int i = 0; i < 600; i += 2) {
        float ratio = static_cast<float>(i) / 600.0f;
        sf::Color topColor(139, 0, 0, 100);     // Rouge sombre
        sf::Color bottomColor(220, 20, 60, 150); // Rouge plus vif
        
        sf::Color currentColor(
            static_cast<std::uint8_t>(topColor.r * (1 - ratio) + bottomColor.r * ratio),
            static_cast<std::uint8_t>(topColor.g * (1 - ratio) + bottomColor.g * ratio),
            static_cast<std::uint8_t>(topColor.b * (1 - ratio) + bottomColor.b * ratio),
            static_cast<std::uint8_t>(topColor.a * (1 - ratio) + bottomColor.a * ratio)
        );
        
        sf::RectangleShape line(sf::Vector2f(800.f, 2.f));
        line.setPosition(sf::Vector2f(0.f, static_cast<float>(i)));
        line.setFillColor(currentColor);
        window.draw(line);
    }
    
    drawTitle(window, "DEFAITE", sf::Color(220, 20, 60));
    drawScore(window, game.score);
    drawButtons(window);
}

void EndScreenManager::drawTitle(sf::RenderWindow& window, const std::string& title, sf::Color baseColor) {
    float time = animationClock.getElapsedTime().asSeconds();
    
    if (fontLoaded) {
        // Utiliser le vrai texte avec animation
        titleText.setString(title);
        titleText.setFillColor(baseColor);
        
        // Animation de rebond
        float bounce = 10.f * std::sin(time * 3.0f);
        titleText.setPosition(sf::Vector2f(300.f, 80.f + bounce));
        
        // Ombre pour le contraste
        sf::Text shadowText = titleText;
        shadowText.setFillColor(sf::Color::Black);
        shadowText.setPosition(sf::Vector2f(305.f, 85.f + bounce));
        window.draw(shadowText);
        
        window.draw(titleText);
    } else {
        // Fallback : utiliser les rectangles
        float letterWidth = 80.f;
        float letterHeight = 100.f;
        float startX = 400.f - (title.length() * letterWidth) / 2;
        float baseY = 80.f;
        
        for (size_t i = 0; i < title.size(); ++i) {
            if (title[i] == ' ') continue;
            
            float bounce = 10.f * std::sin(time * 3.0f + i * 0.5f);
            
            sf::RectangleShape letter(sf::Vector2f(letterWidth * 0.9f, letterHeight));
            float x = startX + i * letterWidth;
            float y = baseY + bounce;
            
            letter.setPosition(sf::Vector2f(x, y));
            letter.setFillColor(baseColor);
            window.draw(letter);
            
            sf::RectangleShape border(sf::Vector2f(letterWidth * 0.9f, letterHeight));
            border.setPosition(sf::Vector2f(x, y));
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(4.f);
            border.setOutlineColor(sf::Color::White);
            window.draw(border);
        }
    }
}

void EndScreenManager::drawScore(sf::RenderWindow& window, int score) {
    if (fontLoaded) {
        // Utiliser le vrai texte
        scoreText.setString("SCORE: " + std::to_string(score));
        scoreText.setPosition(sf::Vector2f(330.f, 230.f));
        scoreText.setFillColor(sf::Color::White);
        
        // Ombre pour le contraste
        sf::Text shadowScore = scoreText;
        shadowScore.setFillColor(sf::Color::Black);
        shadowScore.setPosition(sf::Vector2f(333.f, 233.f));
        window.draw(shadowScore);
        
        window.draw(scoreText);
    } else {
        // Fallback : utiliser les rectangles
        std::string scoreString = "SCORE: " + std::to_string(score);
        
        sf::RectangleShape scoreBg(sf::Vector2f(300.f, 50.f));
        scoreBg.setPosition(sf::Vector2f(250.f, 230.f));
        scoreBg.setFillColor(sf::Color::White);
        window.draw(scoreBg);
        
        sf::RectangleShape scoreBorder(sf::Vector2f(300.f, 50.f));
        scoreBorder.setPosition(sf::Vector2f(250.f, 230.f));
        scoreBorder.setFillColor(sf::Color::Transparent);
        scoreBorder.setOutlineThickness(3.f);
        scoreBorder.setOutlineColor(sf::Color::Black);
        window.draw(scoreBorder);
        
        float letterWidth = 15.f;
        float letterHeight = 30.f;
        float startX = 270.f;
        float baseY = 240.f;
        
        for (size_t i = 0; i < scoreString.size(); ++i) {
            if (scoreString[i] == ' ') {
                startX += letterWidth * 0.5f;
                continue;
            }
            
            sf::RectangleShape letter(sf::Vector2f(letterWidth * 0.8f, letterHeight));
            letter.setPosition(sf::Vector2f(startX + i * letterWidth, baseY));
            letter.setFillColor(sf::Color::Black);
            window.draw(letter);
        }
    }
}
}

void EndScreenManager::drawButtons(sf::RenderWindow& window) {
    float time = animationClock.getElapsedTime().asSeconds();
    
    // Dessiner les boutons de base
    window.draw(playAgainButton);
    window.draw(menuButton);
    window.draw(quitButton);
    
    if (fontLoaded) {
        // Utiliser les vrais textes
        playAgainText.setFillColor(selectedOption == 0 ? sf::Color::Yellow : sf::Color::White);
        menuText.setFillColor(selectedOption == 1 ? sf::Color::Yellow : sf::Color::White);
        quitText.setFillColor(selectedOption == 2 ? sf::Color::Yellow : sf::Color::White);
        
        window.draw(playAgainText);
        window.draw(menuText);
        window.draw(quitText);
        window.draw(instructionText);
    } else {
        // Fallback : utiliser les rectangles
        sf::RectangleShape rejBg(sf::Vector2f(160.f, 40.f));
        rejBg.setPosition(sf::Vector2f(320.f, 355.f));
        rejBg.setFillColor(selectedOption == 0 ? sf::Color::Yellow : sf::Color::White);
        window.draw(rejBg);
        
        sf::RectangleShape rejBorder(sf::Vector2f(160.f, 40.f));
        rejBorder.setPosition(sf::Vector2f(320.f, 355.f));
        rejBorder.setFillColor(sf::Color::Transparent);
        rejBorder.setOutlineThickness(3.f);
        rejBorder.setOutlineColor(selectedOption == 0 ? sf::Color::Red : sf::Color::Black);
        window.draw(rejBorder);
        
        sf::RectangleShape menuBg(sf::Vector2f(120.f, 40.f));
        menuBg.setPosition(sf::Vector2f(340.f, 435.f));
        menuBg.setFillColor(selectedOption == 1 ? sf::Color::Yellow : sf::Color::White);
        window.draw(menuBg);
        
        sf::RectangleShape menuBorder(sf::Vector2f(120.f, 40.f));
        menuBorder.setPosition(sf::Vector2f(340.f, 435.f));
        menuBorder.setFillColor(sf::Color::Transparent);
        menuBorder.setOutlineThickness(3.f);
        menuBorder.setOutlineColor(selectedOption == 1 ? sf::Color::Red : sf::Color::Black);
        window.draw(menuBorder);
        
        sf::RectangleShape quitBg(sf::Vector2f(160.f, 40.f));
        quitBg.setPosition(sf::Vector2f(320.f, 515.f));
        quitBg.setFillColor(selectedOption == 2 ? sf::Color::Yellow : sf::Color::White);
        window.draw(quitBg);
        
        sf::RectangleShape quitBorder(sf::Vector2f(160.f, 40.f));
        quitBorder.setPosition(sf::Vector2f(320.f, 515.f));
        quitBorder.setFillColor(sf::Color::Transparent);
        quitBorder.setOutlineThickness(3.f);
        quitBorder.setOutlineColor(selectedOption == 2 ? sf::Color::Red : sf::Color::Black);
        window.draw(quitBorder);
    }
    
    // Indicateur de sélection
    sf::CircleShape selector(20.f);
    selector.setFillColor(sf::Color::Red);
    
    float pulse = 1.0f + 0.3f * std::sin(time * 6.0f);
    selector.setScale(sf::Vector2f(pulse, pulse));
    
    if (selectedOption == 0) {
        selector.setPosition(sf::Vector2f(280.f, 365.f));
    } else if (selectedOption == 1) {
        selector.setPosition(sf::Vector2f(280.f, 445.f));
    } else {
        selector.setPosition(sf::Vector2f(280.f, 525.f));
    }
    window.draw(selector);
}

// Fonctions de compatibilité
void handleEndScreenEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game, GameState currentState) {
    endScreenManager.handleEvents(window, event, gameState, game, currentState);
}

void updateEndScreen(float deltaTime) {
    endScreenManager.update(deltaTime);
}

void drawVictoryScreen(sf::RenderWindow& window, const GameData& game) {
    window.clear(sf::Color::Black);
    endScreenManager.drawVictory(window, game);
    window.display();
}

void drawDefeatScreen(sf::RenderWindow& window, const GameData& game) {
    window.clear(sf::Color::Black);
    endScreenManager.drawDefeat(window, game);
    window.display();
}