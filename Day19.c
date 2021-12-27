#include "Helpers.c"

#define CAP 100

typedef struct {
    int x;
    int y;
} Coord;

static int compareInt(const void *a, const void *b) {
    return *(const int32_t *)a - *(const int32_t *)b;
}

static uint32_t parse(const char *input, int xs[CAP]) {
    return 0;
}

static int64_t partOne(uint32_t n, const int xs[n]) {

    Coord bs0[3] = {{0, 2}, {4, 1}, {3, 3}};
    Coord bs1[3] = {{-1, -1}, {-5, 0}, {-2, 1}};

    int bs0dist[3][3] = {0};
    int bs1dist[3][3] = {0};

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            // Abs [a − x] + Abs [b − y] + Abs [c − z]
            bs0dist[i][j] = abs(bs0[i].x - bs0[j].x) + abs(bs0[i].y - bs0[j].y);

            bs1dist[i][j] = abs(bs1[i].x - bs1[j].x) + abs(bs1[i].y - bs1[j].y);
        }
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("bs0dist[%d][%d] = %d\n", i, j, bs0dist[i][j]);
        }
    }

    printf("vs\n");

    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            printf("bs1dist[%d][%d] = %d\n", i, j, bs1dist[i][j]);
        }
    }

    // 1 overlapping 0

    return -1;
}

static int64_t partTwo(uint32_t n, const int xs[n]) {
    return -1;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int xs[CAP] = {0};
    uint32_t n = parse(input, xs);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, xs),
                   -1, -2);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, xs),
                   -1, -2);

    return 0;
}
