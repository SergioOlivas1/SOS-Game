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


// Computer tests

TEST_CASE("Computer player type is set correctly", "[computer][sprint4]") {
    Game game(5, GameMode::SIMPLE);

    SECTION("Set both players as AI") {
        game.setupPlayers("AI 1", PlayerType::AI, "AI 2", PlayerType::AI);
        REQUIRE(game.getPlayer1()->getType() == PlayerType::AI);
        REQUIRE(game.getPlayer2()->getType() == PlayerType::AI);
    }

    SECTION("Set mixed player types") {
        game.setupPlayers("Human", PlayerType::HUMAN, "AI", PlayerType::AI);
        REQUIRE(game.getPlayer1()->getType() == PlayerType::HUMAN);
        REQUIRE(game.getPlayer2()->getType() == PlayerType::AI);
    }
}

TEST_CASE("Computer makes valid moves", "[computer][sprint4]") {
    Game game(5, GameMode::SIMPLE);
    game.setupPlayers("AI", PlayerType::AI, "Human", PlayerType::HUMAN);

    SECTION("Computer move is on empty cell") {
        int row, col;
        REQUIRE(game.makeComputerMove(row, col) == true);

        // Verify move was made
        REQUIRE(game.getBoard().getCell(row, col) != CellState::EMPTY);
    }

    SECTION("Computer move is within bounds") {
        int row, col;
        game.makeComputerMove(row, col);

        REQUIRE(row >= 0);
        REQUIRE(row < game.getBoard().getSize());
        REQUIRE(col >= 0);
        REQUIRE(col < game.getBoard().getSize());
    }

    SECTION("Computer cannot move on full board") {
        // Fill the board
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                game.getBoard().makeMove(i, j, 'S');
            }
        }

        int row, col;
        REQUIRE(game.makeComputerMove(row, col) == false);
    }
}

TEST_CASE("Computer makes multiple valid moves", "[computer][sprint4]") {
    Game game(5, GameMode::GENERAL);
    game.setupPlayers("AI 1", PlayerType::AI, "AI 2", PlayerType::AI);

    SECTION("Computer can make 5 consecutive moves") {
        int moveCount = 0;
        for (int i = 0; i < 5; i++) {
            int row, col;
            if (game.makeComputerMove(row, col)) {
                moveCount++;
            }
        }

        REQUIRE(moveCount == 5);
    }
}

TEST_CASE("Computer vs Computer game completes", "[computer][sprint4]") {
    Game game(3, GameMode::SIMPLE);
    game.setupPlayers("AI 1", PlayerType::AI, "AI 2", PlayerType::AI);

    // Play until game ends
    int maxMoves = 100;  // Safety limit
    int moves = 0;

    while (game.getState() == GameState::ONGOING && moves < maxMoves) {
        int row, col;
        game.makeComputerMove(row, col);
        moves++;
    }

    // Game should have ended
    REQUIRE(game.getState() != GameState::ONGOING);
    REQUIRE(moves < maxMoves);  // Shouldn't hit safety limit
}

TEST_CASE("Recording can be started and stopped", "[recording][sprint5]") {
    Game game(5, GameMode::SIMPLE);

    SECTION("Recording starts disabled") {
        REQUIRE(game.isRecording() == false);
    }

    SECTION("Can start recording") {
        game.startRecording();
        REQUIRE(game.isRecording() == true);
    }

    SECTION("Can stop recording") {
        game.startRecording();
        game.stopRecording();
        REQUIRE(game.isRecording() == false);
    }
}

TEST_CASE("Moves are recorded when recording is active", "[recording][sprint5]") {
    Game game(5, GameMode::SIMPLE);
    game.startRecording();

    SECTION("First move is recorded") {
        game.getCurrentPlayer()->setCurrentLetter('S');
        game.makeMove(0, 0);

        auto moves = game.getRecordedMoves();
        REQUIRE(moves.size() == 1);
        REQUIRE(moves[0].row == 0);
        REQUIRE(moves[0].col == 0);
        REQUIRE(moves[0].letter == 'S');
    }

    SECTION("Multiple moves are recorded") {
        game.getCurrentPlayer()->setCurrentLetter('S');
        game.makeMove(0, 0);

        game.getCurrentPlayer()->setCurrentLetter('O');
        game.makeMove(0, 1);

        game.getCurrentPlayer()->setCurrentLetter('S');
        game.makeMove(0, 2);

        auto moves = game.getRecordedMoves();
        REQUIRE(moves.size() == 3);
    }

    SECTION("Move numbers are sequential") {
        game.getCurrentPlayer()->setCurrentLetter('S');
        game.makeMove(0, 0);
        game.getCurrentPlayer()->setCurrentLetter('O');
        game.makeMove(0, 1);

        auto moves = game.getRecordedMoves();
        REQUIRE(moves[0].moveNumber == 1);
        REQUIRE(moves[1].moveNumber == 2);
    }
}

TEST_CASE("Moves are not recorded when recording is off", "[recording][sprint5]") {
    Game game(5, GameMode::SIMPLE);

    SECTION("Moves before recording not saved") {
        game.getCurrentPlayer()->setCurrentLetter('S');
        game.makeMove(0, 0);

        auto moves = game.getRecordedMoves();
        REQUIRE(moves.size() == 0);
    }

    SECTION("Moves after stopping not saved") {
        game.startRecording();
        game.getCurrentPlayer()->setCurrentLetter('S');
        game.makeMove(0, 0);

        game.stopRecording();

        game.getCurrentPlayer()->setCurrentLetter('O');
        game.makeMove(0, 1);

        auto moves = game.getRecordedMoves();
        REQUIRE(moves.size() == 1);  // Only first move recorded
    }
}
