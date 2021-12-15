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
            map[x][y] = row[x] - '0';
        }
    }
}

static void dump(int n, const uint8_t map[n][n]) {
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            printf("%c", map[x][y] + '0');
        }
        printf("\n");
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

    for (;;) {
        uint32_t minFScore = UINT32_MAX;
        int c[2] = {-1, -1};

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (openSet[i][j]) {
                    if (fScore[i][j] < minFScore) {
                        minFScore = fScore[i][j];
                        c[0] = i;
                        c[1] = j;
                    }
                }
            }
        }

        if (minFScore == UINT32_MAX) {
            assert(false && "Can't find any nodes in the openSet");
        }

        if (c[0] == n - 1 && c[1] == n - 1) {
            printf("Found the goal! gScore: %d\n", gScore[n - 1][n - 1]);
            return gScore[n - 1][n - 1];
        }

        openSet[c[0]][c[1]] = false;

        int neigh[4][2] = {
            {c[0], c[1] - 1}, // Top.
            {c[0] + 1, c[1]}, // Right.
            {c[0], c[1] + 1}, // Bottom.
            {c[0] - 1, c[1]}  // Left.
        };

        for (int i = 0; i < 4; ++i) {
            if (neigh[i][0] >= 0 && neigh[i][0] < n && neigh[i][1] >= 0 && neigh[i][1] < n) {
                uint32_t tentative_gScore = gScore[c[0]][c[1]] + map[neigh[i][0]][neigh[i][1]];

                if (tentative_gScore < gScore[neigh[i][0]][neigh[i][1]]) {
                    gScore[neigh[i][0]][neigh[i][1]] = tentative_gScore;

                    fScore[neigh[i][0]][neigh[i][1]] = tentative_gScore + ((n - 1 - neigh[i][0]) + (n - 1 - neigh[i][1]));

                    openSet[neigh[i][0]][neigh[i][1]] = true;
                }
            }
        }
    }
}

static int partOne(int n, const uint8_t map[n][n]) {
    return aStarSearch(n, map);
}

static int partTwo(int n, const uint8_t map[n][n]) {
    int m = n * 5;
    uint8_t map2[m][m];

    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            map2[x][y] = map[x][y];
        }
    }

    for (int k = 0; k < 5; ++k) {
        for (int y = k * n; y < (k + 1) * n; ++y) {
            for (int x = n; x < m; ++x) {
                map2[x][y] = map2[x - n][y] + 1;
                if (map2[x][y] > 9) {
                    map2[x][y] = 1;
                }
            }
        }

        for (int y = n; y < m; ++y) {
            for (int x = k * n; x < (k + 1) * n; ++x) {
                map2[x][y] = map2[x][y - n] + 1;
                if (map2[x][y] > 9) {
                    map2[x][y] = 1;
                }
            }
        }
    }

    return aStarSearch(m, map2);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int n = parseSize(input);

    uint8_t map[n][n];
    parseMap(input, n, map);
    dump(n, map);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, map),
                   40, 441);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, map),
                   315, 2849);

    return 0;
}
