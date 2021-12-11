#include "Helpers.c"

#define CAP 4096

static int parse(const char *input, int depths[CAP]) {
    size_t n = 0;
    int charsRead = 0;
    int filled = 0;

    while ((filled = sscanf(input, "%u\n%n", &depths[n++], &charsRead)) != EOF) {
        assert(filled == 1 && "parseInput: Failed to parse input");

        input += charsRead;

        assert(n < CAP);
    }

    return n;
}

static int partOne(int n, const int depths[n]) {
    int count = 0;
    int previousDepth = 0;

    for (int i = 0; i < n; ++i) {
        int depth = depths[i];

        if (previousDepth > 0 && previousDepth < depth) {
            ++count;
        }

        previousDepth = depth;
    }

    return count;
}

static int partTwo(int n, const int depths[n]) {
    int count = 0;
    int previousSum = 0;

    for (int i = 0; i < (n / 3) * 3; ++i) {
        int sum = depths[i] + depths[i + 1] + depths[i + 2];

        if (previousSum > 0 && previousSum < sum) {
            ++count;
        }

        previousSum = sum;
    }

    return count;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int depths[CAP] = {0};
    int n = parse(input, depths);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, depths),
                   7, 1139);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, depths),
                   5, 1103);

    return 0;
}
