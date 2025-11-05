#include "game.h"

Game::Game(int size, GameMode gameMode)
    : board(size), mode(gameMode), state(GameState::ONGOING), boardSize(size) {

    player1 = std::make_unique<Player>("Player 1", PlayerType::HUMAN);
    player2 = std::make_unique<Player>("Player 2", PlayerType::HUMAN);
    currentPlayer = player1.get();
}

void Game::setupPlayers(const std::string& p1Name, PlayerType p1Type,
                        const std::string& p2Name, PlayerType p2Type) {
    player1 = std::make_unique<Player>(p1Name, p1Type);
    player2 = std::make_unique<Player>(p2Name, p2Type);
    currentPlayer = player1.get();
}

bool Game::makeMove(int row, int col) {
    char letter = currentPlayer->getCurrentLetter();

    if (!board.makeMove(row, col, letter)) {
        return false;
    }

    // Check for SOS formations
    int sosCount = board.checkForSOS(row, col);

    if (sosCount > 0) {
        currentPlayer->addScore(sosCount);

        if (mode == GameMode::SIMPLE) {
            // In simple mode, first SOS wins immediately
            if (currentPlayer == player1.get()) {
                state = GameState::PLAYER1_WIN;
            } else {
                state = GameState::PLAYER2_WIN;
            }
            return true;
        }
        // In general mode, player gets another turn when they score
    } else {
        // No SOS made, switch players
        switchPlayer();
    }

    // Check if game should end
    checkGameEnd();
    return true;
}

void Game::switchPlayer() {
    if (currentPlayer == player1.get()) {
        currentPlayer = player2.get();
    } else {
        currentPlayer = player1.get();
    }
}

void Game::checkGameEnd() {
    if (board.isFull()) {
        if (mode == GameMode::GENERAL) {
            int p1Score = player1->getScore();
            int p2Score = player2->getScore();

            if (p1Score > p2Score) {
                state = GameState::PLAYER1_WIN;
            } else if (p2Score > p1Score) {
                state = GameState::PLAYER2_WIN;
            } else {
                state = GameState::DRAW;
            }
        } else {
            // Simple mode: if board is full and no one won yet, it's a draw
            if (state == GameState::ONGOING) {
                state = GameState::DRAW;
            }
        }
    }
}

Board& Game::getBoard() {
    return board;
}

const Board& Game::getBoard() const {
    return board;
}

Player* Game::getCurrentPlayer() const {
    return currentPlayer;
}

Player* Game::getPlayer1() const {
    return player1.get();
}

Player* Game::getPlayer2() const {
    return player2.get();
}

GameMode Game::getMode() const {
    return mode;
}

GameState Game::getState() const {
    return state;
}

void Game::reset() {
    board.reset();
    player1->resetScore();
    player2->resetScore();
    currentPlayer = player1.get();
    state = GameState::ONGOING;
}

void Game::newGame(int size, GameMode gameMode) {
    // validate board size
    if (size < 3) {
        size = 3;
    }

    boardSize = size;
    mode = gameMode;
    board = Board(size);
    player1->resetScore();
    player2->resetScore();
    currentPlayer = player1.get();
    state = GameState::ONGOING;
}
