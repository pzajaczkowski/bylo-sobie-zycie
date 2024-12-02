#ifndef BOARD_HPP
#define BOARD_HPP

enum Cell { DEAD = 0, ALIVE = 1 };

enum BoardInitType {
    LINE = 0,
    T_SHAPE = 1,
    CROSS = 2,
};

// Board class uses 1D array to store 2D board due to performance reasons
class Board {
public:
    // Constructors and destructor
    Board(int width, int height);

    ~Board();

    // Accessors
    [[nodiscard]] Cell *getRow(int y) const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] Cell *getBoard() const;

    // Mutators
    void setCell(int x, int y, Cell value);

    void setBoard(Cell *new_board);

    void Init(BoardInitType type);

    void updateRow(
        const Cell *prevRow,
        const Cell *currRow,
        const Cell *nextRow,
        Cell *newRow
    ) const;

    void updateBoard(const Cell *upperGhostRow, const Cell *lowerGhostRow);

    void updateBoardWithoutEdges();

    void updateBoardEdges(
        const Cell *upperGhostRow,
        const Cell *lowerGhostRow
    );

    // Static

    static Board
    createSubBoard(const Board &board, int start_row, int rows_number);

private:
    int width;
    int height;
    alignas(64) Cell *board;
    alignas(64) Cell *new_board;
};

#endif  // BOARD_HPP
