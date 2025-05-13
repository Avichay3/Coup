#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "Role.hpp"

class Player;

class Game {
private:
    std::vector<Player*> players;
    std::unordered_map<std::string, bool> isSanctioned;
    std::unordered_map<std::string, std::string> lastArrested;
    size_t turnIndex = 0;
    bool gameOver = false;

public:
    Game();

    void addPlayer(Player* player);
    void eliminate(Player* player);
    void applySanction(Player* target);
    void markArrest(Player* source, Player* target);
    bool wasArrestedByMeLastTurn(Player* source, Player* target) const;

    std::string turn() const;
    std::vector<std::string> playersNames() const;
    std::string winner() const;
    void nextTurn();
    bool isPlayerTurn(Player* player) const;
    bool isSanctionedPlayer(Player* player) const;
};

#endif
