#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include "../include/board.hpp"

struct Args {
    int matrixSize;
    int iterations;
    BoardInitType initType;
    std::string outputDirectory;
    bool is_verbose = false;
};

struct PBM {
    const int width;
    const int height;
    u_int8_t* data;
    PBM(const int width, const int height, u_int8_t* data)
        : width(width), height(height), data(data) {}
    ~PBM() { delete[] data; }
};

int parseArguments(int argc, char* argv[], Args* args);

PBM PBMFromBoard(const Board& board);

void savePBM(const PBM& pbm, const std::string& args, int iteration);

#endif  // UTILS_HPP
