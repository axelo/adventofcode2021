#include "Helpers.c"

#define CAP 500
#define CUBES_CAP 600000

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

typedef struct {
    int x;
    int y;
    int z;
} Cube;

bool on[200][200][200] = {false};

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

static Cuboid intersection(Cuboid a, Cuboid b) { // TODO.
    return (Cuboid){max(a.x1, b.x1),
                    min(a.x2, b.x2),
                    max(a.y1, b.y1),
                    min(a.y2, b.y2),
                    max(a.z1, b.z1),
                    min(a.z2, b.z2)};
}

static void dumpCuboid(Cuboid c) {
    printf("x: (%d,%d), y: (%d,%d), z: (%d,%d)\n", c.x1, c.x2, c.y1, c.y2, c.z1, c.z2);
}

static void turnOn(Cuboid cuboid) {
    int x1 = max(cuboid.x1, -50);
    int x2 = min(cuboid.x2, 50);

    int y1 = max(cuboid.y1, -50);
    int y2 = min(cuboid.y2, 50);

    int z1 = max(cuboid.z1, -50);
    int z2 = min(cuboid.z2, 50);

    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            for (int z = z1; z <= z2; ++z) {
                // if (x < -50 || y < -50 || z < -50) {
                //     continue;
                // }

                // if (x > 50 || y > 50 || z > 50) {
                //     continue;
                // }

                on[x + 50][y + 50][z + 50] = true;
            }
        }
    }
}

static void turnOff(Cuboid cuboid) {
    int x1 = max(cuboid.x1, -50);
    int x2 = min(cuboid.x2, 50);

    int y1 = max(cuboid.y1, -50);
    int y2 = min(cuboid.y2, 50);

    int z1 = max(cuboid.z1, -50);
    int z2 = min(cuboid.z2, 50);

    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            for (int z = z1; z <= z2; ++z) {
                on[x + 50][y + 50][z + 50] = false;
            }
        }
    }
}

static int partOne(int n, const Step steps[n]) {
    assert(n > 0);

    for (int step = 0; step < n; ++step) {
        printf("On step: %d\n", step + 1);
        steps[step].on
            ? turnOn(steps[step].cuboid)
            : turnOff(steps[step].cuboid);
    }

    int nOn = 0;
    for (int x = 0; x < 200; ++x) {
        for (int y = 0; y < 200; ++y) {
            for (int z = 0; z < 200; ++z) {
                if (on[x][y][z]) {
                    ++nOn;
                }
            }
        }
    }

    printf("nOn: %d\n", nOn);

    return nOn;
}

// 2758514936282235
// vs
// 11560337735628470 (all on sum)

static int partTwo(int n, const Step steps[n]) {
    uint64_t maxVolume = 0;
    uint64_t sumVolume = 0;

    for (int i = 0; i < n; ++i) {
        Cuboid c = steps[i].cuboid;

        uint64_t x = abs(c.x2 - c.x1);
        uint64_t y = abs(c.y2 - c.y1);
        uint64_t z = abs(c.z2 - c.z1);

        if (steps[i].on) {
            uint64_t volume = x * y * z;

            sumVolume += volume;

            if (volume > maxVolume)
                maxVolume = volume;
            printf("%i, volume: %llu\n", i + 1, volume);
        }
    }

    printf("max volume: %llu\n", maxVolume);
    printf("sum volume: %llu\n", sumVolume);

    return -1;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Step steps[CAP] = {0};
    int n = parse(input, steps);

    // Helpers_assert(PART1, Helpers_clock(),
    //                partOne(n, steps),
    //                590784, 542711);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, steps),
                   -1, -2);

    return 0;
}
