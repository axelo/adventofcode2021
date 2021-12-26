#include "Helpers.c"

typedef struct {
    uint16_t w;
    uint16_t h;
} Size;

typedef enum {
    None = 0,
    East,
    South
} Herd;

static Size parse(const char *input, Herd map[150][150]) {
    char row[201];

    assert(sscanf(input, "%s200\n", row) == 1);

    uint16_t w = (uint16_t)strlen(row);
    uint16_t y = 0;

    int charsRead = 0;

    while (sscanf(input, "%200s\n%n", row, &charsRead) == 1) {
        input += charsRead;

        for (uint16_t x = 0; x < w; ++x) {
            map[y][x] =
                row[x] == '>'
                    ? East
                : row[x] == 'v'
                    ? South
                    : None;
        }

        y++;
    }

    return (Size){.w = w, .h = y};
}

static void dump(Size size, const Herd map[150][150]) {
    for (uint16_t y = 0; y < size.h; ++y) {
        for (uint16_t x = 0; x < size.w; ++x) {
            printf("%c",
                   map[y][x] == East
                       ? '>'
                   : map[y][x] == South
                       ? 'v'
                       : '.');
        }
        printf("\n");
    }
    printf("\n");
}

static void step(Size size, const Herd input[150][150], Herd output[150][150]) {
    memset(output, None, 150 * 150 * sizeof(Herd));

    for (int y = 0; y < size.h; ++y) {
        for (int x = 0; x < size.w; ++x) {
            if (input[y][x] == East) {
                int x2 = x + 1 >= size.w ? 0 : x + 1;
                if (input[y][x2] == None) {
                    output[y][x2] = East;
                } else {
                    output[y][x] = East;
                }
            }
        }
    }

    for (int y = 0; y < size.h; ++y) {
        for (int x = 0; x < size.w; ++x) {
            if (input[y][x] == South) {
                int y2 = y + 1 >= size.h ? 0 : y + 1;
                if (input[y2][x] == East && output[y2][x] == None) {
                    output[y2][x] = South;
                } else if (input[y2][x] == None && output[y2][x] == None) {
                    output[y2][x] = South;
                } else {
                    output[y][x] = South;
                }
            }
        }
    }
}

static bool equal(Size size, const Herd a[150][150], const Herd b[150][150]) {
    for (int y = 0; y < size.h; ++y) {
        for (int x = 0; x < size.w; ++x) {
            if (a[y][x] != b[y][x]) {
                return false;
            }
        }
    }

    return true;
}

static int64_t partOne(Size size, const Herd map[150][150]) {
    Herd map1[150][150];
    Herd map2[150][150];

    memcpy(map1, map, 150 * 150 * sizeof(Herd));

    printf("Initial:\n");
    dump(size, map);

    Herd(*input)[150][150] = &map1;
    Herd(*output)[150][150] = &map2;

    for (int s = 1;; ++s) {
        step(size, *input, *output);

        if (equal(size, *input, *output)) {
            return s;
        }

        if (input == &map1) { // Swap input/output.
            input = &map2;
            output = &map1;
        } else {
            input = &map1;
            output = &map2;
        }
    }

    return -1;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Herd map[150][150] = {0};
    Size size = parse(input, map);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(size, map),
                   58, 406);

    return 0;
}
