#include <stdbool.h> // bool

#define DAY 11
#define INPUT int input[SIZE][SIZE]

#define SIZE 10

#include "Runner.c"

typedef struct {
    int x;
    int y;
} Point;

static int parse(const char *inputString, int octopuses[SIZE][SIZE]) {
    int charsRead;

    for (int y = 0; y < SIZE; ++y) {
        int filled = sscanf(
            inputString, "%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%n",
            &octopuses[y][0], &octopuses[y][1], &octopuses[y][2], &octopuses[y][3], &octopuses[y][4],
            &octopuses[y][5], &octopuses[y][6], &octopuses[y][7], &octopuses[y][8], &octopuses[y][9],
            &charsRead);

        assert(filled == 10);
        inputString += charsRead;
    }

    return SIZE;
}

static int simulate(int octopuses[SIZE][SIZE]) {
    // First, the energy level of each octopus increases by 1.
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            ++octopuses[y][x];
        }
    }

    // Then, flash any octopus once with energy level 10 (we only want to flash once)
    bool flashedDueToPropagation;

    do {
        flashedDueToPropagation = false;

        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                if (octopuses[y][x] == 10) {
                    ++octopuses[y][x]; // Mark as flashed once.

                    // Propagate flash energy to adjacent points.
                    for (int y2 = y - 1; y2 <= y + 1; ++y2) {
                        for (int x2 = x - 1; x2 <= x + 1; ++x2) {
                            if (x2 >= 0 && y2 >= 0 && x2 < SIZE && y2 < SIZE && octopuses[y2][x2] < 10) {
                                if (++octopuses[y2][x2] == 10) {
                                    // Flag so we can re-iterate and propagate new flashes.
                                    flashedDueToPropagation = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (flashedDueToPropagation);

    // Finally, any octopus that flashed during this step has its energy level set to 0
    int nFlashes = 0;

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (octopuses[y][x] > 9) {
                octopuses[y][x] = 0;
                ++nFlashes;
            }
        }
    }

    return nFlashes;
}

static Result partOne(int n, const int initialState[n][SIZE]) {
    assert(n == SIZE);

    int octopuses[n][SIZE];
    memcpy(octopuses, initialState, sizeof(octopuses));

    int totalFlashes = 0;

    for (int step = 0; step < 100; ++step) {
        totalFlashes += simulate(octopuses);
    }

    return (Result){totalFlashes, 1656, 1686};
}

static Result partTwo(int n, const int initialState[n][SIZE]) {
    assert(n == SIZE);

    int octopuses[n][SIZE];
    memcpy(octopuses, initialState, sizeof(octopuses));

    int step = 1;

    while (simulate(octopuses) != SIZE * SIZE) {
        ++step;
    }

    return (Result){step, 195, 360};
}
