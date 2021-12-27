#include "Helpers.c"

#define CAP 30
#define COORDS_CAP 30

typedef struct {
    int x;
    int y;
    int z;
} Coord;

typedef struct {
    int n;
    Coord beacons[COORDS_CAP];
} Scanner;

// static int compareInt(const void *a, const void *b) {
//     return *(const int32_t *)a - *(const int32_t *)b;
// }

static uint32_t parse(const char *input, Scanner scanners[CAP]) {
    int charsRead = 0;
    int i = -1;
    uint32_t n = 0;

    while (sscanf(input, "--- scanner %d ---\n%n", &i, &charsRead) == 1) {
        input += charsRead;

        int x = 0;
        int y = 0;
        int z = 0;

        while (sscanf(input, "%d,%d,%d\n%n", &x, &y, &z, &charsRead) == 3) {
            input += charsRead;

            int j = scanners[i].n;

            scanners[i].beacons[j].x = x;
            scanners[i].beacons[j].y = y;
            scanners[i].beacons[j].z = z;

            ++scanners[i].n;
        }
    }

    return (uint32_t)i + 1;
}

static int64_t partOne(uint32_t n, const Scanner scanners[n]) {
    printf("n scanners: %d\n", n);
    printf("scanners 0 beacons: %d\n", scanners[0].n);
    printf("scanners 1 beacons: %d\n", scanners[1].n);

    Coord bs0[3] = {{0, 2, 1}, {4, 1, 1}, {3, 3, 1}};
    Coord bs1[3] = {{-1, -1, 1}, {-5, 0, 1}, {-2, 1, 1}};

    int bs0dist[3][3] = {0};
    int bs1dist[3][3] = {0};

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            // Abs [a − x] + Abs [b − y] + Abs [c − z]
            bs0dist[i][j] = abs(bs0[i].x - bs0[j].x) +
                            abs(bs0[i].y - bs0[j].y) +
                            abs(bs0[i].z - bs0[j].z);

            bs1dist[i][j] = abs(bs1[i].x - bs1[j].x) +
                            abs(bs1[i].y - bs1[j].y) +
                            abs(bs1[i].z - bs1[j].z);
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

    Scanner scanners[CAP] = {0};
    uint32_t n = parse(input, scanners);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, scanners),
                   -1, -2);

    // Helpers_assert(PART2, Helpers_clock(),
    //                partTwo(n, xs),
    //                -1, -2);

    return 0;
}
