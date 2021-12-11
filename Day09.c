#include "Helpers.c"

#define CAP 102
#define LOW_POINTS_CAP 256

typedef struct {
    int w;
    int h;
} Dim;

typedef struct {
    int x;
    int y;
} Point;

static Dim parse(const char *input, uint8_t heightmap[CAP][CAP]) {
    Dim dim = {0};

    for (; *input != 0; ++input) {
        int x = 0;

        for (char c = *input; c != 0 && c != '\n'; c = *++input) {
            int height = c - 48;
            assert(height >= 0 && height <= 9);

            heightmap[dim.h][x++] = height;
            assert(x < CAP);
        }

        ++dim.h;
        assert(dim.h < CAP);

        if (dim.w == 0) {
            dim.w = x;
        }

        assert(dim.w == x);
    }

    return dim;
}

static int findLowPoints(Dim dim, const uint8_t heightmap[CAP][CAP], Point points[LOW_POINTS_CAP]) {
    int n = 0;

    for (int y = 0; y < dim.h; ++y) {
        for (int x = 0; x < dim.w; ++x) {
            uint8_t hAbove = y - 1 < 0 ? UINT8_MAX : heightmap[y - 1][x];
            uint8_t hBelow = y + 1 >= dim.h ? UINT8_MAX : heightmap[y + 1][x];
            uint8_t hLeft = x - 1 < 0 ? UINT8_MAX : heightmap[y][x - 1];
            uint8_t hRight = x + 1 >= dim.w ? UINT8_MAX : heightmap[y][x + 1];
            uint8_t h = heightmap[y][x];

            if (h < hAbove && h < hBelow && h < hLeft && h < hRight) {
                points[n++] = (Point){x, y};
                assert(n < LOW_POINTS_CAP);
            }
        }
    }

    return n;
}

static int partOne(Dim dim, const uint8_t heightmap[CAP][CAP]) {
    Point lowPoints[LOW_POINTS_CAP] = {0};
    int nLowPoints = findLowPoints(dim, heightmap, lowPoints);

    int sumRiskLevel = 0;

    for (int i = 0; i < nLowPoints; ++i) {
        sumRiskLevel += heightmap[lowPoints[i].y][lowPoints[i].x] + 1;
    }

    return sumRiskLevel;
}

static int fillBasin(Dim dim, const uint8_t heightmap[CAP][CAP], bool filled[CAP][CAP], int x, int y) {
    if (x < 0 || y < 0 || x >= dim.w || y >= dim.h || filled[y][x] || heightmap[y][x] == 9) {
        return 0;
    }

    filled[y][x] = true;

    return 1 +
           fillBasin(dim, heightmap, filled, x - 1, y) +
           fillBasin(dim, heightmap, filled, x + 1, y) +
           fillBasin(dim, heightmap, filled, x, y - 1) +
           fillBasin(dim, heightmap, filled, x, y + 1);
}

static int partTwo(Dim dim, const uint8_t heightmap[CAP][CAP]) {
    Point lowPoints[LOW_POINTS_CAP] = {0};
    int nLowPoints = findLowPoints(dim, heightmap, lowPoints);

    bool filled[CAP][CAP] = {0};
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

    return basinSizeL * basinSizeXL * basinSizeXXL;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    uint8_t heightmap[CAP][CAP] = {0};
    Dim dim = parse(input, heightmap);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(dim, heightmap),
                   15, 550);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(dim, heightmap),
                   1134, 1100682);

    return 0;
}
