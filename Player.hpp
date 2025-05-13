#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include "Role.hpp"

class Game; // forward declaration

class Player {
private:
    std::string name;
    Role role;
    int coins;
    bool alive;
    Game* game;
    bool extraAction = false; 


public:
    Player(const std::string& name, Role role, Game* game);
    const std::string& getName() const;
    Role getRole() const;
    int getCoins() const;
    bool isAlive() const;

    void gather();
    void tax();
    void bribe();
    void arrest(Player& target);
    void sanction(Player& target);
    void coup(Player& target);

    void removeCoins(int amount);
    void addCoins(int amount);
    void eliminate();  // called when the player is removed from the game

    void spyOn(Player& target);  // Spy action

};

#endif
