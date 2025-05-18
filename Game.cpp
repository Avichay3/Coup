// Game.cpp – כולל חסימות arrest + coup (Spy + General)

#include "Game.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>

void Game::addPlayer(Player* p) {
    if (!p) throw std::invalid_argument("Null player");
    players.push_back(p);
    if (currentTurnIndex == -1) currentTurnIndex = 0;
}

Player* Game::currentPlayer() const {
    if (players.empty()) throw std::logic_error("No players in game.");
    int count = 0;
    int idx = currentTurnIndex;
    while (!players[idx]->isAlive()) {
        idx = (idx + 1) % players.size();
        if (++count > players.size()) throw std::logic_error("No active players.");
    }
    return players[idx];
}

std::string Game::turn() const {
    return currentPlayer()->getName();
}

std::vector<std::string> Game::playersNames() const {
    std::vector<std::string> names;
    for (auto* p : players) {
        if (p->isAlive()) names.push_back(p->getName());
    }
    return names;
}

void Game::nextTurn() {
    if (players.empty()) return;
    do {
        currentTurnIndex = (currentTurnIndex + 1) % players.size();
    } while (!players[currentTurnIndex]->isAlive());

    // ניקוי סנקציות
    for (auto it = sanctions.begin(); it != sanctions.end(); ) {
        if (it->first == players[currentTurnIndex]) {
            it = sanctions.erase(it);
        } else {
            ++it;
        }
    }
    // ניקוי חסימות arrest
    for (auto it = arrestBlocks.begin(); it != arrestBlocks.end(); ) {
        if (it->first == players[currentTurnIndex]) {
            std::cout << "[DEBUG] Removing arrest block from: " << it->first->getName() << std::endl;
            it = arrestBlocks.erase(it);
        } else {
            ++it;
        }
    }
    // ניקוי חסימות coup
    for (auto it = coupBlocks.begin(); it != coupBlocks.end(); ) {
        if (it->first == players[currentTurnIndex]) {
            std::cout << "[DEBUG] Removing coup block from: " << it->first->getName() << std::endl;
            it = coupBlocks.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::eliminate(Player* p) {
    if (!p || !p->isAlive()) throw std::logic_error("Cannot eliminate.");
    p->eliminate();
}

std::string Game::winner() const {
    std::string winnerName = "";
    for (Player* p : players) {
        if (p->isAlive()) {
            if (!winnerName.empty()) throw std::logic_error("Game is not over yet.");
            winnerName = p->getName();
        }
    }
    if (winnerName.empty()) throw std::logic_error("No winner.");
    return winnerName;
}

bool Game::isPlayerTurn(Player* p) const {
    return currentPlayer() == p;
}

void Game::applySanction(Player* target) {
    if (!target || !target->isAlive()) throw std::logic_error("Invalid sanction target");
    sanctions[target] = currentTurnIndex;
}

bool Game::isSanctioned(Player* p) const {
    return sanctions.find(p) != sanctions.end();
}

Player* Game::getPlayer(const std::string& name) {
    for (Player* p : players) {
        if (p->getName() == name) return p;
    }
    throw std::logic_error("Player not found: " + name);
}

void Game::blockArrest(Player* target) {
    arrestBlocks[target] = currentTurnIndex;
    std::cout << "[DEBUG] Blocking arrest for: " << target->getName() << " at turn index: " << currentTurnIndex << std::endl;
}

bool Game::isArrestBlocked(Player* target) const {
    auto it = arrestBlocks.find(target);
    if (it == arrestBlocks.end()) return false;
    bool blocked = it->second == currentTurnIndex;
    std::cout << "[DEBUG] Checking if " << target->getName() << " is blocked from arrest (blocked? " << blocked << ")" << std::endl;
    return blocked;
}

void Game::blockCoup(Player* target) {
    coupBlocks[target] = currentTurnIndex;
    std::cout << "[DEBUG] Blocking coup for: " << target->getName() << " at turn index: " << currentTurnIndex << std::endl;
}

bool Game::isCoupBlocked(Player* target) const {
    auto it = coupBlocks.find(target);
    if (it == coupBlocks.end()) return false;
    bool blocked = it->second == currentTurnIndex;
    std::cout << "[DEBUG] Checking if " << target->getName() << " is blocked from coup (blocked? " << blocked << ")" << std::endl;
    return blocked;
}

void Game::markBribe(Player* p) {
    bribeLog[p] = currentTurnIndex;
}

bool Game::wasBribeUsedBy(Player* p) const {
    auto it = bribeLog.find(p);
    if (it == bribeLog.end()) return false;
    return (it->second + 1 >= currentTurnIndex); // ניתן לבטל עד תחילת תור הבא
}

void Game::cancelBribe(Player* p) {
    bribeLog.erase(p);
    p->clearExtraAction();  // לא יקבל תור נוסף
}

void Game::addCoinsToBank(int amount) {
    bank += amount;
}