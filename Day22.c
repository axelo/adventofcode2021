#include "Helpers.c"

#define CAP 500

typedef struct {
    int x1;
    int x2;
    int y1;
    int y2;
    int z1;
    int z2;
} Cuboid;

typedef struct {
    bool on;
    Cuboid cuboid;
} Step;

static bool grid[900][900][900];

static int parse(const char *input, Step steps[CAP]) {
    char s[5] = {0};
    int x1;
    int x2;
    int y1;
    int y2;
    int z1;
    int z2;
    int charsRead = 0;

    int n = 0;

    while (sscanf(input, "%4s x=%d..%d,y=%d..%d,z=%d..%d\n%n", s, &x1, &x2, &y1, &y2, &z1, &z2, &charsRead) == 7) {
        input += charsRead;
        steps[n++] = (Step){strlen(s) == 2, {x1, x2, y1, y2, z1, z2}};

        assert(n < CAP);
    }

    return n;
}

static inline int max(int a, int b) {
    return a > b ? a : b;
}

static inline int min(int a, int b) {
    return a < b ? a : b;
}

static int partOne(int n, const Step steps[n]) {
    memset(grid, false, sizeof(grid));

    for (int step = 0; step < n; ++step) {
        Cuboid cuboid = steps[step].cuboid;

        int x1 = max(cuboid.x1, -50);
        int y1 = max(cuboid.y1, -50);
        int z1 = max(cuboid.z1, -50);

        int x2 = min(cuboid.x2, 50);
        int y2 = min(cuboid.y2, 50);
        int z2 = min(cuboid.z2, 50);

        for (int x = x1; x <= x2; ++x) {
            for (int y = y1; y <= y2; ++y) {
                for (int z = z1; z <= z2; ++z) {
                    grid[x + 50][y + 50][z + 50] = steps[step].on;
                }
            }
        }
    }

    int nOn = 0;

    for (int x = 0; x < 101; ++x) {
        for (int y = 0; y < 101; ++y) {
            for (int z = 0; z < 101; ++z) {
                if (grid[x][y][z]) {
                    ++nOn;
                }
            }
        }
    }

    return nOn;
}

static int compareInt(const void *a, const void *b) {
    return *(const int32_t *)a - *(const int32_t *)b;
}

static inline uint32_t find(uint32_t n, const int32_t cs[n], int32_t c) {
    for (uint32_t i = 0; i < n; ++i) {
        if (cs[i] == c) {
            return i;
        }
    }
    assert(false && "Could not find coordinate");
}

// Inspired by https://www.youtube.com/watch?v=YKpViLcTp64, using cooridnate compression.
static int64_t partTwo(int n, const Step steps[n]) {
    int32_t xs[850];
    int32_t ys[850];
    int32_t zs[850];
    uint32_t m = 0;

    for (int i = 0; i < n; ++i) {
        Cuboid c = steps[i].cuboid;

        xs[m] = c.x1;
        ys[m] = c.y1;
        zs[m++] = c.z1;

        // Don't fully grasp why + 1.
        xs[m] = c.x2 + 1;
        ys[m] = c.y2 + 1;
        zs[m++] = c.z2 + 1;

        assert(m < 850);
    }

    qsort(xs, m, sizeof(xs[0]), compareInt);
    qsort(ys, m, sizeof(ys[0]), compareInt);
    qsort(zs, m, sizeof(zs[0]), compareInt);

    memset(grid, false, sizeof(grid));

    for (int i = 0; i < n; ++i) {
        Cuboid c = steps[i].cuboid;

        uint32_t ix1 = find(m, xs, c.x1);
        uint32_t iy1 = find(m, ys, c.y1);
        uint32_t iz1 = find(m, zs, c.z1);

        uint32_t ix2 = find(m, xs, c.x2 + 1);
        uint32_t iy2 = find(m, ys, c.y2 + 1);
        uint32_t iz2 = find(m, zs, c.z2 + 1);

        // Don't fully grasp why + 1 and < instead of no + 1 and <=.
        for (uint32_t iz = iz1; iz < iz2; ++iz) {
            for (uint32_t iy = iy1; iy < iy2; ++iy) {
                for (uint32_t ix = ix1; ix < ix2; ++ix) {
                    grid[iz][iy][ix] = steps[i].on;
                }
            }
        }
    }

    int64_t sumVolume = 0;

    for (uint32_t iz = 0; iz < m; ++iz) {
        for (uint32_t iy = 0; iy < m; ++iy) {
            for (uint32_t ix = 0; ix < m; ++ix) {
                if (grid[iz][iy][ix]) {
                    sumVolume += (int64_t)(xs[ix + 1] - xs[ix]) *
                                 (int64_t)(ys[iy + 1] - ys[iy]) *
                                 (int64_t)(zs[iz + 1] - zs[iz]);
                }
            }
        }
    }

    return sumVolume;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Step steps[CAP] = {0};
    int n = parse(input, steps);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, steps),
                   474140, 542711);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, steps),
                   2758514936282235, 1160303042684776);

    return 0;
}
