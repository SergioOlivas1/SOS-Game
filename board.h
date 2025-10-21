#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "enums.h"

class Board {
private:
    int size;
    std::vector<std::vector<CellState>> grid;

    bool checkSOSAt(int row, int col, int dRow, int dCol) const;

public:
    Board(int boardSize = 8);

    // Validation: board size must be > 2
    bool isValidSize(int size) const;
    int getSize() const;
    CellState getCell(int row, int col) const;
    bool isEmpty(int row, int col) const;
    bool isFull() const;

    bool makeMove(int row, int col, char letter);
    int checkForSOS(int row, int col) const;
    void reset();
};

#endif // BOARD_H
