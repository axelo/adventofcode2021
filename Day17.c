#include "Helpers.c"

typedef struct {
    int x1;
    int y1;

    int x2;
    int y2;
} Area;

typedef struct {
    int hits;
    int maxY;
} LaunchResult;

static Area parse(const char *input) {
    Area area = {0};

    assert(sscanf(input, "target area: x=%d..%d, y=%d..%d", &area.x1, &area.x2, &area.y1, &area.y2) == 4);

    assert(area.x1 > 0 && area.x2 > 0);

    return area;
}

static LaunchResult launchProbe(Area target) {
    LaunchResult result = {0};

    int vxLowerBound = 0;
    int vxUpperBound = target.x1 + target.x2;
    int vyLowerBound = -(abs(target.y1) + abs(target.y2));
    int vyUpperBound = abs(target.y1) + abs(target.y2);

    for (int svx = vxLowerBound; svx < vxUpperBound; ++svx) {
        for (int svy = vyLowerBound; svy < vyUpperBound; ++svy) {
            int x = 0;
            int y = 0;

            int vx = svx;
            int vy = svy;

            int localMaxY = y;

            for (;;) {
                x += vx;
                y += vy;

                if (y > localMaxY) {
                    localMaxY = y;
                }

                if (vx > 0) {
                    --vx;
                } else if (vx < 0) {
                    ++vx;
                }

                --vy;

                if (x >= target.x1 && x <= target.x2 && y >= target.y1 && y <= target.y2) {
                    // Hit.
                    if (localMaxY > result.maxY) {
                        result.maxY = localMaxY;
                    }

                    ++result.hits;
                    break;
                }

                if ((vx > 0 && x > target.x2) || (vy < 0 && y < target.y1)) {
                    // Impossible to hit.
                    break;
                }
            }
        }
    }

    return result;
}

static int partOne(Area target) {
    return launchProbe(target).maxY;
}

static int partTwo(Area target) {
    return launchProbe(target).hits;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Area target = parse(input);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(target),
                   45, 6555);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(target),
                   112, 4973);

    return 0;
}
