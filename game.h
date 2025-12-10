#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>
#include <string>

#include "enums.h"
#include "board.h"
#include "player.h"

class Game {
public:
    struct MoveRecord {
        int moveNumber;
        std::string playerName;
        int row;
        int col;
        char letter;
    };

private:
    Board board;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    Player* currentPlayer;
    GameMode mode;
    GameState state;
    int boardSize;

    bool recording;
    std::vector<MoveRecord> recordedMoves;
    int moveCounter;
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

    void startRecording();
    void stopRecording();
    bool isRecording() const;
    std::vector<MoveRecord> getRecordedMoves() const;
    int getBoardSize() const;
};

#endif
