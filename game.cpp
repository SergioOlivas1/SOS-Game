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

    switchPlayer();

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
    // TODO
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
