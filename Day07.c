#include "Helpers.c"
#include <limits.h> // INT_MAX
#include <stdlib.h> // abs

static int parseInput(const char *filename, int capacity, int xs[capacity]) {
    char input[32 * 1024];
    readInput(filename, input, sizeof(input));

    char *inputPtr = input;
    int charsRead = 0;
    int filled = 0;

    int x = 0;
    int n = 0;

    while ((filled = sscanf(inputPtr, "%d,%n", &x, &charsRead)) != EOF) {
        assert(filled == 1);
        assert(x < capacity);

        xs[n++] = x;

        inputPtr += charsRead;
    }

    return n;
}

static int partOne(int n, int xs[n]) {
    int fuels[n];
    memset(fuels, 0, n * sizeof(fuels[0]));

    int minFuel = INT_MAX;

    // O(n*n)
    for (int i = 0; i < n; ++i) {
        int destX = xs[i];

        for (int j = 0; j < n; ++j) {
            int srcX = xs[j];
            int deltaFuel = abs(destX - srcX);

            fuels[i] += deltaFuel;
        }

        if (fuels[i] < minFuel) {
            minFuel = fuels[i];
        }
    }

    return minFuel;
}

static int partTwo(int n, int xs[n]) {
    int maxX = xs[0];

    for (int i = 1; i < n; ++i) {
        if (xs[i] > maxX) {
            maxX = xs[i];
        }
    }

    int fuels[maxX];
    memset(fuels, 0, maxX * sizeof(fuels[0]));

    int minFuel = INT_MAX;

    // O(n*n*n)
    for (int destX = 0; destX <= maxX; ++destX) {
        for (int i = 0; i < n; ++i) {
            int srcX = xs[i];
            int steps = abs(destX - srcX);

            for (int j = 1; j <= steps; ++j) {
                fuels[destX] += j;
            }
        }

        if (fuels[destX] < minFuel) {
            minFuel = fuels[destX];
        }
    }

    return minFuel;
}

int main() {
    int xs[4096];
    memset(xs, -1, sizeof(xs));

    int n = parseInput("./Day07.txt", sizeof(xs) / sizeof(xs[0]), xs);
    assert(n > 0);

    int partOneResult = partOne(n, xs);
    printf("Part one: %d\n", partOneResult);
    // assert(partOneResult == 37); // Example
    assert(partOneResult == 340056);

    int partTwoResult = partTwo(n, xs);
    printf("Part two: %d\n", partTwoResult);
    // assert(partTwoResult == 168); // Example
    assert(partTwoResult == 96592275);
}
