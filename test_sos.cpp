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

TEST_CASE("Simple game - Player 1 (Blue) wins", "[simple][sprint3]") {
    Game game(3, GameMode::SIMPLE);

    // Create SOS horizontally: S-O-S at row 0
    game.getCurrentPlayer()->setCurrentLetter('S');
    REQUIRE(game.makeMove(0, 0) == true);  // P1 places S

    game.getCurrentPlayer()->setCurrentLetter('O');
    REQUIRE(game.makeMove(0, 1) == true);  // P2 places O

    game.getCurrentPlayer()->setCurrentLetter('S');
    REQUIRE(game.makeMove(0, 2) == true);  // P1 places S and completes SOS

    // Player 1 should win immediately
    REQUIRE(game.getState() == GameState::PLAYER1_WIN);
    REQUIRE(game.getPlayer1()->getScore() == 1);
}

TEST_CASE("Simple game - Draw", "[simple][sprint3]") {
    Game game(3, GameMode::SIMPLE);

    // Fill board without creating SOS
    // Pattern that avoids SOS:
    // S O O
    // O S O
    // O O S

    game.getCurrentPlayer()->setCurrentLetter('S');
    game.makeMove(0, 0);  // P1: S

    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(0, 1);  // P2: O

    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(0, 2);  // P1: O

    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(1, 0);  // P2: O

    game.getCurrentPlayer()->setCurrentLetter('S');
    game.makeMove(1, 1);  // P1: S

    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(1, 2);  // P2: O

    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(2, 0);  // P1: O

    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(2, 1);  // P2: O

    game.getCurrentPlayer()->setCurrentLetter('S');
    game.makeMove(2, 2);  // P1: S

    // Board is full, no SOS created, should be draw
    REQUIRE(game.getState() == GameState::DRAW);
    REQUIRE(game.getPlayer1()->getScore() == 0);
    REQUIRE(game.getPlayer2()->getScore() == 0);
}


TEST_CASE("General game - Player 1 (Red) wins with most SOS", "[general][sprint3]") {
    Game game(4, GameMode::GENERAL);

    // Player 1 creates first SOS horizontally at row 0
    game.getCurrentPlayer()->setCurrentLetter('S');
    game.makeMove(0, 0);  // P1: S

    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(0, 1);  // P2: O

    game.getCurrentPlayer()->setCurrentLetter('S');
    game.makeMove(0, 2);  // P1: S (creates SOS, gets another turn)
    REQUIRE(game.getPlayer1()->getScore() == 1);

    // P1 still has turn, makes another move
    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(1, 0);  // P1: O

    // Continue filling board
    game.getCurrentPlayer()->setCurrentLetter('O');
    game.makeMove(1, 1);  // P2: O

    // Fill remaining cells without creating more SOS
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (game.getBoard().isEmpty(i, j)) {
                game.getCurrentPlayer()->setCurrentLetter('O');
                game.makeMove(i, j);
            }
        }
    }

    // P1 should win with score of 1, P2 with 0
    REQUIRE(game.getState() == GameState::PLAYER1_WIN);
    REQUIRE(game.getPlayer1()->getScore() >= 1);
}
