#define DAY 01
#define INPUT int
#define INPUT_CAP 4096

#include "Runner.c"

static int parse(const char *inputString, int depths[INPUT_CAP]) {
    size_t n = 0;
    int charsRead = 0;
    int filled = 0;

    while ((filled = sscanf(inputString, "%u\n%n", &depths[n++], &charsRead)) != EOF) {
        assert(filled == 1 && "parseInput: Failed to parse input");

        inputString += charsRead;

        assert(n < INPUT_CAP);
    }

    return n;
}

static Result solvePartOne(int n, const int depths[n]) {
    int count = 0;
    int previousDepth = 0;

    for (int i = 0; i < n; ++i) {
        int depth = depths[i];

        if (previousDepth > 0 && previousDepth < depth) {
            ++count;
        }

        previousDepth = depth;
    }

    return (Result){count, 7, 1139};
}

static Result solvePartTwo(int n, const int depths[n]) {
    int count = 0;
    int previousSum = 0;

    for (int i = 0; i < (n / 3) * 3; ++i) {
        int sum = depths[i] + depths[i + 1] + depths[i + 2];

        if (previousSum > 0 && previousSum < sum) {
            ++count;
        }

        previousSum = sum;
    }

    return (Result){count, 5, 1103};
}
