#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Game.hpp"
#include "Player.hpp"
#include "Role.hpp"

TEST_CASE("Player Initialization and Basic Info") {
    Game g;
    Player p("Alice", Role::Governor, &g);
    CHECK(p.getName() == "Alice");
    CHECK(p.getCoins() == 0);
    CHECK(p.isAlive() == true);
    CHECK(p.getRole() == Role::Governor);
}

TEST_CASE("Gather and Tax") {
    Game g;
    Player p1("Bob", Role::Governor, &g);
    Player p2("Dan", Role::Merchant, &g);

    for (int i = 0; i < 5; ++i) {
        p1.gather();
        p2.gather();
    }

    CHECK(p1.getCoins() == 5);
    CHECK(p2.getCoins() >= 5);
}

TEST_CASE("Tax - Governor vs Normal") {
    Game g;
    Player gov("Gov", Role::Governor, &g);
    Player mer("Mer", Role::Merchant, &g);

    gov.tax(); CHECK(gov.getCoins() == 3);
    mer.tax(); CHECK(mer.getCoins() >= 2);
}

TEST_CASE("Bribe and Extra Action") {
    Game g;
    Player p1("Briber", Role::Merchant, &g);
    Player p2("Receiver", Role::Spy, &g);

    for (int i = 0; i < 3; ++i) {
        p1.tax(); p2.tax();
    }
    p1.gather(); p2.gather(); // total 9 coins
    p1.gather(); // will gain bonus

    CHECK(p1.getCoins() >= 10);
    p1.bribe();
    CHECK(p1.getCoins() == 6);  // 10 - 4
}

TEST_CASE("Coup Mechanics") {
    Game g;
    Player p1("CoupMan", Role::Baron, &g);
    Player p2("Target", Role::Spy, &g);

    for (int i = 0; i < 10; ++i) {
        p1.gather(); p2.gather();
    }

    CHECK(p1.getCoins() >= 10);
    p1.coup(p2);
    CHECK_FALSE(p2.isAlive());
}

TEST_CASE("Invest - Baron only") {
    Game g;
    Player baron("Baron", Role::Baron, &g);
    Player other("Other", Role::Merchant, &g);

    for (int i = 0; i < 3; ++i) {
        baron.gather(); other.gather();
    }

    baron.invest();
    CHECK(baron.getCoins() == 6);

    CHECK_THROWS(other.invest());
}

TEST_CASE("Sanction & Block") {
    Game g;
    Player gov("Gov", Role::Governor, &g);
    Player mer("Mer", Role::Merchant, &g);

    for (int i = 0; i < 3; ++i) { gov.gather(); mer.gather(); }

    gov.sanction(mer);
    CHECK_THROWS(mer.gather());  // must call immediately after
}

TEST_CASE("Spy blocks arrest") {
    Game g;
    Player spy("SpyGuy", Role::Spy, &g);
    Player gov("GovGuy", Role::Governor, &g);

    spy.gather();
    gov.gather();
    spy.spyOn(gov);
    spy.endTurn();  // spyOn doesn't end turn

    CHECK_THROWS(gov.arrest(spy));
}

TEST_CASE("Judge cancels bribe") {
    Game g;
    Player briber("Briber", Role::Baron, &g);
    Player judge("Judgey", Role::Judge, &g);

    for (int i = 0; i < 4; ++i) { briber.gather(); judge.gather(); }

    briber.bribe();
    CHECK_NOTHROW(judge.judgeBribe(briber));
}

TEST_CASE("Can't arrest same player twice in a row") {
    Game g;
    Player p1("A", Role::Governor, &g);
    Player p2("B", Role::Merchant, &g);

    for (int i = 0; i < 3; ++i) { p1.gather(); p2.gather(); }

    p1.arrest(p2);
    p2.gather();
    CHECK_THROWS(p1.arrest(p2));
}

TEST_CASE("Judge can't cancel bribe if none made") {
    Game g;
    Player p1("Briber", Role::Baron, &g);
    Player p2("Judgey", Role::Judge, &g);

    p1.gather(); p2.gather();
    CHECK_THROWS(p2.judgeBribe(p1));
}

TEST_CASE("General blocks coup successfully") {
    Game g;
    Player attacker("Attacker", Role::Governor, &g);
    Player general("Gen", Role::General, &g);

    for (int i = 0; i < 10; ++i) {
        attacker.gather(); general.gather();
    }

    general.preventCoup(general);  // block first
    attacker.coup(general);        // will be blocked

    CHECK(general.isAlive());
}

TEST_CASE("Player cannot coup themselves") {
    Game g;
    Player p1("Selfie", Role::Governor, &g);
    for (int i = 0; i < 10; ++i) p1.gather();

    CHECK_THROWS(p1.coup(p1));
}
