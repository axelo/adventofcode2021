#include "Helpers.c"

static int parseSize(const char *input) {
    const char *inputFirstLine = input;

    while (*inputFirstLine != 0 && *inputFirstLine != '\n') {
        ++inputFirstLine;
    }

    return inputFirstLine - input;
}

static void parseMap(const char *input, int n, uint8_t map[n][n]) {
    int charsRead = 0;
    char row[n + 1];

    for (int y = 0; y < n; ++y) {
        assert(sscanf(input, "%s\n%n", row, &charsRead) == 1);
        assert(charsRead <= n + 1);
        input += charsRead;

        for (int x = 0; x < n; ++x) {
            map[y][x] = row[x] - '0';
        }
    }
}

// Inspired from https://en.wikipedia.org/wiki/A*_search_algorithm
// h(n) is the manhattan distance to bottom-right (n-1, n-1)
static int aStarSearch(int n, const uint8_t map[n][n]) {
    bool openSet[n][n];
    memset(openSet, false, n * n * sizeof(openSet[0][0]));

    // Cost of the cheapest path from start to node n currently known.
    uint32_t gScore[n][n];
    memset(gScore, UINT32_MAX, n * n * sizeof(gScore[0][0]));

    // Current best guess as to how short a path from start to finish can be if it goes through node n
    uint32_t fScore[n][n];
    memset(fScore, UINT32_MAX, n * n * sizeof(fScore[0][0]));

    openSet[0][0] = true;
    gScore[0][0] = 0;
    fScore[0][0] = (n - 1 - 0) + (n - 1 - 0); // h((0,0))

    int cy = 0;
    int cx = 0;

    for (;;) {
        uint32_t minFScore = UINT32_MAX;

        for (int y = 0; y < n; ++y) {
            for (int x = 0; x < n; ++x) {
                if (openSet[y][x]) {
                    if (fScore[y][x] < minFScore) {
                        minFScore = fScore[y][x];
                        cy = y;
                        cx = x;
                    }
                }
            }
        }

        if (minFScore == UINT32_MAX) {
            assert(false && "Can't find any open nodes :/");
        }

        if (cx == n - 1 && cy == n - 1) {
            return gScore[n - 1][n - 1];
        }

        openSet[cy][cx] = false;

        int neigh[4][2] = {
            {cy, cx - 1}, // Left.
            {cy + 1, cx}, // Bottom.
            {cy, cx + 1}, // Right.
            {cy - 1, cx}  // Top.
        };

        for (int i = 0; i < 4; ++i) {
            int ny = neigh[i][0];
            int nx = neigh[i][1];

            if (ny >= 0 && ny < n && nx >= 0 && nx < n) {
                uint32_t tentative_gScore = gScore[cy][cx] + map[ny][nx];

                if (tentative_gScore < gScore[ny][nx]) {
                    gScore[ny][nx] = tentative_gScore;
                    fScore[ny][nx] = tentative_gScore + ((n - 1 - ny) + (n - 1 - nx));

                    openSet[ny][nx] = true;
                }
            }
        }
    }
}

static int partOne(int n, const uint8_t map[n][n]) {
    return aStarSearch(n, map);
}

static int partTwo(int n, const uint8_t tile[n][n]) {
    int m = n * 5;
    uint8_t map[m][m];

    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            map[y][x] = tile[y][x];
        }
    }

    for (int i = 0; i < 5; ++i) {
        for (int y = i * n; y < (i + 1) * n; ++y) {
            for (int x = n; x < m; ++x) {
                map[y][x] = map[y][x - n] + 1;
                if (map[y][x] > 9) {
                    map[y][x] = 1;
                }
            }
        }

        for (int y = n; y < m; ++y) {
            for (int x = i * n; x < (i + 1) * n; ++x) {
                map[y][x] = map[y - n][x] + 1;
                if (map[y][x] > 9) {
                    map[y][x] = 1;
                }
            }
        }
    }

    return aStarSearch(m, map);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int n = parseSize(input);

    uint8_t map[n][n];
    parseMap(input, n, map);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, map),
                   40, 441);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, map),
                   315, 2849);

    return 0;
}
