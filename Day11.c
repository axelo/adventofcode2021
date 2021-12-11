#include "Helpers.c"

#define N 10

static int parse(const char *input, int octopuses[N][N]) {
    int charsRead;

    for (int y = 0; y < N; ++y) {
        int filled = sscanf(
            input, "%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%n",
            &octopuses[y][0], &octopuses[y][1], &octopuses[y][2], &octopuses[y][3], &octopuses[y][4],
            &octopuses[y][5], &octopuses[y][6], &octopuses[y][7], &octopuses[y][8], &octopuses[y][9],
            &charsRead);

        assert(filled == 10);
        input += charsRead;
    }

    return N;
}

static int simulate(int octopuses[N][N]) {
    // First, the energy level of each octopus increases by 1.
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            ++octopuses[y][x];
        }
    }

    // Then, flash any octopus once with energy level 10 (we only want to flash once).
    bool flashedDueToPropagation;

    do {
        flashedDueToPropagation = false;

        for (int y = 0; y < N; ++y) {
            for (int x = 0; x < N; ++x) {
                if (octopuses[y][x] == 10) {
                    ++octopuses[y][x]; // Mark as already flashed.

                    // Propagate flash energy to adjacent points.
                    for (int y2 = y - 1; y2 <= y + 1; ++y2) {
                        for (int x2 = x - 1; x2 <= x + 1; ++x2) {
                            if (x2 >= 0 && y2 >= 0 && x2 < N && y2 < N && octopuses[y2][x2] < 10) {
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

    // Finally, any octopus that flashed, reset energy level to 0.
    int nFlashes = 0;

    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            if (octopuses[y][x] > 9) {
                octopuses[y][x] = 0;
                ++nFlashes;
            }
        }
    }

    return nFlashes;
}

static int partOne(const int initialState[N][N]) {
    int octopuses[N][N];
    memcpy(octopuses, initialState, sizeof(octopuses));

    int totalFlashes = 0;

    for (int step = 0; step < 100; ++step) {
        totalFlashes += simulate(octopuses);
    }

    return totalFlashes;
}

static int partTwo(const int initialState[N][N]) {
    int octopuses[N][N];
    memcpy(octopuses, initialState, sizeof(octopuses));

    int step = 1;

    while (simulate(octopuses) != N * N) {
        ++step;
    }

    return step;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int octopuses[N][N] = {0};
    parse(input, octopuses);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(octopuses),
                   1656, 1686);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(octopuses),
                   195, 360);

    return 0;
}
