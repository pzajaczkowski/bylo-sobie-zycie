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

    args->board_size = atoi(argv[1]);
    args->iterations = atoi(argv[2]);
    args->init_type = static_cast<BoardInitType>(atoi(argv[3]));

    if (argc > 4) {
        args->output_directory = argv[4];
        args->is_verbose = true;
    }

    return 0;
}

PGM PGMFromBoard(const Board& board) {
    const int width = board.getWidth();
    const int height = board.getHeight();
    const Cell* board_data = board.getBoard();
    PGM pgm{width, height, new u_int8_t[width * height]};

    for (int i = 0; i < width * height; ++i) {
        pgm.data[i] = board_data[i] == ALIVE ? 255 : 0;
    }

    return pgm;
}

PGM PGMFromCells(
    const Cell* cells,
    const int width,
    const int height,
    const int* edge_rows,
    const int edge_row_count
) {
    PGM pgm{width, height, new u_int8_t[width * height]};
    int edge_idx = 0;

    for (int row = 0; row < height; ++row) {
        const bool is_edge_row =
            (edge_idx < edge_row_count && edge_rows[edge_idx] == row);

        for (int col = 0; col < width; ++col) {
            const int idx = row * width + col;
            if (is_edge_row) {
                pgm.data[idx] = cells[idx] == ALIVE ? 255 : 69;
            } else {
                pgm.data[idx] = cells[idx] == ALIVE ? 255 : 0;
            }
        }

        if (is_edge_row) {
            ++edge_idx;
        }
    }

    return pgm;
}

void savePGM(
    const PGM& pgm,
    const std::string& output_directory,
    int iteration
) {
    std::filesystem::__cxx11::path snapshots_path =
        std::filesystem::__cxx11::path(output_directory);
    create_directories(snapshots_path);

    std::string filename =
        (snapshots_path / ("snapshot_" + std::to_string(iteration) + ".pgm"))
            .string();

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to open file for writing: " << filename
                  << std::endl;
        return;
    }

    // Header
    outfile << "P5\n" << pgm.width << " " << pgm.height << "\n255\n";
    // Data
    outfile.write(
        reinterpret_cast<const char*>(pgm.data),
        pgm.width * pgm.height
    );
    outfile.close();
}