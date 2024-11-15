#include <mpi.h>
#include <iostream>
#include <vector>
#include <utils.hpp>
#include <board.hpp>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int proc_id, procs_count;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);  // Get process ID
    MPI_Comm_size(MPI_COMM_WORLD, &procs_count);  // Get number of processes

    // Parse command-line arguments
    Args args;
    if (parseArguments(argc, argv, &args) == 1) {
        MPI_Finalize();
        return 1;
    }
    const bool verbose = args.is_verbose;
    const int iterations = args.iterations;
    const int board_size = args.board_size;

    // Check if there are enough processes
    if (procs_count < 2 || (verbose && procs_count < 3)) {
        std::cerr << "At least " << (verbose ? 3 : 2) << " processes are required." << std::endl;
        MPI_Finalize();
        return 1;
    }

    const int working_procs_count = verbose ? procs_count - 1 : procs_count;
    const int last_proc_id = verbose ? procs_count - 2 : procs_count - 1;

    // Calculate rows for each process
    std::vector<int> start_rows(working_procs_count), num_rows(working_procs_count);
    int row = 0;
    for (int p_id = 0; p_id < working_procs_count; ++p_id) {
        const int rows_for_proc = (board_size / working_procs_count) + (p_id < board_size % working_procs_count ? 1 : 0);
        start_rows[p_id] = row;
        num_rows[p_id] = rows_for_proc;
        row += rows_for_proc;
    }

    const int proc_start_row = start_rows[proc_id];
    const int proc_rows_num = num_rows[proc_id];

    // Initialize the game board
    Board board(board_size, board_size);
    board.Init(args.init_type);

    // Create sub-board for the process
    Board proc_board = Board::createSubBoard(board, proc_start_row, proc_rows_num);

    // Verbose process: save board snapshots
    if (verbose && proc_id == last_proc_id + 1) {
        std::vector<Cell> snapshot_board(board_size * board_size);
        savePGM(PGMFromBoard(board), args.output_directory, 0);  // Save initial snapshot

        for (int iter = 1; iter <= iterations; ++iter) {
            std::vector<MPI_Request> requests(last_proc_id + 1);

            // Receive data from all processes for verbose output
            for (int i = 0; i <= last_proc_id; ++i) {
                MPI_Irecv(&snapshot_board[start_rows[i] * board_size], board_size * num_rows[i], MPI_INT,
                          i, i, MPI_COMM_WORLD, &requests[i]);
            }
            MPI_Waitall(last_proc_id + 1, requests.data(), MPI_STATUSES_IGNORE);

            // Save the snapshot of the board
            PGM pgm = PGMFromCells(snapshot_board.data(), board_size, board_size, &start_rows[1], last_proc_id);
            savePGM(pgm, args.output_directory, iter);
        }

        MPI_Finalize();
        return 0;
    }

    // Main game loop
    for (int iter = 0; iter < iterations; ++iter) {
        // Asynchronous communication with neighbors
        std::vector<Cell> upperGhostRow(board_size), lowerGhostRow(board_size);
        MPI_Request req_upper_recv, req_upper_send;
        MPI_Request req_lower_recv, req_lower_send;

        // Non-blocking receive and send with the upper neighbor
        if (proc_id > 0) {
            MPI_Irecv(upperGhostRow.data(), board_size, MPI_INT, proc_id - 1, 0, MPI_COMM_WORLD, &req_upper_recv);
            MPI_Isend(proc_board.getRow(0), board_size, MPI_INT, proc_id - 1, 1, MPI_COMM_WORLD, &req_upper_send);
        }

        // Non-blocking receive and send with the lower neighbor
        if (proc_id < last_proc_id) {
            MPI_Irecv(lowerGhostRow.data(), board_size, MPI_INT, proc_id + 1, 1, MPI_COMM_WORLD, &req_lower_recv);
            MPI_Isend(proc_board.getRow(proc_rows_num - 1), board_size, MPI_INT, proc_id + 1, 0, MPI_COMM_WORLD, &req_lower_send);
        }

        // Wait for upper row communication
        if (proc_id > 0) {
            MPI_Wait(&req_upper_recv, MPI_STATUS_IGNORE);
            MPI_Wait(&req_upper_send, MPI_STATUS_IGNORE);
        }

        // Wait for lower row communication
        if (proc_id < last_proc_id) {
            MPI_Wait(&req_lower_recv, MPI_STATUS_IGNORE);
            MPI_Wait(&req_lower_send, MPI_STATUS_IGNORE);
        }

        // Update board with received ghost rows
        proc_board.updateBoard(upperGhostRow.data(), lowerGhostRow.data());

        // If verbose, send data to the last process
        if (verbose) {
            MPI_Request request;
            MPI_Isend(proc_board.getBoard(), board_size * proc_rows_num, MPI_INT, last_proc_id + 1, proc_id, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();
    return 0;
}
