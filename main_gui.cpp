#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Game.hpp"
#include "Player.hpp"
#include "Role.hpp"

int main() {
    Game game;

    // Create players
    std::vector<Player*> players;
    players.push_back(new Player("Avichay", Role::Governor, &game));
    players.push_back(new Player("Hadar", Role::Spy, &game));
    players.push_back(new Player("Dani", Role::Baron, &game));

    // SFML window setup
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game - GUI");

    // Load font
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Error loading font!\n";
        return 1;
    }

    // Turn text
    sf::Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(26);
    turnText.setPosition(50, 20);
    turnText.setFillColor(sf::Color::White);

    // Result text
    sf::Text resultText;
    resultText.setFont(font);
    resultText.setCharacterSize(22);
    resultText.setPosition(50, 70);
    resultText.setFillColor(sf::Color::Green);

    // ===== GATHER Button =====
    sf::RectangleShape gatherBtn(sf::Vector2f(200, 50));
    gatherBtn.setPosition(50, 120);
    gatherBtn.setFillColor(sf::Color(100, 100, 250));

    sf::Text gatherText("Gather", font, 24);
    gatherText.setPosition(90, 130);
    gatherText.setFillColor(sf::Color::White);

    // ===== TAX Button =====
    sf::RectangleShape taxBtn(sf::Vector2f(200, 50));
    taxBtn.setPosition(50, 190);
    taxBtn.setFillColor(sf::Color(100, 150, 100));

    sf::Text taxText("Tax", font, 24);
    taxText.setPosition(90, 200);
    taxText.setFillColor(sf::Color::White);

    // ===== BRIBE Button =====
    sf::RectangleShape bribeBtn(sf::Vector2f(200, 50));
    bribeBtn.setPosition(50, 260);
    bribeBtn.setFillColor(sf::Color(200, 100, 100));

    sf::Text bribeText("Bribe", font, 24);
    bribeText.setPosition(90, 270);
    bribeText.setFillColor(sf::Color::White);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(sf::Mouse::getPosition(window));
                Player* current = game.currentPlayer();

                // GATHER
                if (gatherBtn.getGlobalBounds().contains(mousePos)) {
                    try {
                        current->gather();
                        resultText.setString(current->getName() + " gathered 1 coin.");
                        resultText.setFillColor(sf::Color::Green);
                    } catch (const std::exception& e) {
                        resultText.setString("Error: " + std::string(e.what()));
                        resultText.setFillColor(sf::Color::Red);
                    }
                }

                // TAX
                if (taxBtn.getGlobalBounds().contains(mousePos)) {
                    try {
                        current->tax();
                        int amount = (current->getRole() == Role::Governor) ? 3 : 2;
                        resultText.setString(current->getName() + " taxed and got " + std::to_string(amount) + " coins.");
                        resultText.setFillColor(sf::Color::Green);
                    } catch (const std::exception& e) {
                        resultText.setString("Error: " + std::string(e.what()));
                        resultText.setFillColor(sf::Color::Red);
                    }
                }

                // BRIBE
                if (bribeBtn.getGlobalBounds().contains(mousePos)) {
                    try {
                        current->bribe();
                        resultText.setString(current->getName() + " paid 4 coins to bribe and earned an extra action.");
                        resultText.setFillColor(sf::Color::Green);
                    } catch (const std::exception& e) {
                        resultText.setString("Error: " + std::string(e.what()));
                        resultText.setFillColor(sf::Color::Red);
                    }
                }
            }
        }

        // Update turn text
        std::string currentName = game.turn();
        int coins = game.getPlayer(currentName)->getCoins();
        turnText.setString("Current turn: " + currentName + " (" + std::to_string(coins) + " coins)");

        // Drawing
        window.clear(sf::Color(30, 30, 30));
        window.draw(turnText);
        window.draw(resultText);

        window.draw(gatherBtn);
        window.draw(gatherText);

        window.draw(taxBtn);
        window.draw(taxText);

        window.draw(bribeBtn);
        window.draw(bribeText);

        window.display();
    }

    return 0;
}
