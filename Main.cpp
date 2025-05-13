#include <iostream>
#include "Game.hpp"
#include "Player.hpp"
#include "Role.hpp"

int main() {
    Game game;

    // Create players
    Player avichay("Avichay", Role::Governor, &game);
    Player hadar("Hadar", Role::Spy, &game);
    Player dani("Dani", Role::Baron, &game);

    std::cout << "Starting players:\n";
    for (const auto& name : game.playersNames()) {
        std::cout << "- " << name << "\n";
    }

    std::cout << "\n--- Game Start ---\n\n";

    // Turn 1: Avichay (Governor) uses tax → gets 3 coins
    std::cout << "Turn: " << game.turn() << std::endl;
    avichay.tax();

    // Turn 2: Hadar does gather
    std::cout << "Turn: " << game.turn() << std::endl;
    hadar.gather();

    // Turn 3: Dani does gather
    std::cout << "Turn: " << game.turn() << std::endl;
    dani.gather();

    // Turn 4: Avichay uses bribe → earns another move this turn
    std::cout << "Turn: " << game.turn() << std::endl;
    avichay.addCoins(8);  // Add coins manually so he can afford bribe + coup
    avichay.bribe();      // pays 4 coins → stays in turn
    avichay.coup(hadar);  // pays 7 coins → eliminates Hadar

    std::cout << "Hadar is " << (hadar.isAlive() ? "still in the game" : "eliminated") << ".\n";

    // Turn 5: Dani's turn
    std::cout << "Turn: " << game.turn() << std::endl;
    dani.gather();

    // Print active players
    std::cout << "\nRemaining players:\n";
    for (const auto& name : game.playersNames()) {
        std::cout << "- " << name << "\n";
    }

    return 0;
}
