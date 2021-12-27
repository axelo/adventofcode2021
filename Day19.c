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

// static bool overlapping(const Coord beacons[COORDS_CAP] b1, Coord beacon b) {
//     for (int i = 0; i < 25; ++i) {
//         for (int j = i + 1; j < 25; ++j) {
//             printf("bs0dist[%d][%d] = %d\n", i, j, bs0dist[i][j]);
//         }
//     }
// }

static int blubb(const Scanner originScanner, const Scanner relativeScanner, Coord overlap[12][2]) {

    int bs0dist[25][25] = {0};
    int bs1dist[25][25] = {0};

    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            // Abs [a − x] + Abs [b − y] + Abs [c − z]
            bs0dist[i][j] = abs(originScanner.beacons[i].x - originScanner.beacons[j].x) +
                            abs(originScanner.beacons[i].y - originScanner.beacons[j].y) +
                            abs(originScanner.beacons[i].z - originScanner.beacons[j].z);

            bs1dist[i][j] = abs(relativeScanner.beacons[i].x - relativeScanner.beacons[j].x) +
                            abs(relativeScanner.beacons[i].y - relativeScanner.beacons[j].y) +
                            abs(relativeScanner.beacons[i].z - relativeScanner.beacons[j].z);
        }
    }

    int bar = 0;

    for (int i = 0; i < 25; ++i) {
        int foo = 0;
        int t[25] = {0};

        for (int j = 0; j < 25; ++j) {

            if (i != j) {
                int nSame = 0;

                // printf("bs0dist[%d][%d] = %d should be found in 11 other beacons\n", i, j, bs0dist[i][j]);

                for (int k = 0; k < 25; ++k) {
                    // printf("Checking agains source k: %d\n", k);

                    for (int l = 0; l < 25; ++l) {
                        // printf("bs1dist[%d][%d] = %d\n", i, j, bs1dist[i][j]);

                        if (k != l && bs0dist[k][l] == bs1dist[i][j]) {
                            ++nSame;
                            ++t[k];
                            // printf("k is %d\n", k);
                        }
                    }
                }

                if (nSame > 0) {
                    // printf("nSame is: %d\n", nSame);
                    ++foo;
                }
            }
        }

        if (foo >= 11) { // replace foo with finding max t.
            int maxTI = 0;
            for (int c = 0; c < 25; ++c) {
                // printf("t[%d] = %d\n", c, t[c]);
                if (t[c] > t[maxTI]) {
                    maxTI = c;
                }
            }

            // printf("max ti: %d\n", maxTI);

            overlap[bar][0] = originScanner.beacons[maxTI];
            overlap[bar][1] = relativeScanner.beacons[i];

            ++bar;

            assert(bar < 13);

            // printf("foo is %d for relative scanner beacon %4d,%4d,%4d - origin scanner beacon %4d,%4d,%4d\n", foo,
            //        relativeScanner.beacons[i].x,
            //        relativeScanner.beacons[i].y,
            //        relativeScanner.beacons[i].z,
            //        originScanner.beacons[maxTI].x,
            //        originScanner.beacons[maxTI].y,
            //        originScanner.beacons[maxTI].z);
        }
    }

    // printf("\nbar is %d\n", bar);
    return bar;
}

static int64_t partOne(uint32_t n, const Scanner scanners[n]) {
    printf("n scanners: %d\n", n);
    printf("scanners 0 beacons: %d\n", scanners[0].n);
    printf("scanners 1 beacons: %d\n", scanners[1].n);

    Coord overlap[12][2];

    if (blubb(scanners[0], scanners[1], overlap) >= 12) {

        for (int i = 0; i < 12; ++i) {
            printf("Origin scanner beacon %4d,%4d,%4d; Relative scanner beacon %4d,%4d,%4d\n",
                   overlap[i][0].x,
                   overlap[i][0].y,
                   overlap[i][0].z,
                   overlap[i][1].x,
                   overlap[i][1].y,
                   overlap[i][1].z);
        }
    }

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
