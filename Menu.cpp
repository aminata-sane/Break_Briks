#include "Menu.h"
#include <iostream>
#include <random>
#include <cstdint>

// Instance globale du gestionnaire de menu
MenuManager menuManager;

// Constructeur de MenuManager
MenuManager::MenuManager() : selectedButton(0), titleScale(1.0f), titleRotation(0.0f), fontLoaded(false) {
    // Essayer de charger une police système
    if (!font.loadFromFile("/System/Library/Fonts/Arial.ttf") && 
        !font.loadFromFile("/System/Library/Fonts/Helvetica.ttc") &&
        !font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
        std::cout << "Impossible de charger une police, utilisation des rectangles" << std::endl;
        fontLoaded = false;
    } else {
        fontLoaded = true;
        std::cout << "Police chargée avec succès" << std::endl;
    }
    
    // Configuration des boutons
    startButton.setSize(sf::Vector2f(200.f, 60.f));
    startButton.setPosition(sf::Vector2f(300.f, 350.f));
    startButton.setFillColor(sf::Color(70, 130, 180, 200)); // Bleu semi-transparent
    
    quitButton.setSize(sf::Vector2f(200.f, 60.f));
    quitButton.setPosition(sf::Vector2f(300.f, 450.f));
    quitButton.setFillColor(sf::Color(220, 20, 60, 200)); // Rouge semi-transparent
    
    // Configuration du fond
    background.setSize(sf::Vector2f(800.f, 600.f));
    background.setPosition(sf::Vector2f(0.f, 0.f));
    
    // Si on a une font, configurer les textes
    if (fontLoaded) {
        titleText.setFont(font);
        titleText.setString("BREAK-BRIKS");
        titleText.setCharacterSize(60);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(sf::Vector2f(250.f, 120.f));
        
        raquetteText.setFont(font);
        raquetteText.setString("RAQUETTE");
        raquetteText.setCharacterSize(30);
        raquetteText.setFillColor(sf::Color::White);
        raquetteText.setPosition(sf::Vector2f(330.f, 365.f));
        
        canonText.setFont(font);
        canonText.setString("CANON");
        canonText.setCharacterSize(30);
        canonText.setFillColor(sf::Color(180, 180, 180));
        canonText.setPosition(sf::Vector2f(360.f, 465.f));
    }
    
    // Créer les particules initiales
    createParticles();
}

void MenuManager::handleEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState) {
    if (event.is<sf::Event::KeyPressed>()) {
        auto keyEvent = event.getIf<sf::Event::KeyPressed>();
        
        if (keyEvent->code == sf::Keyboard::Key::Up || keyEvent->code == sf::Keyboard::Key::W) {
            selectedButton = (selectedButton - 1 + 2) % 2;
        }
        else if (keyEvent->code == sf::Keyboard::Key::Down || keyEvent->code == sf::Keyboard::Key::S) {
            selectedButton = (selectedButton + 1) % 2;
        }
        else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Space) {
            if (selectedButton == 0) {
                gameState = GameState::PLAYING;
            } else {
                window.close();
            }
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
        
        if (startButton.getGlobalBounds().contains(mousePos)) {
            selectedButton = 0;
        } else if (quitButton.getGlobalBounds().contains(mousePos)) {
            selectedButton = 1;
        }
    }
    
    if (event.is<sf::Event::MouseButtonPressed>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), 
                                 static_cast<float>(mouseEvent->position.y));
            
            if (startButton.getGlobalBounds().contains(mousePos)) {
                gameState = GameState::PLAYING;
            } else if (quitButton.getGlobalBounds().contains(mousePos)) {
                window.close();
            }
        }
    }
}

void MenuManager::update(float deltaTime) {
    // Animation du titre
    float time = animationClock.getElapsedTime().asSeconds();
    titleScale = 1.0f + 0.1f * std::sin(time * 2.0f);
    titleRotation = 2.0f * std::sin(time * 0.5f);
    
    // Mettre à jour les particules
    updateParticles(deltaTime);
    
    // Créer de nouvelles particules
    if (particles.size() < 50) {
        createParticles();
    }
    
    // Animation des boutons
    if (selectedButton == 0) {
        startButton.setFillColor(sf::Color(100, 160, 220, 255));
        quitButton.setFillColor(sf::Color(220, 20, 60, 200));
    } else {
        startButton.setFillColor(sf::Color(70, 130, 180, 200));
        quitButton.setFillColor(sf::Color(255, 50, 90, 255));
    }
}

