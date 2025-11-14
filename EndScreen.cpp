#include "EndScreen.h"
#include "Game.h" // Pour initializeGame
#include <cmath>
#include <string>
#include <iostream>

// Constructeur
EndScreenManager::EndScreenManager() : selectedOption(0), fontLoaded(false) {
    // Charger la police arial.ttf
    if (font.openFromFile("arial.ttf")) {
        fontLoaded = true;
        std::cout << "Police Arial chargée avec succès pour EndScreen !" << std::endl;
        
        // Configuration des textes
        titleText = std::make_unique<sf::Text>(font);
        titleText->setCharacterSize(60);
        titleText->setPosition(sf::Vector2f(300.f, 120.f));
        
        scoreText = std::make_unique<sf::Text>(font);
        scoreText->setCharacterSize(30);
        scoreText->setPosition(sf::Vector2f(320.f, 200.f));
        scoreText->setFillColor(sf::Color::White);
        
        playAgainText = std::make_unique<sf::Text>(font);
        playAgainText->setString("REJOUER");
        playAgainText->setCharacterSize(24);
        playAgainText->setPosition(sf::Vector2f(350.f, 365.f));
        
        menuText = std::make_unique<sf::Text>(font);
        menuText->setString("MENU");
        menuText->setCharacterSize(24);
        menuText->setPosition(sf::Vector2f(365.f, 445.f));
        
        quitText = std::make_unique<sf::Text>(font);
        quitText->setString("QUITTER");
        quitText->setCharacterSize(24);
        quitText->setPosition(sf::Vector2f(345.f, 525.f));
        
        instructionText = std::make_unique<sf::Text>(font);
        instructionText->setString("Flèches pour naviguer, ENTREE pour valider");
        instructionText->setCharacterSize(16);
        instructionText->setPosition(sf::Vector2f(250.f, 570.f));
        instructionText->setFillColor(sf::Color::White);
        
    } else {
        fontLoaded = false;
        std::cout << "Impossible de charger arial.ttf pour EndScreen, utilisation des rectangles de substitution" << std::endl;
    }
    
    playAgainButton.setSize(sf::Vector2f(200.f, 60.f));
    playAgainButton.setPosition(sf::Vector2f(300.f, 350.f));
    playAgainButton.setFillColor(sf::Color(50, 150, 50, 200)); 
    
    menuButton.setSize(sf::Vector2f(200.f, 60.f));
    menuButton.setPosition(sf::Vector2f(300.f, 430.f));
    menuButton.setFillColor(sf::Color(70, 130, 180, 200));
    
    quitButton.setSize(sf::Vector2f(200.f, 60.f));
    quitButton.setPosition(sf::Vector2f(300.f, 510.f));
    quitButton.setFillColor(sf::Color(220, 20, 60, 200));
}

void EndScreenManager::handleEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState, GameData& game) {
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
                initializeGame(game, window);
                gameState = GameState::PLAYING;
            } else if (selectedOption == 1) {
                // Retour au menu
                gameState = GameState::MENU;
            } else {
                // Quitter
                window.close();
            }
        }
    }
}

void EndScreenManager::update(float deltaTime) {
    // Animation simple
}

void EndScreenManager::drawVictory(sf::RenderWindow& window, const GameData& game) {
    drawCommon(window, game, "VICTOIRE!", sf::Color(255, 215, 0));
}

void EndScreenManager::drawDefeat(sf::RenderWindow& window, const GameData& game) {
    drawCommon(window, game, "DEFAITE", sf::Color(220, 20, 60));
}

