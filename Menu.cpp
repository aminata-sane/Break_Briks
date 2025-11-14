#include "Menu.h"
#include <iostream>

// Constructeur de MenuManager
MenuManager::MenuManager() 
    : selectedButton(0), 
      titleScale(1.0f), 
      titleRotation(0.0f), 
      gen(std::random_device{}()),
      fontLoaded(false)
{
    // Charger la police arial.ttf
    if (font.openFromFile("arial.ttf")) {
        fontLoaded = true;
        std::cout << "Police Arial chargée avec succès !" << std::endl;
        
        // Configuration des textes
        titleText = std::make_unique<sf::Text>(font);
        titleText->setString("BREAK-BRIKS");
        titleText->setCharacterSize(60);
        titleText->setFillColor(sf::Color(255, 215, 0)); // Couleur dorée
        
        jouerText = std::make_unique<sf::Text>(font);
        jouerText->setString("JOUER");
        jouerText->setCharacterSize(28);
        jouerText->setPosition(sf::Vector2f(360.f, 365.f));
        
        quitterText = std::make_unique<sf::Text>(font);
        quitterText->setString("QUITTER");
        quitterText->setCharacterSize(28);
        quitterText->setPosition(sf::Vector2f(340.f, 465.f));
        
        instructionText = std::make_unique<sf::Text>(font);
        instructionText->setString("Utilisez les flèches pour naviguer, ENTREE pour valider");
        instructionText->setCharacterSize(16);
        instructionText->setFillColor(sf::Color::White);
        instructionText->setPosition(sf::Vector2f(200.f, 530.f));
        
    } else {
        fontLoaded = false;
        std::cout << "Impossible de charger arial.ttf, utilisation des rectangles de substitution" << std::endl;
    }
    
    startButton.setSize(sf::Vector2f(200.f, 60.f));
    startButton.setPosition(sf::Vector2f(300.f, 350.f));
    startButton.setFillColor(sf::Color(70, 130, 180, 200));
    
    quitButton.setSize(sf::Vector2f(200.f, 60.f));
    quitButton.setPosition(sf::Vector2f(300.f, 450.f));
    quitButton.setFillColor(sf::Color(220, 20, 60, 200));
    
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
    
    if (event.is<sf::Event::MouseMoved>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseMoved>();
        sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), static_cast<float>(mouseEvent->position.y));
        if (startButton.getGlobalBounds().contains(mousePos)) {
            selectedButton = 0;
        } else if (quitButton.getGlobalBounds().contains(mousePos)) {
            selectedButton = 1;
        }
    }
    
    if (event.is<sf::Event::MouseButtonPressed>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), static_cast<float>(mouseEvent->position.y));
            if (startButton.getGlobalBounds().contains(mousePos)) {
                gameState = GameState::PLAYING;
            } else if (quitButton.getGlobalBounds().contains(mousePos)) {
                window.close();
            }
        }
    }
}

void MenuManager::update(float deltaTime) {
    updateParticles(deltaTime);
}

void MenuManager::draw(sf::RenderWindow& window) {
    // Fond simple
    window.clear(sf::Color(20, 30, 50));
    
    // Dessiner les particules
    drawParticles(window);
    
    // Dessiner le titre
    drawTitle(window);
    
    // Dessiner les boutons
    drawButtons(window);
}

void MenuManager::createParticles() {
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
    
    // Ajouter de nouvelles particules occasionnellement
    if (particles.size() < 50 && gen() % 100 == 0) {
        createParticles();
    }
}

void MenuManager::drawParticles(sf::RenderWindow& window) {
    for (const auto& particle : particles) {
        sf::CircleShape shape(3.f);
        shape.setPosition(particle.position);
        shape.setFillColor(particle.color);
        window.draw(shape);
    }
}

