#include <limits.h>  // INT_MAX
#include <stdbool.h> // bool
#include <stdint.h>  // uint8_t

#define DAY 09
#define INPUT_CAP 102
#define INPUT uint8_t input[INPUT_CAP][INPUT_CAP]
#define N Dim

#define LOW_POINTS_CAP 256

typedef struct {
    int w;
    int h;
} Dim;

typedef struct {
    int x;
    int y;
} Point;

#include "Runner.c"

static Dim parse(const char *inputString, uint8_t heightmap[INPUT_CAP][INPUT_CAP]) {
    Dim dim = {0};

    for (; *inputString != 0; ++inputString) {
        int x = 0;

        for (char c = *inputString; c != 0 && c != '\n'; c = *++inputString) {
            int height = c - 48;
            assert(height >= 0 && height <= 9);

            heightmap[dim.h][x++] = height;
            assert(x < INPUT_CAP);
        }

        ++dim.h;
        assert(dim.h < INPUT_CAP);

        if (dim.w == 0) {
            dim.w = x;
        }

        assert(dim.w == x);
    }

    return dim;
}

static int findLowPoints(Dim dim, const uint8_t heightmap[INPUT_CAP][INPUT_CAP], Point points[LOW_POINTS_CAP]) {
    int n = 0;

    for (int r = 0; r < dim.h; ++r) {
        for (int c = 0; c < dim.w; ++c) {
            int hAbove = r - 1 < 0 ? INT_MAX : heightmap[r - 1][c];
            int hBelow = r + 1 >= dim.h ? INT_MAX : heightmap[r + 1][c];
            int hLeft = c - 1 < 0 ? INT_MAX : heightmap[r][c - 1];
            int hRight = c + 1 >= dim.w ? INT_MAX : heightmap[r][c + 1];
            int h = heightmap[r][c];

            if (h < hAbove && h < hBelow && h < hLeft && h < hRight) {
                points[n++] = (Point){.x = c, .y = r};

                assert(n < LOW_POINTS_CAP);
            }
        }
    }

    return n;
}

static Result partOne(Dim dim, const uint8_t heightmap[INPUT_CAP][INPUT_CAP]) {
    Point lowPoints[LOW_POINTS_CAP] = {0};
    int nLowPoints = findLowPoints(dim, heightmap, lowPoints);

    int sumRiskLevel = 0;

    for (int i = 0; i < nLowPoints; ++i) {
        int h = heightmap[lowPoints[i].y][lowPoints[i].x];
        sumRiskLevel += h + 1;
    }

    return (Result){sumRiskLevel, 15, 550};
}

static int fillBasin(Dim dim, const uint8_t heightmap[INPUT_CAP][INPUT_CAP], bool filled[INPUT_CAP][INPUT_CAP], int x, int y) {
    if (x < 0 || y < 0 || x >= dim.w || y >= dim.h || filled[y][x]) {
        return 0;
    }

    filled[y][x] = true;

    if (heightmap[y][x] == 9) {
        return 0;
    }

    return 1 +
           fillBasin(dim, heightmap, filled, x - 1, y) +
           fillBasin(dim, heightmap, filled, x + 1, y) +
           fillBasin(dim, heightmap, filled, x, y - 1) +
           fillBasin(dim, heightmap, filled, x, y + 1);
}

static Result partTwo(Dim dim, const uint8_t heightmap[INPUT_CAP][INPUT_CAP]) {
    Point lowPoints[LOW_POINTS_CAP] = {0};
    int nLowPoints = findLowPoints(dim, heightmap, lowPoints);

    bool filled[INPUT_CAP][INPUT_CAP] = {0};
    int basinSizeL = 0;
    int basinSizeXL = 0;
    int basinSizeXXL = 0;

    for (int i = 0; i < nLowPoints; ++i) {
        int basinSize = fillBasin(dim, heightmap, filled, lowPoints[i].x, lowPoints[i].y);

        if (basinSize > basinSizeXXL) {
            basinSizeL = basinSizeXL;
            basinSizeXL = basinSizeXXL;
            basinSizeXXL = basinSize;
        } else if (basinSize > basinSizeXL) {
            basinSizeL = basinSizeXL;
            basinSizeXL = basinSize;
        } else if (basinSize > basinSizeL) {
            basinSizeL = basinSize;
        }
    }

    return (Result){basinSizeL * basinSizeXL * basinSizeXXL,
                    1134, 1100682};
}