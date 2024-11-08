#include <mpi.h>

#include <iostream>
#include <utils.hpp>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int proc_id;
    int procs_count;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    MPI_Comm_size(MPI_COMM_WORLD, &procs_count);

    // #1 Parse command-line arguments
    Args args;
    if (parseArguments(argc, argv, &args) == 1) {
        MPI_Finalize();
        return 1;
    }
    const bool verbose = args.is_verbose;

    // #2 Check if there are at least 2 or 3 processes in case of verbose mode
    if (procs_count < 2) {
        std::cerr << "At least 2 processes are required to run the program"
                  << std::endl;
        MPI_Finalize();
        return 1;
    }
    if (verbose && procs_count < 3) {
        std::cerr << "At least 3 processes are required to run the program in "
                     "verbose mode"
                  << std::endl;
        MPI_Finalize();
        return 1;
    }

    const int iterations = args.iterations;
    const int board_size = args.board_size;

    const int working_procs_count = verbose ? procs_count - 1 : procs_count;
    // Id of the last working process (not verbose one)
    const int last_proc_id = verbose ? procs_count - 2 : procs_count - 1;

    // Calculate number of rows for each process
    int* start_rows = new int[working_procs_count];
    int* num_rows = new int[working_procs_count];

    int row = 0;
    for (int p_id = 0; p_id < working_procs_count; ++p_id) {
        const int rows_for_proc =
            (board_size / working_procs_count) +
            (p_id < board_size % working_procs_count ? 1 : 0);
        start_rows[p_id] = row;
        num_rows[p_id] = rows_for_proc;
        row += rows_for_proc;
    }

    const int proc_start_row = start_rows[proc_id];
    const int proc_rows_num = num_rows[proc_id];

    // #3 Init main board
    Board board(board_size, board_size);
    board.Init(args.init_type);

    // #4 Copy board part to subBoard
    Board proc_board =
        Board::createSubBoard(board, proc_start_row, proc_rows_num);

    // #5.1 If verbose process (last process) gather data and save snapshot
    if (verbose && proc_id == last_proc_id + 1) {
        Cell* snapshot_board = new Cell[board_size * board_size];

        // Save first iteration
        savePGM(PGMFromBoard(board), args.output_directory, 0);

        // Save rest iterations
        for (int iter = 1; iter <= iterations; ++iter) {
            MPI_Request* requests = new MPI_Request[last_proc_id + 1];
            int sum = 0;
            for (int i = 0; i <= last_proc_id; ++i) {
                MPI_Irecv(
                    &snapshot_board[start_rows[i] * board_size],
                    board_size * num_rows[i],
                    MPI_INT,
                    i,
                    i,
                    MPI_COMM_WORLD,
                    &requests[i]
                );

                sum += board_size * num_rows[i];
            }
            MPI_Waitall(last_proc_id + 1, requests, MPI_STATUSES_IGNORE);

            PGM pgm = PGMFromCells(
                snapshot_board,
                board_size,
                board_size,
                &start_rows[1],
                last_proc_id
            );
            savePGM(pgm, args.output_directory, iter);

            delete[] requests;
        }

        delete[] snapshot_board;
        delete[] start_rows;
        delete[] num_rows;
        MPI_Finalize();
        return 0;
    }

    for (int iter = 0; iter < iterations; ++iter) {
        // #5.2 Exchange data with neighbors
        Cell* upperGhostRow = new Cell[board_size]{};
        Cell* lowerGhostRow = new Cell[board_size]{};

        int status = MPI_SUCCESS;
        // Send and receive upper row
        if (proc_id > 0) {
            status = MPI_Sendrecv(
                proc_board.getRow(0),
                board_size,
                MPI_INT,
                proc_id - 1,
                0,
                upperGhostRow,
                board_size,
                MPI_INT,
                proc_id - 1,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
        }
        if (status != MPI_SUCCESS) {
            std::cerr << "Error in UPPER row sync -> proc_id: " << proc_id
                      << std::endl;
        }

        // Send and receive lower row
        if (proc_id < last_proc_id) {
            MPI_Sendrecv(
                proc_board.getRow(proc_rows_num - 1),
                board_size,
                MPI_INT,
                proc_id + 1,
                0,
                lowerGhostRow,
                board_size,
                MPI_INT,
                proc_id + 1,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
        }
        if (status != MPI_SUCCESS) {
            std::cerr << "Error in LOWER row sync -> proc_id: " << proc_id
                      << std::endl;
        }

        // #6 Update board
        proc_board.updateBoard(upperGhostRow, lowerGhostRow);

        // #7 If verbose sent info to verbose process
        if (verbose) {
            MPI_Request request = MPI_REQUEST_NULL;
            status = MPI_Isend(
                proc_board.getBoard(),
                board_size * proc_rows_num,
                MPI_INT,
                last_proc_id + 1,
                proc_id,
                MPI_COMM_WORLD,
                &request
            );

            if (status != MPI_SUCCESS) {
                std::cerr
                    << "Error in sending data to verbose process -> proc_id: "
                    << proc_id << std::endl;
            }

            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
    }

    delete[] start_rows;
    delete[] num_rows;
    MPI_Finalize();
    return 0;
}