void EndScreenManager::drawCommon(sf::RenderWindow& window, const GameData& game, const std::string& title, sf::Color color) {
    // Fond sombre
    window.clear(sf::Color(10, 10, 20));
    
    // Dessiner le titre
    float time = animationClock.getElapsedTime().asSeconds();
    
    if (fontLoaded) {
        // Utiliser le vrai texte avec animation
        titleText->setString(title);
        titleText->setFillColor(color);
        
        // Animation de rebond
        float bounce = 10.f * std::sin(time * 3.0f);
        titleText->setPosition(sf::Vector2f(300.f, 80.f + bounce));
        
        // Ombre pour le contraste
        sf::Text shadowText = *titleText;
        shadowText.setFillColor(sf::Color::Black);
        shadowText.setPosition(sf::Vector2f(305.f, 85.f + bounce));
        window.draw(shadowText);
        
        window.draw(*titleText);
        
        // Afficher le score avec vrai texte
        scoreText->setString("SCORE: " + std::to_string(game.score));
        window.draw(*scoreText);
    } else {
        // Fallback : utiliser les rectangles
        float letterWidth = 80.f;
        float letterHeight = 100.f;
        float startX = 400.f - (title.length() * letterWidth) / 2;
        float baseY = 80.f;
        
        for (size_t i = 0; i < title.size(); ++i) {
            if (title[i] == ' ') continue;
            
            // Animation de rebond
            float bounce = 10.f * std::sin(time * 3.0f + i * 0.5f);
            
            sf::RectangleShape letter(sf::Vector2f(letterWidth * 0.9f, letterHeight));
            float x = startX + i * letterWidth;
            float y = baseY + bounce;
            
            letter.setPosition(sf::Vector2f(x, y));
            letter.setFillColor(color);
            window.draw(letter);
            
            // Contour blanc pour la visibilité
            sf::RectangleShape border(sf::Vector2f(letterWidth * 0.9f, letterHeight));
            border.setPosition(sf::Vector2f(x, y));
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(4.f);
            border.setOutlineColor(sf::Color::White);
            window.draw(border);
        }
        
        // Afficher le score avec rectangles
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
    }
    
    // Dessiner les boutons
    drawButtons(window);
}

void EndScreenManager::drawButtons(sf::RenderWindow& window) {
    float time = animationClock.getElapsedTime().asSeconds();
    
    if (fontLoaded) {
        // Utiliser les vrais textes
        
        // Bouton REJOUER
        playAgainText->setFillColor(selectedOption == 0 ? sf::Color::Yellow : sf::Color::White);
        if (selectedOption == 0) {
            playAgainText->setScale(sf::Vector2f(1.1f, 1.1f));
        } else {
            playAgainText->setScale(sf::Vector2f(1.0f, 1.0f));
        }
        window.draw(*playAgainText);
        
        // Bouton MENU
        menuText->setFillColor(selectedOption == 1 ? sf::Color::Yellow : sf::Color::White);
        if (selectedOption == 1) {
            menuText->setScale(sf::Vector2f(1.1f, 1.1f));
        } else {
            menuText->setScale(sf::Vector2f(1.0f, 1.0f));
        }
        window.draw(*menuText);
        
        // Bouton QUITTER
        quitText->setFillColor(selectedOption == 2 ? sf::Color::Yellow : sf::Color::White);
        if (selectedOption == 2) {
            quitText->setScale(sf::Vector2f(1.1f, 1.1f));
        } else {
            quitText->setScale(sf::Vector2f(1.0f, 1.0f));
        }
        window.draw(*quitText);
        
    } else {
        // Fallback : utiliser les rectangles
        
        // Texte "REJOUER"
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
        
        // Texte "MENU"
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
        
        // Texte "QUITTER"
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

sf::Color EndScreenManager::getGradientColor(float t) {
    // Couleur arc-en-ciel simple
    t = fmod(t, 1.0f);
    if (t < 0) t += 1.0f;
    
    if (t < 0.33f) {
        return sf::Color(255, static_cast<std::uint8_t>(t * 765), 0);
    } else if (t < 0.67f) {
        return sf::Color(static_cast<std::uint8_t>((0.67f - t) * 765), 255, 0);
    } else {
        return sf::Color(0, static_cast<std::uint8_t>((1.0f - t) * 765), 255);
    }
}