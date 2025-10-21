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

bool Board::checkSOSAt(int row, int col, int dRow, int dCol) const {
    return false;
}

int Board::checkForSOS(int row, int col) const {
    return 0;
}

// reset board
void Board::reset() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid[i][j] = CellState::EMPTY;
        }
    }
}