void MenuManager::draw(sf::RenderWindow& window) {
    // Dégradé de fond animé
    float time = animationClock.getElapsedTime().asSeconds();
    sf::Color topColor = getGradientColor(time * 0.5f);
    sf::Color bottomColor = getGradientColor(time * 0.5f + 0.5f);
    
    // Créer un dégradé vertical simple avec des rectangles
    for (int i = 0; i < 600; i += 2) {
        float ratio = static_cast<float>(i) / 600.0f;
        sf::Color currentColor(
            static_cast<std::uint8_t>(topColor.r * (1 - ratio) + bottomColor.r * ratio),
            static_cast<std::uint8_t>(topColor.g * (1 - ratio) + bottomColor.g * ratio),
            static_cast<std::uint8_t>(topColor.b * (1 - ratio) + bottomColor.b * ratio)
        );
        
        sf::RectangleShape line(sf::Vector2f(800.f, 2.f));
        line.setPosition(sf::Vector2f(0.f, static_cast<float>(i)));
        line.setFillColor(currentColor);
        window.draw(line);
    }
    
    // Dessiner les particules
    drawParticles(window);
    
    // Dessiner le titre
    drawTitle(window);
    
    // Dessiner les boutons
    drawButtons(window);
}

void MenuManager::createParticles() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(0.f, 800.f);
    std::uniform_real_distribution<float> yDist(0.f, 600.f);
    std::uniform_real_distribution<float> velDist(-50.f, 50.f);
    std::uniform_real_distribution<float> lifeDist(2.f, 5.f);
    std::uniform_int_distribution<int> colorDist(0, 3);
    
    for (int i = 0; i < 5; ++i) {
        sf::Vector2f pos(xDist(gen), yDist(gen));
        sf::Vector2f vel(velDist(gen), velDist(gen));
        
        sf::Color colors[] = {
            sf::Color(255, 100, 100, 150),
            sf::Color(100, 255, 100, 150),
            sf::Color(100, 100, 255, 150),
            sf::Color(255, 255, 100, 150)
        };
        
        particles.emplace_back(pos, vel, colors[colorDist(gen)], lifeDist(gen));
    }
}

