// main_gui.cpp - כולל Governor + Spy + Baron + Judge + General + תיקון Must Coup

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Game.hpp"
#include "Player.hpp"
#include "Role.hpp"

int main() {
    Game game;
    std::vector<Player*> players;
    players.push_back(new Player("Orel", Role::Governor, &game));
    players.push_back(new Player("Avi", Role::Spy, &game));
    players.push_back(new Player("Alon", Role::General, &game));
    players.push_back(new Player("Avicii", Role::Judge, &game));

    sf::RenderWindow window(sf::VideoMode(1100, 650), "Coup Game - GUI");
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) return 1;

    sf::Text turnText("", font, 26); turnText.setPosition(50, 20); turnText.setFillColor(sf::Color::White);
    sf::Text resultText("", font, 22); resultText.setPosition(50, 70); resultText.setFillColor(sf::Color::Green);

    sf::RectangleShape gatherBtn({200, 50}), taxBtn({200, 50}), bribeBtn({200, 50}), coupBtn({200, 50});
    sf::RectangleShape sanctionBtn({200, 50}), investBtn({200, 50}), spyBtn({200, 50}), cancelBribeBtn({200, 50}), preventCoupBtn({200, 50});

    gatherBtn.setPosition(50, 120); gatherBtn.setFillColor(sf::Color(100, 100, 250));
    taxBtn.setPosition(50, 190); taxBtn.setFillColor(sf::Color(100, 150, 100));
    bribeBtn.setPosition(50, 260); bribeBtn.setFillColor(sf::Color(200, 100, 100));
    coupBtn.setPosition(50, 330); coupBtn.setFillColor(sf::Color(250, 150, 50));
    sanctionBtn.setPosition(50, 400); sanctionBtn.setFillColor(sf::Color(150, 0, 150));
    investBtn.setPosition(50, 470); investBtn.setFillColor(sf::Color(0, 180, 180));
    spyBtn.setPosition(50, 540); spyBtn.setFillColor(sf::Color(90, 90, 90));
    cancelBribeBtn.setPosition(300, 540); cancelBribeBtn.setFillColor(sf::Color(180, 0, 0));
    preventCoupBtn.setPosition(300, 470); preventCoupBtn.setFillColor(sf::Color(0, 100, 250));

    sf::Text gatherText("Gather", font, 24), taxText("Tax", font, 24), bribeText("Bribe", font, 24), coupText("Coup", font, 24);
    sf::Text sanctionText("Sanction", font, 24), investText("Invest (Baron)", font, 20), spyText("Spy (Spy only)", font, 20);
    sf::Text cancelBribeText("Cancel Bribe", font, 20), preventCoupText("Prevent Coup", font, 20);

    gatherText.setPosition(90, 130); taxText.setPosition(90, 200); bribeText.setPosition(90, 270); coupText.setPosition(90, 340);
    sanctionText.setPosition(80, 410); investText.setPosition(60, 480); spyText.setPosition(70, 550);
    cancelBribeText.setPosition(310, 550); preventCoupText.setPosition(310, 480);

    for (auto* t : {&gatherText, &taxText, &bribeText, &coupText, &sanctionText, &investText, &spyText, &cancelBribeText, &preventCoupText})
        t->setFillColor(sf::Color::White);

    std::vector<sf::RectangleShape> targetButtons;
    std::vector<sf::Text> targetTexts;
    bool choosingTarget = false, choosingSanction = false, choosingSpy = false, choosingCancelBribe = false, choosingPreventCoup = false;
    bool gameOver = false, mustCoup = false;
    std::string winnerName = "";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (gameOver) continue;

            Player* current = game.currentPlayer();
            mustCoup = current->getCoins() >= 10;
            sf::Vector2f mouse(sf::Mouse::getPosition(window));

            int before = current->getCoins();
            current->merchantBonus();
            if (current->getCoins() > before) {
                resultText.setString(current->getName() + " (Merchant) gained 1 bonus coin for starting with 3+.");
                resultText.setFillColor(sf::Color::Green);
            }

            if (mustCoup && !choosingTarget) {
                choosingTarget = true;
                targetButtons.clear(); targetTexts.clear();
                int y = 470;
                for (Player* p : players) {
                    if (p->isAlive() && p != current) {
                        sf::RectangleShape btn({200, 40}); btn.setPosition(300, y); btn.setFillColor(sf::Color(60, 60, 60));
                        sf::Text txt(p->getName(), font, 20); txt.setPosition(310, y + 5); txt.setFillColor(sf::Color::White);
                        targetButtons.push_back(btn); targetTexts.push_back(txt); y += 50;
                    }
                }
                resultText.setString("You have 10+ coins. Must coup!");
                resultText.setFillColor(sf::Color::Green);
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (choosingTarget || choosingSanction || choosingSpy || choosingCancelBribe || choosingPreventCoup) {
                    for (size_t i = 0; i < targetButtons.size(); ++i) {
                        if (targetButtons[i].getGlobalBounds().contains(mouse)) {
                            Player* target = game.getPlayer(targetTexts[i].getString());
                            try {
                                if (choosingTarget) current->coup(*target);
                                else if (choosingSanction) current->sanction(*target);
                                else if (choosingSpy) current->spyOn(*target);
                                else if (choosingCancelBribe) current->judgeBribe(*target);
                                else if (choosingPreventCoup) current->preventCoup(*target);
                                resultText.setString("Action successful.");
                                resultText.setFillColor(sf::Color::Green);
                            } catch (const std::exception& e) {
                                resultText.setString(e.what());
                                resultText.setFillColor(sf::Color::Red);
                            }
                            choosingTarget = choosingSanction = choosingSpy = choosingCancelBribe = choosingPreventCoup = false;
                            break;
                        }
                    }
                } else if (gatherBtn.getGlobalBounds().contains(mouse)) {
                    try { current->gather(); resultText.setString(current->getName() + " gathered 1 coin."); resultText.setFillColor(sf::Color::Green); }
                    catch (const std::exception& e) { resultText.setString(e.what()); resultText.setFillColor(sf::Color::Red); }
                } else if (taxBtn.getGlobalBounds().contains(mouse)) {
                    try { current->tax(); resultText.setString(current->getName() + " taxed."); resultText.setFillColor(sf::Color::Green); }
                    catch (const std::exception& e) { resultText.setString(e.what()); resultText.setFillColor(sf::Color::Red); }
                } else if (bribeBtn.getGlobalBounds().contains(mouse)) {
                    try { current->bribe(); resultText.setString(current->getName() + " bribed."); resultText.setFillColor(sf::Color::Green); }
                    catch (const std::exception& e) { resultText.setString(e.what()); resultText.setFillColor(sf::Color::Red); }
                } else if (sanctionBtn.getGlobalBounds().contains(mouse)) {
                    if (current->getCoins() >= 3) {
                        choosingSanction = true;
                        targetButtons.clear(); targetTexts.clear();
                        int y = 470;
                        for (Player* p : players) {
                            if (p->isAlive() && p != current) {
                                sf::RectangleShape btn({200, 40}); btn.setPosition(550, y); btn.setFillColor(sf::Color(120, 0, 120));
                                sf::Text txt(p->getName(), font, 20); txt.setPosition(560, y + 5); txt.setFillColor(sf::Color::White);
                                targetButtons.push_back(btn); targetTexts.push_back(txt); y += 50;
                            }
                        }
                        resultText.setString("Choose player to sanction");
                    } else {
                        resultText.setString("Need 3 coins to sanction.");
                        resultText.setFillColor(sf::Color::Red);
                    }
                } else if (spyBtn.getGlobalBounds().contains(mouse)) {
                    if (current->getRole() == Role::Spy) {
                        choosingSpy = true;
                        targetButtons.clear(); targetTexts.clear();
                        int y = 470;
                        for (Player* p : players) {
                            if (p->isAlive() && p != current) {
                                sf::RectangleShape btn({200, 40}); btn.setPosition(800, y); btn.setFillColor(sf::Color(80, 80, 80));
                                sf::Text txt(p->getName(), font, 20); txt.setPosition(810, y + 5); txt.setFillColor(sf::Color::White);
                                targetButtons.push_back(btn); targetTexts.push_back(txt); y += 50;
                            }
                        }
                        resultText.setString("Choose player to spy on");
                    } else {
                        resultText.setString("Only Spy can use this.");
                        resultText.setFillColor(sf::Color::Red);
                    }
                } else if (preventCoupBtn.getGlobalBounds().contains(mouse)) {
                    if (current->getRole() == Role::General) {
                        choosingPreventCoup = true;
                        targetButtons.clear(); targetTexts.clear();
                        int y = 470;
                        for (Player* p : players) {
                            if (p->isAlive() && (p == current || game.wasCoupTargeted(p))) {
                                sf::RectangleShape btn({200, 40}); btn.setPosition(550, y); btn.setFillColor(sf::Color(30, 30, 180));
                                sf::Text txt(p->getName(), font, 20); txt.setPosition(560, y + 5); txt.setFillColor(sf::Color::White);
                                targetButtons.push_back(btn); targetTexts.push_back(txt); y += 50;
                            }
                        }
                        resultText.setString("Choose player to protect from coup");
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));
        Player* current = nullptr;
        if (!gameOver) {
            try { winnerName = game.winner(); gameOver = true; }
            catch (...) { current = game.currentPlayer(); mustCoup = current->getCoins() >= 10; }
        }

        if (gameOver) {
            sf::Text winText("🏆 Winner: " + winnerName + " 🏆", font, 36);
            winText.setFillColor(sf::Color::Cyan);
            winText.setPosition(250, 250);
            window.draw(winText);
        } else {
            turnText.setString("Turn: " + current->getName() + " (" + std::to_string(current->getCoins()) + " coins)");
            window.draw(turnText);
            window.draw(resultText);

            if (!mustCoup && !choosingTarget && !choosingSanction && !choosingSpy && !choosingCancelBribe && !choosingPreventCoup) {
                window.draw(gatherBtn); window.draw(gatherText);
                window.draw(taxBtn); window.draw(taxText);
                window.draw(bribeBtn); window.draw(bribeText);
                window.draw(coupBtn); window.draw(coupText);
                window.draw(sanctionBtn); window.draw(sanctionText);
                if (current->getRole() == Role::Baron) { window.draw(investBtn); window.draw(investText); }
                if (current->getRole() == Role::Spy) { window.draw(spyBtn); window.draw(spyText); }
                if (current->getRole() == Role::Judge) { window.draw(cancelBribeBtn); window.draw(cancelBribeText); }
                if (current->getRole() == Role::General) { window.draw(preventCoupBtn); window.draw(preventCoupText); }
            }

            for (size_t i = 0; i < targetButtons.size(); ++i) {
                window.draw(targetButtons[i]);
                window.draw(targetTexts[i]);
            }

            sf::Text playerListTitle("Players:", font, 20);
            playerListTitle.setPosition(850, 30);
            playerListTitle.setFillColor(sf::Color::White);
            window.draw(playerListTitle);

            int py = 60;
            for (Player* p : players) {
                if (!p->isAlive()) continue;
                std::string info = p->getName() + " - " + std::to_string(p->getCoins()) + " coins";
                sf::Text pText(info, font, 18);
                pText.setPosition(850, py);
                pText.setFillColor(sf::Color::White);
                window.draw(pText); py += 30;
            }
        }

        window.display();
    }

    return 0;
}
