#include "Helpers.c"

#define OPEN_CAP 2000

typedef struct {
    uint16_t y;
    uint16_t x;
} Coord;

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

// Inspired from https://en.wikipedia.org/wiki/A*_search_algorithm,
// h(n) is the Manhattan distance to bottom-right (n - 1, n - 1)
static uint32_t aStarSearch(int n, const uint8_t map[n][n]) {
    // Cost of the shortest path from start to node n currently known.
    uint32_t gScore[n][n];
    memset(gScore, UINT32_MAX, n * n * sizeof(gScore[0][0])); // Fill with "infinity".

    // Current best guess to how short a path from start to goal can be if it goes through node n.
    uint32_t fScore[n][n];
    memset(fScore, UINT32_MAX, n * n * sizeof(fScore[0][0])); // Fill with "infinity".

    // List of open nodes that may be revisited.
    Coord open[OPEN_CAP] = {0};

    // Insert start node (0,0).
    int nOpen = 1;
    open[0] = (Coord){.y = 0, .x = 0};

    // Set initial score to 0 at (0, 0)
    gScore[0][0] = 0;

    // Best case scenario from (0, 0) to (n - 1, n - 1).
    fScore[0][0] = (n - 1 - 0) + (n - 1 - 0); // h((0,0))

    for (;;) {
        assert(nOpen > 0 && "Can't find any open nodes :/");

        // Find the open node with smallest fScore value. O(n).
        int iOpen = 0;
        uint32_t minFScore = fScore[open[0].y][open[0].x];

        for (int i = 1; i < nOpen; ++i) {
            if (fScore[open[i].y][open[i].x] < minFScore) {
                minFScore = fScore[open[i].y][open[i].x];
                iOpen = i;
            }
        }

        uint16_t cy = open[iOpen].y;
        uint16_t cx = open[iOpen].x;

        // If we've reached the goal, (n - 1, n - 1), return our gScore.
        if (cx == n - 1 && cy == n - 1) {
            return gScore[n - 1][n - 1];
        }

        // Remove current node from our open list as we have discovered it now. O(n).
        for (int i = iOpen; i < nOpen - 1; ++i) {
            open[i] = open[i + 1];
        }
        --nOpen;

        // Neighbor nodes to current node.
        uint16_t neigh[4][2] = {
            {cy, cx - 1}, // Left.
            {cy + 1, cx}, // Bottom.
            {cy, cx + 1}, // Right.
            {cy - 1, cx}  // Top.
        };

        // Consider every neighbor.
        for (int i = 0; i < 4; ++i) {
            uint16_t ny = neigh[i][0];
            uint16_t nx = neigh[i][1];

            if (ny < n && nx < n) {
                uint32_t tentativeGScore = gScore[cy][cx] + map[ny][nx];

                if (tentativeGScore < gScore[ny][nx]) {
                    // Found a cheaper path than recorded, record this one.
                    gScore[ny][nx] = tentativeGScore;
                    fScore[ny][nx] = tentativeGScore + ((n - 1 - ny) + (n - 1 - nx));

                    // Insert neighbor node. O(1).
                    open[nOpen++] = (Coord){.y = ny, .x = nx};
                    assert(nOpen < OPEN_CAP);
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
