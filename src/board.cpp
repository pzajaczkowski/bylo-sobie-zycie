#include "../include/board.hpp"

#include <cstring>
#include <new>
#include <utility>
#ifdef _OPENMP
#include <omp.h>
#endif

// Constructors

Board::Board(const int width, const int height)
    : width(width), height(height), board(new(std::align_val_t(64)) Cell[width * height]{}),
      new_board(new(std::align_val_t(64)) Cell[width * height]{}) {
}

Board::~Board() {
    operator delete[](board, std::align_val_t(64));
    operator delete[](new_board, std::align_val_t(64));
}

// Accessors

Cell *Board::getRow(const int y) const { return &board[y * width]; }

int Board::getWidth() const { return width; }

int Board::getHeight() const { return height; }

Cell *Board::getBoard() const { return board; }

// Mutators

inline void Board::setCell(const int x, const int y, const Cell value) {
    board[y * width + x] = value;
}

void Board::setBoard(Cell *new_board) {
    operator delete[](board, std::align_val_t(64));
    board = new_board;
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
                setCell(width / 2, i, ALIVE);
            }
            break;
    }
}

inline void Board::updateRow(
    const Cell *prevRow,
    const Cell *currRow,
    const Cell *nextRow,
    Cell *newRow
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

void Board::updateBoard(const Cell *upperGhostRow, const Cell *lowerGhostRow) {
    // first row
    updateRow(
        upperGhostRow,
        &board[0 * width],
        &board[1 * width],
        &new_board[0]
    );

    // middle rows
#pragma omp parallel for schedule(static)
    for (int i = 1; i < height - 1; ++i) {
        updateRow(
            &board[(i - 1) * width],
            &board[i * width],
            &board[(i + 1) * width],
            &new_board[i * width]
        );
    }

    // last row
    updateRow(
        &board[(height - 2) * width],
        &board[(height - 1) * width],
        lowerGhostRow,
        &new_board[(height - 1) * width]
    );

    std::swap(board, new_board);
}

void Board::updateBoardWithoutEdges() {

    // middle rows
#pragma omp parallel for schedule(static)
    for (int i = 1; i < height - 1; ++i) {
        updateRow(
            &board[(i - 1) * width],
            &board[i * width],
            &board[(i + 1) * width],
            &new_board[i * width]
        );
    }
}

void Board::updateBoardEdges(
    const Cell *upperGhostRow,
    const Cell *lowerGhostRow
) {
    // first row
    updateRow(
        upperGhostRow,
        &board[0 * width],
        &board[1 * width],
        &new_board[0]
    );

    // last row
    updateRow(
        &board[(height - 2) * width],
        &board[(height - 1) * width],
        lowerGhostRow,
        &new_board[(height - 1) * width]
    );

    std::swap(board, new_board);
}

// Static

Board Board::createSubBoard(
    const Board &board,
    const int start_row,
    const int rows_number
) {
    const Cell *parent_board_data = board.getBoard();
    Board sub_board(board.getWidth(), rows_number);
    Cell *sub_board_data = sub_board.getBoard();

    const int offset = start_row * sub_board.width;
    std::memcpy(
        sub_board_data,
        parent_board_data + offset,
        sizeof(Cell) * sub_board.width * rows_number
    );

    return sub_board;
}
