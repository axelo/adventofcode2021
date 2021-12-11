#include "Helpers.c"

#define CAP 100

static int parse(const char *input, int xs[CAP]) {
    return 0;
}

static int partOne(int n, const int xs[CAP]) {
    return -1;
}

static int partTwo(int n, const int xs[CAP]) {
    return -1;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int xs[CAP] = {0};
    int n = parse(input, xs);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, xs),
                   -1, -2);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, xs),
                   -1, -2);

    return 0;
}
