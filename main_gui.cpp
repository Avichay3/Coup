#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Game.hpp"
#include "Player.hpp"
#include "Role.hpp"

int main() {
    Game game;

    std::vector<Player*> players;
    players.push_back(new Player("Avichay", Role::Governor, &game));
    players.push_back(new Player("Hadar", Role::Spy, &game));
    players.push_back(new Player("Dani", Role::Baron, &game));

    sf::RenderWindow window(sf::VideoMode(900, 600), "Coup Game - GUI");

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Error loading font!\n";
        return 1;
    }

    // Turn info
    sf::Text turnText("Turn: ", font, 26);
    turnText.setPosition(50, 20);
    turnText.setFillColor(sf::Color::White);

    sf::Text resultText("", font, 22);
    resultText.setPosition(50, 70);
    resultText.setFillColor(sf::Color::Green);

    // Action Buttons
    sf::RectangleShape gatherBtn(sf::Vector2f(200, 50));
    gatherBtn.setPosition(50, 120);
    gatherBtn.setFillColor(sf::Color(100, 100, 250));
    sf::Text gatherText("Gather", font, 24);
    gatherText.setPosition(90, 130);
    gatherText.setFillColor(sf::Color::White);

    sf::RectangleShape taxBtn(sf::Vector2f(200, 50));
    taxBtn.setPosition(50, 190);
    taxBtn.setFillColor(sf::Color(100, 150, 100));
    sf::Text taxText("Tax", font, 24);
    taxText.setPosition(90, 200);
    taxText.setFillColor(sf::Color::White);

    sf::RectangleShape bribeBtn(sf::Vector2f(200, 50));
    bribeBtn.setPosition(50, 260);
    bribeBtn.setFillColor(sf::Color(200, 100, 100));
    sf::Text bribeText("Bribe", font, 24);
    bribeText.setPosition(90, 270);
    bribeText.setFillColor(sf::Color::White);

    sf::RectangleShape coupBtn(sf::Vector2f(200, 50));
    coupBtn.setPosition(50, 330);
    coupBtn.setFillColor(sf::Color(250, 150, 50));
    sf::Text coupText("Coup", font, 24);
    coupText.setPosition(90, 340);
    coupText.setFillColor(sf::Color::White);

    // Coup Target Selection
    std::vector<sf::RectangleShape> targetButtons;
    std::vector<sf::Text> targetTexts;
    bool choosingTarget = false;

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(sf::Mouse::getPosition(window));
                Player* current = game.currentPlayer();

                if (!choosingTarget) {
                    if (gatherBtn.getGlobalBounds().contains(mouse)) {
                        try {
                            current->gather();
                            resultText.setString(current->getName() + " gathered 1 coin.");
                            resultText.setFillColor(sf::Color::Green);
                        } catch (std::exception& e) {
                            resultText.setString(e.what());
                            resultText.setFillColor(sf::Color::Red);
                        }
                    }

                    if (taxBtn.getGlobalBounds().contains(mouse)) {
                        try {
                            current->tax();
                            int amount = (current->getRole() == Role::Governor) ? 3 : 2;
                            resultText.setString(current->getName() + " taxed and got " + std::to_string(amount) + " coins.");
                            resultText.setFillColor(sf::Color::Green);
                        } catch (std::exception& e) {
                            resultText.setString(e.what());
                            resultText.setFillColor(sf::Color::Red);
                        }
                    }

                    if (bribeBtn.getGlobalBounds().contains(mouse)) {
                        try {
                            current->bribe();
                            resultText.setString(current->getName() + " bribed and earned extra action.");
                            resultText.setFillColor(sf::Color::Green);
                        } catch (std::exception& e) {
                            resultText.setString(e.what());
                            resultText.setFillColor(sf::Color::Red);
                        }
                    }

                    if (coupBtn.getGlobalBounds().contains(mouse)) {
                        if (current->getCoins() < 7) {
                            resultText.setString("You need at least 7 coins to perform a coup.");
                            resultText.setFillColor(sf::Color::Red);
                        } else {
                            targetButtons.clear();
                            targetTexts.clear();
                            choosingTarget = true;

                            int y = 420;
                            for (Player* p : players) {
                                if (p->isAlive() && p != current) {
                                    sf::RectangleShape btn(sf::Vector2f(200, 40));
                                    btn.setPosition(300, y);
                                    btn.setFillColor(sf::Color(90, 90, 90));
                                    targetButtons.push_back(btn);

                                    sf::Text txt(p->getName(), font, 20);
                                    txt.setPosition(310, y + 5);
                                    txt.setFillColor(sf::Color::White);
                                    targetTexts.push_back(txt);

                                    y += 50;
                                }
                            }

                            resultText.setString("Choose a player to coup.");
                            resultText.setFillColor(sf::Color::Yellow);
                        }
                    }
                } else {
                    // Handle coup target
                    for (size_t i = 0; i < targetButtons.size(); ++i) {
                        if (targetButtons[i].getGlobalBounds().contains(mouse)) {
                            Player* target = game.getPlayer(targetTexts[i].getString());
                            try {
                                current->coup(*target);
                                resultText.setString(current->getName() + " performed a coup on " + target->getName() + "!");
                                resultText.setFillColor(sf::Color::Green);
                            } catch (std::exception& e) {
                                resultText.setString(e.what());
                                resultText.setFillColor(sf::Color::Red);
                            }
                            choosingTarget = false;
                            break;
                        }
                    }
                }
            }
        }

        Player* current = game.currentPlayer();
        turnText.setString("Current turn: " + current->getName() + " (" + std::to_string(current->getCoins()) + " coins)");

        window.clear(sf::Color(30, 30, 30));
        window.draw(turnText);
        window.draw(resultText);

        window.draw(gatherBtn); window.draw(gatherText);
        window.draw(taxBtn);    window.draw(taxText);
        window.draw(bribeBtn);  window.draw(bribeText);
        window.draw(coupBtn);   window.draw(coupText);

        // Draw target selection if active
        if (choosingTarget) {
            for (size_t i = 0; i < targetButtons.size(); ++i) {
                window.draw(targetButtons[i]);
                window.draw(targetTexts[i]);
            }
        }

        // === Player list display ===
        sf::Text playerListTitle("Players:", font, 20);
        playerListTitle.setPosition(700, 30);
        playerListTitle.setFillColor(sf::Color::White);
        window.draw(playerListTitle);

        int py = 60;
        for (Player* p : players) {
            if (!p->isAlive()) continue;
            std::string info = p->getName() + " - " + std::to_string(p->getCoins()) + " coins";
            sf::Text pText(info, font, 18);
            pText.setPosition(700, py);
            pText.setFillColor(sf::Color::White);
            window.draw(pText);
            py += 30;
        }

        window.display();
    }

    return 0;
}
