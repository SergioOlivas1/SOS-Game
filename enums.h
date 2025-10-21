#ifndef ENUMS_H
#define ENUMS_H

enum class CellState {
    EMPTY,
    S,
    O
};

enum class PlayerType {
    HUMAN,
    AI
};

enum class GameMode {
    SIMPLE,  // First to make SOS wins
    GENERAL  // Most SOS sequences wins
};

enum class GameState {
    ONGOING,
    PLAYER1_WIN,
    PLAYER2_WIN,
    DRAW
};

#endif
