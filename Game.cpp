#include "Game.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <iostream>
#include <unordered_set>

Game::Game() {}

void Game::addPlayer(Player* player) {
    if (gameOver) throw std::logic_error("Cannot add players after game started.");
    players.push_back(player);
    isSanctioned[player->getName()] = false;
}

void Game::eliminate(Player* player) {
    player->eliminate();

    int aliveCount = 0;
    for (Player* p : players) {
        if (p->isAlive()) aliveCount++;
    }
    if (aliveCount == 1) gameOver = true;
}

void Game::applySanction(Player* target) {
    isSanctioned[target->getName()] = true;
}

void Game::markArrest(Player* source, Player* target) {
    lastArrested[source->getName()] = target->getName();
}

bool Game::wasArrestedByMeLastTurn(Player* source, Player* target) const {
    auto it = lastArrested.find(source->getName());
    return it != lastArrested.end() && it->second == target->getName();
}

std::string Game::turn() const {
    if (players.empty()) throw std::logic_error("No players in game.");
    if (!players[turnIndex]->isAlive()) throw std::logic_error("Dead player's turn.");
    return players[turnIndex]->getName();
}

std::vector<std::string> Game::playersNames() const {
    std::vector<std::string> active;
    for (Player* p : players) {
        if (p->isAlive()) active.push_back(p->getName());
    }
    return active;
}

std::string Game::winner() const {
    if (!gameOver) throw std::logic_error("Game is not over yet.");
    for (Player* p : players) {
        if (p->isAlive()) return p->getName();
    }
    throw std::logic_error("No winner found.");
}

void Game::nextTurn() {
    if (players.empty()) return;

    size_t total = players.size();
    for (size_t i = 1; i <= total; ++i) {
        size_t next = (turnIndex + i) % total;
        if (players[next]->isAlive()) {
            turnIndex = next;

            // Clear sanctions (valid until start of next turn)
            isSanctioned[players[next]->getName()] = false;

            // Clear arrest blocks (Spy effects)
            std::cout << "[DEBUG] Clearing arrest block list\n";
            arrestBlockedPlayers.clear();

            break;
        }
    }
}

bool Game::isPlayerTurn(Player* player) const {
    return players[turnIndex] == player;
}

bool Game::isSanctionedPlayer(Player* player) const {
    auto it = isSanctioned.find(player->getName());
    return it != isSanctioned.end() && it->second;
}

void Game::blockArrestFor(Player* p) {
    std::cout << "[DEBUG] Blocking arrest for: " << p->getName() << std::endl;
    arrestBlockedPlayers.insert(p->getName());
}

bool Game::isArrestBlocked(Player* p) const {
    std::cout << "[DEBUG] Checking if " << p->getName() << " is blocked from arrest.\n";
    return arrestBlockedPlayers.count(p->getName()) > 0;
}
