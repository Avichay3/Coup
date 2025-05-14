#include "Player.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

Player::Player(const std::string& name, Role role, Game* game)
    : name(name), role(role), coins(0), alive(true), game(game) {
    if (!game) {
        throw std::invalid_argument("Player must be assigned to a game.");
    }
    game->addPlayer(this);
}

const std::string& Player::getName() const {
    return name;
}

Role Player::getRole() const {
    return role;
}

int Player::getCoins() const {
    return coins;
}

bool Player::isAlive() const {
    return alive;
}

void Player::gather() {
    if (!alive) throw std::logic_error("Dead player cannot gather.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (game->isSanctionedPlayer(this)) throw std::logic_error("You are sanctioned and cannot gather.");

    coins += 1;
    std::cout << name << " gathered 1 coin." << std::endl;

    if (extraAction) {
        extraAction = false;
    }
    game->nextTurn();
}

void Player::tax() {
    if (!alive) throw std::logic_error("Dead player cannot tax.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (game->isSanctionedPlayer(this)) throw std::logic_error("You are sanctioned and cannot tax.");

    int amount = 2;
    if (role == Role::Governor) amount = 3;

    coins += amount;
    std::cout << name << " taxed and got " << amount << " coins." << std::endl;

    if (extraAction) {
        extraAction = false;
    }
    game->nextTurn();
}

void Player::bribe() {
    if (!alive) throw std::logic_error("Dead player cannot bribe.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (coins < 4) throw std::logic_error("Not enough coins to bribe.");

    coins -= 4;
    extraAction = true;

    std::cout << name << " paid 4 coins to bribe and earned an extra action.\n";
    // No nextTurn() here – player acts again
}

void Player::arrest(Player& target) {
    if (!alive || !target.isAlive()) throw std::logic_error("Both players must be alive.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (game->wasArrestedByMeLastTurn(this, &target)) throw std::logic_error("Cannot arrest same player twice in a row.");
    if (game->isArrestBlocked(this)) throw std::logic_error("You have been blocked from using arrest this turn.");

    if (target.getCoins() > 0) {
        target.removeCoins(1);
        this->addCoins(1);
    }

    game->markArrest(this, &target);
    std::cout << name << " arrested " << target.getName() << " and took 1 coin." << std::endl;

    if (extraAction) {
        extraAction = false;
    }
    game->nextTurn();
}




void Player::sanction(Player& target) {
    if (!alive || !target.isAlive()) throw std::logic_error("Both players must be alive.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (coins < 3) throw std::logic_error("Not enough coins for sanction.");

    coins -= 3;
    game->applySanction(&target);

    std::cout << name << " sanctioned " << target.getName() << "." << std::endl;

    if (extraAction) {
        extraAction = false;
    }
    game->nextTurn();
}

void Player::coup(Player& target) {
    if (!alive || !target.isAlive()) throw std::logic_error("Both players must be alive.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (coins < 7) throw std::logic_error("Not enough coins to perform a coup.");

    coins -= 7;
    game->eliminate(&target);

    std::cout << name << " performed a coup on " << target.getName() << "." << std::endl;

    if (extraAction) {
        extraAction = false;
    }
    game->nextTurn();
}

void Player::addCoins(int amount) {
    coins += amount;
}

void Player::removeCoins(int amount) {
    if (coins < amount) {
        throw std::logic_error("Not enough coins.");
    }
    coins -= amount;
}

void Player::eliminate() {
    alive = false;
    std::cout << name << " has been eliminated from the game." << std::endl;
}

void Player::spyOn(Player& target) {
    if (!alive) throw std::logic_error("Dead player cannot spy.");
    if (role != Role::Spy) throw std::logic_error("Only a Spy can use spyOn.");
    if (!target.isAlive()) throw std::logic_error("Cannot spy on a dead player.");

    std::cout << name << " spies on " << target.getName()
              << ": they have " << target.getCoins() << " coins.\n";

    game->blockArrestFor(&target);
}



