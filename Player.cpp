// Player.cpp – כולל Baron, Spy, General

#include "Player.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

Player::Player(const std::string& name, Role role, Game* game)
    : name(name), role(role), coins(0), alive(true), game(game), extraAction(false) {
    if (!game) throw std::invalid_argument("Player must be assigned to a game.");
    game->addPlayer(this);
}

const std::string& Player::getName() const { return name; }
Role Player::getRole() const { return role; }
int Player::getCoins() const { return coins; }
bool Player::isAlive() const { return alive; }

void Player::gather() {
    if (!alive) throw std::logic_error("Dead player cannot gather.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (game->isSanctioned(this)) throw std::logic_error("You are sanctioned and cannot gather.");
    coins += 1;
    std::cout << name << " gathered 1 coin." << std::endl;
    endTurn();
}

void Player::tax() {
    if (!alive) throw std::logic_error("Dead player cannot tax.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (game->isSanctioned(this)) {
        if (role == Role::Baron) {
            coins += 1;
            std::cout << name << " is sanctioned (Baron), receives 1 coin compensation." << std::endl;
            endTurn(); return;
        }
        throw std::logic_error("You are sanctioned and cannot tax.");
    }
    coins += (role == Role::Governor) ? 3 : 2;
    std::cout << name << " taxed and got coins." << std::endl;
    endTurn();
}

void Player::bribe() {
    if (!alive) throw std::logic_error("Dead player cannot bribe.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (coins < 4) throw std::logic_error("Not enough coins to bribe.");

    coins -= 4;
    extraAction = true;
    game->markBribe(this);  // חשוב!

    std::cout << name << " paid 4 coins to bribe and earned an extra action.\n";
}




void Player::sanction(Player& target) {
    if (!alive || !target.isAlive()) throw std::logic_error("Both players must be alive.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (coins < 3) throw std::logic_error("Not enough coins to sanction.");
    if (target.getRole() == Role::Judge) {
        game->addCoinsToBank(1);  // קנס נוסף
    }
    coins -= 3;
    game->applySanction(&target);
    std::cout << name << " sanctioned " << target.getName() << "." << std::endl;
    endTurn();
}

void Player::coup(Player& target) {
    if (!alive || !target.isAlive()) throw std::logic_error("Both players must be alive.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (game->isCoupBlocked(&target)) {
        coins -= 7;
        std::cout << name << " tried to coup " << target.getName() << " but it was blocked! Coins lost." << std::endl;
        endTurn(); return;
    }
    if (coins < 7) throw std::logic_error("Not enough coins to perform a coup.");
    coins -= 7;
    game->eliminate(&target);
    std::cout << name << " performed a coup on " << target.getName() << "." << std::endl;
    endTurn();
}

void Player::invest() {
    if (!alive) throw std::logic_error("Dead player cannot invest.");
    if (!game->isPlayerTurn(this)) throw std::logic_error("Not your turn.");
    if (role != Role::Baron) throw std::logic_error("Only a Baron can invest.");
    if (coins < 3) throw std::logic_error("Not enough coins to invest.");
    coins -= 3; coins += 6;
    std::cout << name << " invested and gained 6 coins." << std::endl;
    endTurn();
}

void Player::spyOn(Player& target) {
    if (!alive || !target.isAlive()) throw std::logic_error("Both players must be alive.");
    if (role != Role::Spy) throw std::logic_error("Only Spy can spy.");
    std::cout << name << " spies on " << target.getName() << ": " << target.getCoins() << " coins." << std::endl;
    game->blockArrest(&target);
    // Not ending turn
}

void Player::preventCoup(Player& target) {
    if (!alive || !target.isAlive()) throw std::logic_error("Both players must be alive.");
    if (role != Role::General) throw std::logic_error("Only General can prevent coup.");
    if (coins < 5) throw std::logic_error("Not enough coins to block coup.");
    coins -= 5;
    game->blockCoup(&target);
    std::cout << name << " (General) blocked coup against " << target.getName() << "." << std::endl;
    endTurn();
}

void Player::addCoins(int amount) { coins += amount; }
void Player::removeCoins(int amount) { if (coins < amount) throw std::logic_error("Not enough coins."); coins -= amount; }
void Player::eliminate() { alive = false; std::cout << name << " has been eliminated." << std::endl; }

void Player::endTurn() {
    if (extraAction) { extraAction = false; return; }
    game->nextTurn();
}

void Player::judgeBribe(Player& target) {
    if (!alive || !target.isAlive())
        throw std::logic_error("Both players must be alive.");
    if (role != Role::Judge)
        throw std::logic_error("Only Judge can cancel bribes.");
    if (!game->wasBribeUsedBy(&target))
        throw std::logic_error("No bribe to cancel.");

    // ביטול השוחד
    game->cancelBribe(&target);
    std::cout << name << " canceled bribe by " << target.getName() << std::endl;
}

void Player::clearExtraAction() {
    extraAction = false;
}
