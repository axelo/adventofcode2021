#include "Helpers.c"

static int parseInput(const char *filename, int *depths, size_t maxCount) {
    char input[32 * 1024] = {0};

    readInput(filename, input, sizeof(input));

    char *inputPtr = input;
    size_t depthCount = 0;
    int charsRead = 0;

    while (sscanf(inputPtr, "%u\n%n", &depths[depthCount++], &charsRead) == 1) {
        inputPtr += charsRead;

        assert(depthCount < maxCount);
    }

    return depthCount;
}

static int partOne(const int *depths, size_t depthCount) {
    int count = 0;
    int previousDepth = 0;

    for (size_t i = 0; i < depthCount; ++i) {
        int depth = depths[i];

        if (previousDepth > 0 && previousDepth < depth) {
            ++count;
        }

        previousDepth = depth;
    }

    return count;
}

static int partTwo(const int *depths, size_t depthCount) {
    int count = 0;
    int previousSum = 0;

    for (size_t i = 0; i < (depthCount / 3) * 3; ++i) {
        int sum = depths[i] + depths[i + 1] + depths[i + 2];

        if (previousSum > 0 && previousSum < sum) {
            ++count;
        }

        previousSum = sum;
    }

    return count;
}

int main() {
    int depths[4096] = {0};
    size_t depthCount = parseInput("./Day1.txt", depths, sizeof(depths) / sizeof(int));

    int partOneResult = partOne(depths, depthCount);
    assert(partOneResult == 1139);
    printf("Part one: %d\n", partOneResult);

    int partTwoResult = partTwo(depths, depthCount);
    assert(partTwoResult == 1103);
    printf("Part two: %d\n", partTwoResult);
}
