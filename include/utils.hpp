#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include "../include/board.hpp"

struct Args {
    int board_size;
    int iterations;
    BoardInitType init_type;
    std::string output_directory;
    bool is_verbose = false;
};

struct PGM {
    const int width;
    const int height;
    u_int8_t* data;
    PGM(const int width, const int height, u_int8_t* data)
        : width(width), height(height), data(data) {}
    ~PGM() { delete[] data; }
};

int parseArguments(int argc, char* argv[], Args* args);

PGM PGMFromBoard(const Board& board);

void savePGM(const PGM& pgm, const std::string& output_directory, int iteration);

#endif  // UTILS_HPP
