#pragma once

#include <string>
#include "Role.hpp"
class Game;

class Player {
private:
    std::string name;
    Role role;
    int coins;
    bool alive;
    Game* game;
    bool extraAction;

public:
    Player(const std::string& name, Role role, Game* game);

    const std::string& getName() const;
    Role getRole() const;
    int getCoins() const;
    bool isAlive() const;

    void gather();
    void tax();
    void bribe();
    void coup(Player& target);
    void sanction(Player& target);
    void invest(); // 🟢 Baron only
    void spyOn(Player& target); // 🕵️ Spy only

    void addCoins(int amount);
    void removeCoins(int amount);
    void eliminate();
    void endTurn();
};