void MenuManager::updateParticles(float deltaTime) {
    for (auto it = particles.begin(); it != particles.end();) {
        it->position += it->velocity * deltaTime;
        it->life -= deltaTime;
        
        // Fade out effect
        float alpha = (it->life / it->maxLife) * 150.0f;
        it->color.a = static_cast<std::uint8_t>(std::max(0.0f, alpha));
        
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void MenuManager::drawParticles(sf::RenderWindow& window) {
    for (const auto& particle : particles) {
        sf::CircleShape circle(3.f);
        circle.setPosition(particle.position);
        circle.setFillColor(particle.color);
        window.draw(circle);
    }
}

void MenuManager::drawTitle(sf::RenderWindow& window) {
    float time = animationClock.getElapsedTime().asSeconds();
    
    if (fontLoaded) {
        // Utiliser le vrai texte avec animation
        titleText.setPosition(sf::Vector2f(300.f, 120.f + 10.f * std::sin(time * 2.0f)));
        titleText.setFillColor(sf::Color(255, 215, 0)); // Couleur dorée
        
        // Effet d'ombre
        sf::Text shadowText = titleText;
        shadowText.setPosition(sf::Vector2f(305.f, 125.f + 10.f * std::sin(time * 2.0f)));
        shadowText.setFillColor(sf::Color::Black);
        window.draw(shadowText);
        
        window.draw(titleText);
    } else {
        // Fallback : utiliser les rectangles comme avant
        std::string title = "BREAK-BRIKS";
        float letterWidth = 60.f;
        float letterHeight = 80.f;
        float startX = 400.f - (title.length() * letterWidth) / 2;
        float baseY = 100.f;
        
        for (size_t i = 0; i < title.size(); ++i) {
            if (title[i] == '-') {
                sf::RectangleShape dash(sf::Vector2f(40.f, 10.f));
                float x = startX + i * letterWidth + 10;
                float y = baseY + letterHeight * 0.5f + 5.f * std::sin(time * 2.0f + i * 0.5f);
                dash.setPosition(sf::Vector2f(x, y));
                dash.setFillColor(sf::Color::Yellow);
                window.draw(dash);
                continue;
            }
            
            sf::RectangleShape letter(sf::Vector2f(letterWidth * 0.9f, letterHeight));
            float x = startX + i * letterWidth;
            float y = baseY + 10.f * std::sin(time * 2.0f + i * 0.5f);
            
            letter.setPosition(sf::Vector2f(x, y));
            letter.setFillColor(i % 2 == 0 ? sf::Color::Cyan : sf::Color::Magenta);
            window.draw(letter);
            
            sf::RectangleShape border(sf::Vector2f(letterWidth * 0.9f, letterHeight));
            border.setPosition(sf::Vector2f(x, y));
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(3.f);
            border.setOutlineColor(sf::Color::White);
            window.draw(border);
        }
    }
}

void MenuManager::drawButtons(sf::RenderWindow& window) {
    float time = animationClock.getElapsedTime().asSeconds();
    
    // Dessiner les boutons de base
    window.draw(startButton);
    window.draw(quitButton);
    
    if (fontLoaded) {
        // Utiliser les vrais textes
        // Texte RAQUETTE
        raquetteText.setPosition(sf::Vector2f(330.f, 365.f));
        raquetteText.setFillColor(selectedButton == 0 ? sf::Color::Yellow : sf::Color::White);
        window.draw(raquetteText);
        
        // Texte CANON
        canonText.setPosition(sf::Vector2f(360.f, 465.f));
        canonText.setFillColor(selectedButton == 1 ? sf::Color::Yellow : sf::Color(180, 180, 180));
        window.draw(canonText);
        
        // Instruction
        sf::Text instructionText;
        instructionText.setFont(font);
        instructionText.setString("Utilisez les flèches pour naviguer, ENTREE pour valider");
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setPosition(sf::Vector2f(210.f, 530.f));
        window.draw(instructionText);
    } else {
        // Fallback : utiliser les rectangles
        sf::RectangleShape raquetteTextBg(sf::Vector2f(160.f, 40.f));
        raquetteTextBg.setPosition(sf::Vector2f(320.f, 355.f));
        raquetteTextBg.setFillColor(selectedButton == 0 ? sf::Color::Yellow : sf::Color::White);
        window.draw(raquetteTextBg);
        
        sf::RectangleShape raquetteBorder(sf::Vector2f(160.f, 40.f));
        raquetteBorder.setPosition(sf::Vector2f(320.f, 355.f));
        raquetteBorder.setFillColor(sf::Color::Transparent);
        raquetteBorder.setOutlineThickness(3.f);
        raquetteBorder.setOutlineColor(selectedButton == 0 ? sf::Color::Red : sf::Color::Black);
        window.draw(raquetteBorder);
        
        sf::RectangleShape canonTextBg(sf::Vector2f(120.f, 40.f));
        canonTextBg.setPosition(sf::Vector2f(340.f, 455.f));
        canonTextBg.setFillColor(selectedButton == 1 ? sf::Color::Yellow : sf::Color(180, 180, 180));
        window.draw(canonTextBg);
        
        sf::RectangleShape canonBorder(sf::Vector2f(120.f, 40.f));
        canonBorder.setPosition(sf::Vector2f(340.f, 455.f));
        canonBorder.setFillColor(sf::Color::Transparent);
        canonBorder.setOutlineThickness(3.f);
        canonBorder.setOutlineColor(selectedButton == 1 ? sf::Color::Red : sf::Color::Black);
        window.draw(canonBorder);
        
        sf::RectangleShape instruction(sf::Vector2f(400.f, 30.f));
        instruction.setPosition(sf::Vector2f(200.f, 520.f));
        instruction.setFillColor(sf::Color::White);
        window.draw(instruction);
        
        sf::RectangleShape instrBorder(sf::Vector2f(400.f, 30.f));
        instrBorder.setPosition(sf::Vector2f(200.f, 520.f));
        instrBorder.setFillColor(sf::Color::Transparent);
        instrBorder.setOutlineThickness(2.f);
        instrBorder.setOutlineColor(sf::Color::Black);
        window.draw(instrBorder);
    }
    
    // Indicateur de sélection
    sf::CircleShape selector(15.f);
    selector.setFillColor(sf::Color::Red);
    float pulse = 1.0f + 0.5f * std::sin(time * 8.0f);
    selector.setScale(sf::Vector2f(pulse, pulse));
    
    if (selectedButton == 0) {
        selector.setPosition(sf::Vector2f(280.f, 365.f));
    } else {
        selector.setPosition(sf::Vector2f(280.f, 465.f));
    }
    window.draw(selector);
}

sf::Color MenuManager::getGradientColor(float t) {
    // Créer une couleur arc-en-ciel basée sur t (0-1)
    t = fmod(t, 1.0f);
    if (t < 0) t += 1.0f;
    
    float r, g, b;
    if (t < 1.0f/6.0f) {
        r = 1.0f; g = t * 6.0f; b = 0.0f;
    } else if (t < 2.0f/6.0f) {
        r = 1.0f - (t - 1.0f/6.0f) * 6.0f; g = 1.0f; b = 0.0f;
    } else if (t < 3.0f/6.0f) {
        r = 0.0f; g = 1.0f; b = (t - 2.0f/6.0f) * 6.0f;
    } else if (t < 4.0f/6.0f) {
        r = 0.0f; g = 1.0f - (t - 3.0f/6.0f) * 6.0f; b = 1.0f;
    } else if (t < 5.0f/6.0f) {
        r = (t - 4.0f/6.0f) * 6.0f; g = 0.0f; b = 1.0f;
    } else {
        r = 1.0f; g = 0.0f; b = 1.0f - (t - 5.0f/6.0f) * 6.0f;
    }
    
    return sf::Color(
        static_cast<std::uint8_t>(r * 255),
        static_cast<std::uint8_t>(g * 255),
        static_cast<std::uint8_t>(b * 255)
    );
}

// Fonctions de compatibilité
void handleMenuEvents(sf::RenderWindow& window, const sf::Event& event, GameState& gameState) {
    menuManager.handleEvents(window, event, gameState);
}

void updateMenu(float deltaTime) {
    menuManager.update(deltaTime);
}

void drawMenu(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    menuManager.draw(window);
    window.display();
}