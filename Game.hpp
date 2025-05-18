#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Player;

class Game {
private:
    std::vector<Player*> players;
    int currentTurnIndex = -1;
    std::unordered_map<Player*, int> sanctions;
    std::unordered_map<Player*, int> arrestBlocks; // 💥 Spy blocks arrest temporarily

public:
    void addPlayer(Player* p);
    Player* currentPlayer() const;
    std::string turn() const;
    std::vector<std::string> playersNames() const;
    void nextTurn();
    void eliminate(Player* p);
    std::string winner() const;
    bool isPlayerTurn(Player* p) const;

    void applySanction(Player* target);
    bool isSanctioned(Player* p) const;

    Player* getPlayer(const std::string& name);

    void blockArrest(Player* target);       // 🕵️ Spy sets arrest block
    bool isArrestBlocked(Player* target) const; // Called before arrest
};
