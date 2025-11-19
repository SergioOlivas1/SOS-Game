#ifndef GAME_H
#define GAME_H

#include <memory>
#include "enums.h"
#include "board.h"
#include "player.h"

class Game {
private:
    Board board;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    Player* currentPlayer;
    GameMode mode;
    GameState state;
    int boardSize;

public:
    Game(int size = 8, GameMode gameMode = GameMode::SIMPLE);

    void setupPlayers(const std::string& p1Name, PlayerType p1Type,
                      const std::string& p2Name, PlayerType p2Type);

    bool makeMove(int row, int col);
    bool makeComputerMove();
    bool makeComputerMove(int& outRow, int& outCol);
    void switchPlayer();
    void checkGameEnd();

    Board& getBoard();
    const Board& getBoard() const;
    Player* getCurrentPlayer() const;
    Player* getPlayer1() const;
    Player* getPlayer2() const;
    GameMode getMode() const;
    GameState getState() const;

    void reset();
    void newGame(int size, GameMode gameMode);
};

#endif
