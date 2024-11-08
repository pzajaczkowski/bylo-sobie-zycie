#include "../include/board.hpp"

// Constructors

Board::Board(const int width, const int height)
    : width(width), height(height), board(new Cell[width * height]{}) {}

Board::~Board() { delete[] board; }

// Accessors

Cell* Board::getRow(const int y) const { return &board[y * width]; }

int Board::getWidth() const { return width; }

int Board::getHeight() const { return height; }

Cell* Board::getBoard() const { return board; }

// Mutators

void Board::setCell(const int x, const int y, const Cell value) {
    board[y * width + x] = value;
}

void Board::Init(const BoardInitType type) {
    switch (type) {
        case LINE:
            for (int i = 0; i < height; ++i) {
                setCell(width / 2, i, ALIVE);
            }
            break;
        case T_SHAPE:
            for (int i = 0; i < height; ++i) {
                setCell(width / 2, i, ALIVE);
            }
            for (int i = 0; i < width; ++i) {
                setCell(i, 0, ALIVE);
            }
            break;
        case CROSS:
            for (int i = 0; i < height; ++i) {
                setCell(i, height / 2, ALIVE);
            }
            for (int i = 0; i < width; ++i) {
                setCell(height / 2, i, ALIVE);
            }
            break;
    }
}

void Board::updateRow(
    const Cell* prevRow,
    const Cell* currRow,
    const Cell* nextRow,
    Cell* newRow
) const {
    for (int column = 0; column < width; ++column) {
        int neighbors = 0;

        // Count neighbors
        for (int offset = 0; offset < 3; ++offset) {
            const int eval_column = column - 1 + offset;
            // Skip edge cases
            if (eval_column < 0 || eval_column >= width) {
                continue;
            }
            // Upper
            if (prevRow && prevRow[eval_column] == ALIVE) {
                neighbors++;
            }
            // Middle
            if (eval_column != column && currRow[eval_column] == ALIVE) {
                neighbors++;
            }
            // Lower
            if (nextRow && nextRow[eval_column] == ALIVE) {
                neighbors++;
            }
        }

        if (currRow[column] == ALIVE) {
            newRow[column] = (neighbors == 2 || neighbors == 3) ? ALIVE : DEAD;
        } else {
            newRow[column] = (neighbors == 3) ? ALIVE : DEAD;
        }
    }
}

void Board::updateBoard(const Cell* upperGhostRow, const Cell* lowerGhostRow) {
    Cell* newBoard = new Cell[width * height]{};

    // first row
    updateRow(
        upperGhostRow,
        &board[0 * width],
        &board[1 * width],
        &newBoard[0]
    );

    // middle rows
    for (int i = 1; i < height - 1; ++i) {
        updateRow(
            &board[(i - 1) * width],
            &board[i * width],
            &board[(i + 1) * width],
            &newBoard[i * width]
        );
    }

    // last row
    updateRow(
        &board[(height - 2) * width],
        &board[(height - 1) * width],
        lowerGhostRow,
        &newBoard[(height - 1) * width]
    );

    delete[] board;
    board = newBoard;
}
