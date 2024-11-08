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
    [[nodiscard]] Cell* getRow(int y) const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] Cell* getBoard() const;

    // Mutators
    void setCell(int x, int y, Cell value);
    void Init(BoardInitType type);
    void updateRow(
        const Cell* prevRow,
        const Cell* currRow,
        const Cell* nextRow,
        Cell* newRow
    ) const;
    void updateBoard(const Cell* upperGhostRow, const Cell* lowerGhostRow);

   private:
    int width;
    int height;
    Cell* board;
};

#endif  // BOARD_HPP
