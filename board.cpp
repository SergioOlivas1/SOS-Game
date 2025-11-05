#include "board.h"

Board::Board(int boardSize) : size(boardSize) {
    if (boardSize < 3) {
        size = 3; // Minimum valid size
    }
    grid.resize(size, std::vector<CellState>(size, CellState::EMPTY));
}

bool Board::isValidSize(int boardSize) const {
    return boardSize > 2;
}

int Board::getSize() const {
    return size;
}

CellState Board::getCell(int row, int col) const {
    if (row >= 0 && row < size && col >= 0 && col < size) {
        return grid[row][col];
    }
    return CellState::EMPTY;
}

bool Board::isEmpty(int row, int col) const {
    return getCell(row, col) == CellState::EMPTY;
}

bool Board::isFull() const {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == CellState::EMPTY) {
                return false;
            }
        }
    }
    return true;
}

bool Board::makeMove(int row, int col, char letter) {
    if (row < 0 || row >= size || col < 0 || col >= size) {
        return false;
    }

    if (!isEmpty(row, col)) {
        return false;
    }

    if (letter == 'S') {
        grid[row][col] = CellState::S;
    } else if (letter == 'O') {
        grid[row][col] = CellState::O;
    } else {
        return false;
    }

    return true;
}

int Board::checkForSOS(int row, int col) const {
    int count = 0;
    CellState placed = getCell(row, col);

    if (placed == CellState::S) {
        // When S is placed, check if it COMPLETES any S-O-S sequences
        // We only look BACKWARD to see if there's already S-O waiting for this S

        // 8 directions to check backward
        int directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
            {0, 1}, {1, -1}, {1, 0}, {1, 1}
        };

        for (int i = 0; i < 8; i++) {
            int oRow = row - directions[i][0];      // O should be here
            int oCol = col - directions[i][1];
            int sRow = row - 2*directions[i][0];    // Starting S should be here
            int sCol = col - 2*directions[i][1];

            if (sRow >= 0 && sRow < size && sCol >= 0 && sCol < size &&
                oRow >= 0 && oRow < size && oCol >= 0 && oCol < size) {
                if (getCell(sRow, sCol) == CellState::S &&
                    getCell(oRow, oCol) == CellState::O) {
                    count++;
                }
            }
        }
    } else if (placed == CellState::O) {
        // When O is placed, check if it's between two S's in ANY direction
        // We check 4 LINES (each line has 2 opposite directions)
        // This avoids double counting the same SOS

        int lines[4][2] = {
            {-1, 0},  // vertical line: up/down
            {0, 1},   // horizontal line: left/right
            {-1, 1},  // diagonal line: up-right/down-left ↗↙
            {1, 1}    // diagonal line: down-right/up-left ↘↖
        };

        for (int i = 0; i < 4; i++) {
            int s1Row = row + lines[i][0];
            int s1Col = col + lines[i][1];
            int s2Row = row - lines[i][0];
            int s2Col = col - lines[i][1];

            if (s1Row >= 0 && s1Row < size && s1Col >= 0 && s1Col < size &&
                s2Row >= 0 && s2Row < size && s2Col >= 0 && s2Col < size) {

                if (getCell(s1Row, s1Col) == CellState::S &&
                    getCell(s2Row, s2Col) == CellState::S) {
                    count++;
                }
            }
        }
    }

    return count;
}

// reset board
void Board::reset() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid[i][j] = CellState::EMPTY;
        }
    }
}
