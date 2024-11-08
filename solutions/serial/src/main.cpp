#include <utils.hpp>

int main(const int argc, char* argv[]) {
    // #1 Parse command-line arguments
    Args args;
    if (parseArguments(argc, argv, &args) == 1) {
        return 1;
    }

    // #2 Initialize board
    Board board(args.board_size, args.board_size);
    board.Init(args.init_type);

    // #3 Run iterations
    for (int i = 0; i < args.iterations; ++i) {
        // #4 Save snapshot if verbose
        if (args.is_verbose) {
            PGM pbm = PGMFromBoard(board);
            savePGM(pbm, args.output_directory, i);
        }

        // #5 Update board with empty ghost rows
        board.updateBoard(nullptr, nullptr);
    }

    if (args.is_verbose) {
        const PGM pbm = PGMFromBoard(board);
        savePGM(pbm, args.output_directory, args.iterations);
    }
}
