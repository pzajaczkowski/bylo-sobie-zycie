#include "../include/utils.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

void printParseArgumentsUsage(int argc, char* argv[]) {
    std::cout << "Usage: " << argv[0]
              << " <size> <iterations> <type> <output_directory>\n"
              << "  type: type of the initial board\n"
              << "    0: line\n"
              << "    1: t shape\n"
              << "    2: cross\n"
              << "  output_directory: directory to save the output (verbose)\n";
}

// Function to parse command-line arguments
int parseArguments(const int argc, char* argv[], Args* args) {
    if (argc < 3) {
        printParseArgumentsUsage(argc, argv);
        return 1;
    }

    args->matrixSize = atoi(argv[1]);
    args->iterations = atoi(argv[2]);
    args->initType = static_cast<BoardInitType>(atoi(argv[3]));

    if (argc > 4) {
        args->outputDirectory = argv[4];
        args->is_verbose = true;
    }

    return 0;
}

PBM PBMFromBoard(const Board& board) {
    const int width = board.getWidth();
    const int height = board.getHeight();
    const Cell* boardData = board.getBoard();
    PBM pbm{width, height, new u_int8_t[width * height]};

    for (int i = 0; i < width * height; ++i) {
        pbm.data[i] = boardData[i] == ALIVE ? 255 : 0;
    }

    return pbm;
}

void savePBM(
    const PBM& pbm,
    const std::string& outputDirectory,
    int iteration
) {
    std::filesystem::__cxx11::path snapshotsPath =
        std::filesystem::__cxx11::path(outputDirectory);
    create_directories(snapshotsPath);

    std::string filename =
        (snapshotsPath / ("snapshot_" + std::to_string(iteration) + ".pgm"))
            .string();

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to open file for writing: " << filename
                  << std::endl;
        return;
    }

    // Header
    outfile << "P5\n" << pbm.width << " " << pbm.height << "\n255\n";
    // Data
    outfile.write(
        reinterpret_cast<const char*>(pbm.data),
        pbm.width * pbm.height
    );
    outfile.close();
}