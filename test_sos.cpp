#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "board.h"
#include "game.h"
#include "player.h"

// Tests

TEST_CASE("Board size validation", "[board]") {
    SECTION("Valid board sizes") {
        Board board3(3);
        REQUIRE(board3.getSize() == 3);

        Board board5(5);
        REQUIRE(board5.getSize() == 5);

        Board board10(10);
        REQUIRE(board10.getSize() == 10);
    }

    SECTION("Invalid board size - too small") {
        Board board1(1);
        REQUIRE(board1.getSize() == 3); // Should default to minimum

        Board board2(2);
        REQUIRE(board2.getSize() == 3); // Should default to minimum
    }

    SECTION("Board size validation method") {
        Board board(5);
        REQUIRE(board.isValidSize(3) == true);
        REQUIRE(board.isValidSize(5) == true);
        REQUIRE(board.isValidSize(10) == true);
        REQUIRE(board.isValidSize(2) == false);
        REQUIRE(board.isValidSize(1) == false);
        REQUIRE(board.isValidSize(0) == false);
    }
}

TEST_CASE("Game mode selection", "[game]") {
    SECTION("Simple mode") {
        Game game(5, GameMode::SIMPLE);
        REQUIRE(game.getMode() == GameMode::SIMPLE);
    }

    SECTION("General mode") {
        Game game(5, GameMode::GENERAL);
        REQUIRE(game.getMode() == GameMode::GENERAL);
    }

    SECTION("New game changes mode") {
        Game game(5, GameMode::SIMPLE);
        REQUIRE(game.getMode() == GameMode::SIMPLE);

        game.newGame(6, GameMode::GENERAL);
        REQUIRE(game.getMode() == GameMode::GENERAL);
        REQUIRE(game.getBoard().getSize() == 6);
    }
}

