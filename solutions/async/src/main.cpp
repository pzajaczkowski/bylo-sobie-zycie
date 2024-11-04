#include <iostream>
#include <mpi.h>
#include <fstream>
#include <sys/stat.h>

using namespace std;

const bool ALIVE = true;
const bool DEAD = false;

void initializeBoard(bool** board, int rows, int cols, bool random = false) {
    for (int i = 1; i <= rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            board[i][j] = DEAD; // Wypełnij planszę martwymi komórkami
        }
    }

    // Ustaw pionową linię na środku
    int midCol = cols / 2; // Środkowa kolumna
    for (int i = 1; i <= rows; ++i) {
        board[i][midCol] = ALIVE; // Ustaw komórki w środkowej kolumnie na ALIVE
    }
}

int countNeighbors(bool** board, int x, int y, int rows, int cols) {
    int count = 0;
    for (int i = x - 1; i <= x + 1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            if (i == x && j == y) continue; // Pomijaj samą komórkę
            count += board[(i + rows) % rows][(j + cols) % cols]; // Cykl przez granice
        }
    }
    return count;
}

void updateBoard(bool** board, bool** newBoard, int rows, int cols) {
    for (int i = 1; i <= rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int neighbors = countNeighbors(board, i, j, rows, cols);
            if (board[i][j] == ALIVE) {
                newBoard[i][j] = (neighbors == 2 || neighbors == 3) ? ALIVE : DEAD;
            } else {
                newBoard[i][j] = (neighbors == 3) ? ALIVE : DEAD;
            }
        }
    }
}

// Funkcja do zapisywania planszy do pliku PGM
void saveSnapshot(bool** board, int rows, int cols, int iteration) {
    // Tworzenie folderu snapshots, jeśli nie istnieje
    mkdir("snapshots", 0777); // Ignoruj błąd, jeśli folder już istnieje

    // Tworzenie nazwy pliku
    string filename = "snapshots/snapshot_" + to_string(iteration) + ".pgm";
    ofstream file(filename);

    // Zapis nagłówka PGM
    file << "P2\n"; // P2 oznacza PGM w formacie tekstowym
    file << cols << " " << rows << "\n";
    file << "1\n"; // Maksymalna wartość szarości (1 dla martwej komórki, 0 dla żywej)

    // Zapis planszy
    for (int i = 1; i <= rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file << (board[i][j] == ALIVE ? 0 : 1) << " "; // ALIVE jako 0, DEAD jako 1
        }
        file << "\n";
    }

    file.close();
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Sprawdź, czy podano odpowiednią liczbę argumentów
    if (argc < 3) {
        if (rank == 0) {
            cout << "Użycie: " << argv[0] << " <rozmiar planszy N> <liczba iteracji>" << endl;
        }
        MPI_Finalize();
        return 1; // Zakończ program z kodem błędu
    }

    int N = std::atoi(argv[1]); // Rozmiar planszy z pierwszego argumentu
    int iterations = std::atoi(argv[2]); // Liczba iteracji z drugiego argumentu

    int rows = N / size; // Liczba wierszy na proces
    bool** subBoard = new bool*[rows + 2]; // Wiersze + "ghost rows"
    bool** newBoard = new bool*[rows + 2];
    for (int i = 0; i < rows + 2; ++i) {
        subBoard[i] = new bool[N];
        newBoard[i] = new bool[N];
    }

    initializeBoard(subBoard, rows, N); // Inicjalizuj planszę

    for (int iter = 0; iter < iterations; ++iter) {
        // Wymiana danych z sąsiadami
        if (rank > 0) {
            MPI_Sendrecv(subBoard[1], N, MPI_C_BOOL, rank - 1, 0,
                         subBoard[0], N, MPI_C_BOOL, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            MPI_Sendrecv(subBoard[rows], N, MPI_C_BOOL, rank + 1, 0,
                         subBoard[rows + 1], N, MPI_C_BOOL, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        updateBoard(subBoard, newBoard, rows, N); // Aktualizuj planszę

        // Zamień tablice
        bool** temp = subBoard;
        subBoard = newBoard;
        newBoard = temp;

        // Zapisz zrzut planszy po każdej iteracji (tylko w procesie 0)
        if (rank == 0) {
            saveSnapshot(subBoard, rows, N, iter);
        }
    }

    // Gromadzenie wyników
    bool* finalBoard = nullptr;
    if (rank == 0) {
        finalBoard = new bool[N * N];
    }

    MPI_Gather(&(subBoard[1][0]), rows * N, MPI_C_BOOL, finalBoard, rows * N, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    // Wyświetl końcowy stan planszy
    if (rank == 0) {
        // for (int i = 0; i < N; ++i) {
        //     for (int j = 0; j < N; ++j) {
        //         cout << (finalBoard[i * N + j] == ALIVE ? '.' : ' ') << " ";
        //     }
        //     cout << endl;
        // }
        delete[] finalBoard; // Zwolnij pamięć
    }

    // Zwolnij pamięć
    for (int i = 0; i < rows + 2; ++i) {
        delete[] subBoard[i];
        delete[] newBoard[i];
    }
    delete[] subBoard;
    delete[] newBoard;

    MPI_Finalize(); // Zakończ MPI
    return 0;
}
