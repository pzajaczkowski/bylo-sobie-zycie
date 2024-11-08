#include <utils.hpp>

int main(const int argc, char* argv[]) {
    // #1 Parse command-line arguments
    Args args;
    if(!parseArguments(argc, argv, &args)) {
        return 1;
    }

    // #2 Initialize board
    Board board(args.matrixSize, args.matrixSize);
    board.Init(args.initType);

    // #3 Run iterations
    for (int i = 0; i < args.iterations; ++i) {
        // #4 Save snapshot if verbose
        if (args.is_verbose) {
            PBM pbm = PBMFromBoard(board);
            savePBM(pbm, args.outputDirectory, i);
        }

        // #5 Update board with empty ghost rows
        board.updateBoard(nullptr, nullptr);
    }

    if (args.is_verbose) {
        const PBM pbm = PBMFromBoard(board);
        savePBM(pbm, args.outputDirectory, args.iterations);
    }
}
