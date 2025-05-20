#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Game.hpp"
#include "Player.hpp"
#include "Role.hpp"

TEST_CASE("Basic test") {
    Game g;
    Player p("Alice", Role::Governor, &g);
    CHECK(p.getName() == "Alice");
    CHECK(p.getCoins() == 0);
}
