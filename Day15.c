#include "Helpers.c"

#define BIN_HEAP_CAP 2000

typedef struct {
    uint16_t y;
    uint16_t x;
} Coord;

// Inspired by https://www.geeksforgeeks.org/binary-heap/.
// This version gets away by using 32 bits for the prio and another 32 bits for any user data.
typedef struct {
    uint64_t heap[BIN_HEAP_CAP];
    int n;
} PrioQueue;

static void PrioQueue_enqueue(uint32_t prio, uint32_t payload, PrioQueue *queue) {
    assert(queue->n <= BIN_HEAP_CAP);

    int i = queue->n++;
    queue->heap[i] = ((uint64_t)payload << 32) | prio;

    if (i == 0) {
        return;
    }

    int parent = (i - 1) / 2; // parent index relative i.

    // Bubble up until min property is held.
    while ((queue->heap[i] & 0xffffffff) < (queue->heap[parent] & 0xffffffff)) {
        // Swap values with parent and child.
        uint64_t tmp = queue->heap[parent];
        queue->heap[parent] = queue->heap[i];
        queue->heap[i] = tmp;

        i = parent;           // Move up one level.
        parent = (i - 1) / 2; // Next parent.
    }
}

static uint32_t PrioQueue_dequeue(PrioQueue *queue) {
    if (queue->n <= 0) {
        return UINT32_MAX;
    } else if (queue->n == 1) {
        return queue->heap[--queue->n] >> 32;
    }

    uint32_t payload = queue->heap[0] >> 32;

    queue->heap[0] = queue->heap[--queue->n];

    // Ensure min property is held.
    int i = 0;

    for (;;) {
        int l = (2 * i) + 1; // Left child index of i.
        int r = (2 * i) + 2; // Right child index of i.
        int smallest = i;

        if (l < queue->n && (queue->heap[l] & 0xffffffff) < (queue->heap[i] & 0xffffffff)) {
            smallest = l;
        }

        if (r < queue->n && (queue->heap[r] & 0xffffffff) < (queue->heap[smallest] & 0xffffffff)) {
            smallest = r;
        }

        if (smallest == i) {
            break;
        }

        // Swap smallest with i.
        uint64_t tmp = queue->heap[smallest];
        queue->heap[smallest] = queue->heap[i];
        queue->heap[i] = tmp;

        i = smallest; // Check next.
    }

    return payload;
}

static int parseSize(const char *input) {
    const char *inputFirstLine = input;

    while (*inputFirstLine != 0 && *inputFirstLine != '\n') {
        ++inputFirstLine;
    }

    return (int)(inputFirstLine - input);
}

static void parseMap(const char *input, int n, uint8_t map[n][n]) {
    int charsRead = 0;
    char row[n + 1];

    for (int y = 0; y < n; ++y) {
        assert(sscanf(input, "%s\n%n", row, &charsRead) == 1);
        assert(charsRead <= n + 1);
        input += charsRead;

        for (int x = 0; x < n; ++x) {
            map[y][x] = (uint8_t)(row[x] - '0');
        }
    }
}

// Inspired from https://en.wikipedia.org/wiki/A*_search_algorithm,
// h(n) is the Manhattan distance to bottom-right (n - 1, n - 1)
static uint32_t aStarSearch(int n, const uint8_t map[n][n]) {
    // List of open nodes that may be revisited.
    PrioQueue nodeByLowestPrio;
    nodeByLowestPrio.n = 0;

    // Cost of the shortest path from start to node n currently known.
    uint32_t gScore[n][n];
    memset(gScore, INT32_MAX, (uint32_t)n * (uint32_t)n * sizeof(gScore[0][0])); // Fill with "infinity".

    // Current best guess to how short a path from start to goal can be if it goes through node n.
    uint32_t fScore[n][n];
    memset(fScore, INT32_MAX, (uint32_t)n * (uint32_t)n * sizeof(fScore[0][0])); // Fill with "infinity".

    // Set initial score to 0 at (0, 0)
    gScore[0][0] = 0;

    // Best case scenario from (0, 0) to (n - 1, n - 1).
    fScore[0][0] = (uint32_t)((n - 1 - 0) + (n - 1 - 0)); // h((0,0))

    // Insert start node.
    PrioQueue_enqueue(fScore[0][0], (0 << 16) | 0, &nodeByLowestPrio);

    for (;;) {
        uint32_t cyAndCx = PrioQueue_dequeue(&nodeByLowestPrio); // O(log n).
        uint16_t cy = cyAndCx >> 16;
        uint16_t cx = cyAndCx & 0xffff;

        // If we've reached the goal, (n - 1, n - 1), return our gScore.
        if (cx == n - 1 && cy == n - 1) {
            return gScore[n - 1][n - 1];
        }

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
                    fScore[ny][nx] = tentativeGScore + (uint32_t)((n - 1 - ny) + (n - 1 - nx));

                    // Insert neighbor node. O(log n).
                    PrioQueue_enqueue(fScore[ny][nx], (uint32_t)(ny << 16) | nx, &nodeByLowestPrio);
                }
            }
        }
    }
}

static uint32_t partOne(int n, const uint8_t map[n][n]) {
    return aStarSearch(n, map);
}

static uint32_t partTwo(int n, const uint8_t tile[n][n]) {
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