void MenuManager::drawTitle(sf::RenderWindow& window) {
    float time = animationClock.getElapsedTime().asSeconds();
    
    if (fontLoaded) {
        // Utiliser le vrai texte avec animation
        titleText->setPosition(sf::Vector2f(300.f, 120.f + 10.f * std::sin(time * 2.0f)));
        
        // Effet d'ombre pour le contraste
        sf::Text shadowText = *titleText;
        shadowText.setPosition(sf::Vector2f(305.f, 125.f + 10.f * std::sin(time * 2.0f)));
        shadowText.setFillColor(sf::Color::Black);
        window.draw(shadowText);
        
        // Texte principal
        window.draw(*titleText);
    } else {
        // Fallback : utiliser les rectangles comme avant
        std::string title = "BREAK-BRIKS";
        float letterWidth = 60.f;
        float letterHeight = 80.f;
        float startX = 400.f - (title.length() * letterWidth) / 2;
        float baseY = 100.f;
        
        for (size_t i = 0; i < title.size(); ++i) {
            if (title[i] == '-') {
                // Dessiner un trait pour le tiret
                sf::RectangleShape dash(sf::Vector2f(40.f, 10.f));
                float x = startX + i * letterWidth + 10;
                float y = baseY + letterHeight * 0.5f + 5.f * std::sin(time * 2.0f + i * 0.5f);
                dash.setPosition(sf::Vector2f(x, y));
                dash.setFillColor(sf::Color::Yellow);
                window.draw(dash);
                continue;
            }
            
            // Créer une lettre colorée
            sf::RectangleShape letter(sf::Vector2f(letterWidth * 0.9f, letterHeight));
            float x = startX + i * letterWidth;
            float y = baseY + 10.f * std::sin(time * 2.0f + i * 0.5f);
            
            letter.setPosition(sf::Vector2f(x, y));
            
            // Couleur alternée
            if (i % 2 == 0) {
                letter.setFillColor(sf::Color::Cyan);
            } else {
                letter.setFillColor(sf::Color::Magenta);
            }
            
            window.draw(letter);
            
            // Bordure blanche
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
        jouerText->setFillColor(selectedButton == 0 ? sf::Color::Yellow : sf::Color::White);
        quitterText->setFillColor(selectedButton == 1 ? sf::Color::Yellow : sf::Color::White);
        
        window.draw(*jouerText);
        window.draw(*quitterText);
        window.draw(*instructionText);
    } else {
        // Fallback : utiliser les rectangles
        sf::RectangleShape jouerBg(sf::Vector2f(120.f, 40.f));
        jouerBg.setPosition(sf::Vector2f(340.f, 355.f));
        jouerBg.setFillColor(selectedButton == 0 ? sf::Color::Yellow : sf::Color::White);
        window.draw(jouerBg);
        
        sf::RectangleShape jouerBorder(sf::Vector2f(120.f, 40.f));
        jouerBorder.setPosition(sf::Vector2f(340.f, 355.f));
        jouerBorder.setFillColor(sf::Color::Transparent);
        jouerBorder.setOutlineThickness(3.f);
        jouerBorder.setOutlineColor(selectedButton == 0 ? sf::Color::Red : sf::Color::Black);
        window.draw(jouerBorder);
        
        sf::RectangleShape quitterBg(sf::Vector2f(140.f, 40.f));
        quitterBg.setPosition(sf::Vector2f(330.f, 455.f));
        quitterBg.setFillColor(selectedButton == 1 ? sf::Color::Yellow : sf::Color(180, 180, 180));
        window.draw(quitterBg);
        
        sf::RectangleShape quitterBorder(sf::Vector2f(140.f, 40.f));
        quitterBorder.setPosition(sf::Vector2f(330.f, 455.f));
        quitterBorder.setFillColor(sf::Color::Transparent);
        quitterBorder.setOutlineThickness(3.f);
        quitterBorder.setOutlineColor(selectedButton == 1 ? sf::Color::Red : sf::Color::Black);
        window.draw(quitterBorder);
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
        r = (2.0f/6.0f - t) * 6.0f; g = 1.0f; b = 0.0f;
    } else if (t < 3.0f/6.0f) {
        r = 0.0f; g = 1.0f; b = (t - 2.0f/6.0f) * 6.0f;
    } else if (t < 4.0f/6.0f) {
        r = 0.0f; g = (4.0f/6.0f - t) * 6.0f; b = 1.0f;
    } else if (t < 5.0f/6.0f) {
        r = (t - 4.0f/6.0f) * 6.0f; g = 0.0f; b = 1.0f;
    } else {
        r = 1.0f; g = 0.0f; b = (1.0f - t) * 6.0f;
    }
    
    return sf::Color(static_cast<std::uint8_t>(r * 255), static_cast<std::uint8_t>(g * 255), static_cast<std::uint8_t>(b * 255));
}