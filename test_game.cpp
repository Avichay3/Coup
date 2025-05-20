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

    CHECK(g.turn() == "Bob");
    p1.gather();
    CHECK(p1.getCoins() == 1);
    
    CHECK(g.turn() == "Dan");
    p2.gather();  // Should get merchant bonus if coins >= 3
    CHECK(p2.getCoins() == 1);

    p2.gather(); p2.gather(); // Now has 3 coins
    p2.gather(); // Will get +1 from merchant bonus
    CHECK(p2.getCoins() == 5);
}

TEST_CASE("Tax - Governor vs Normal") {
    Game g;
    Player gov("Gov", Role::Governor, &g);
    Player mer("Mer", Role::Merchant, &g);

    gov.tax(); // +3
    CHECK(gov.getCoins() == 3);

    mer.tax(); // +2 + maybe bonus
    CHECK(mer.getCoins() >= 2);
}

TEST_CASE("Bribe and Extra Action") {
    Game g;
    Player p1("Briber", Role::Merchant, &g);
    Player p2("Receiver", Role::Spy, &g);

    p1.gather(); p2.gather(); // round 1
    p1.tax(); p2.tax();       // round 2
    p1.tax(); p2.tax();       // round 3, p1 has 6+ coins
    p1.gather();              // 7 coins
    p1.bribe();
    CHECK(p1.getCoins() == 3);  // paid 4 coins
}

TEST_CASE("Coup Mechanics") {
    Game g;
    Player p1("CoupMan", Role::Baron, &g);
    Player p2("Target", Role::Spy, &g);

    for (int i = 0; i < 10; ++i) p1.gather();
    CHECK(p1.getCoins() >= 10);

    p1.coup(p2);
    CHECK(p2.isAlive() == false);
}

TEST_CASE("Invest - Baron only") {
    Game g;
    Player baron("Baron", Role::Baron, &g);
    Player other("Other", Role::Merchant, &g);

    for (int i = 0; i < 3; ++i) baron.gather();
    baron.invest();
    CHECK(baron.getCoins() == 6);

    other.gather(); other.gather(); other.gather();
    CHECK_THROWS(other.invest());
}

TEST_CASE("Sanction & Block") {
    Game g;
    Player gov("Gov", Role::Governor, &g);
    Player mer("Mer", Role::Merchant, &g);

    for (int i = 0; i < 3; ++i) gov.gather();
    gov.sanction(mer);

    CHECK_THROWS(mer.gather());
    CHECK_THROWS(mer.tax());
}

TEST_CASE("Spy blocks arrest") {
    Game g;
    Player spy("SpyGuy", Role::Spy, &g);
    Player gov("GovGuy", Role::Governor, &g);

    spy.gather(); gov.gather();
    spy.spyOn(gov); // blocks arrest
    gov.gather(); spy.gather();
    CHECK_THROWS(gov.arrest(spy));
}

TEST_CASE("Judge cancels bribe") {
    Game g;
    Player briber("Briber", Role::Baron, &g);
    Player judge("Judgey", Role::Judge, &g);

    for (int i = 0; i < 4; ++i) briber.gather();
    briber.bribe(); // extra action = true

    CHECK_NOTHROW(judge.judgeBribe(briber));
}