#include "Helpers.c"
#include <limits.h> // INT_MAX
#include <math.h>   // ceil, floor
#include <stdlib.h> // abs, qsort

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

        xs[n++] = x;
        assert(n < capacity);

        inputPtr += charsRead;
    }

    return n;
}

static int compareInt(const void *a, const void *b) {
    return (*(int *)a > *(int *)b)
               ? 1
               : -1;
}

static int partOne(int n, const int crabXS[n]) {
    int fuels[n];
    memset(fuels, 0, n * sizeof(fuels[0]));

    int xs[n];
    memcpy(xs, crabXS, sizeof(xs));

    qsort(xs, n, sizeof(xs[0]), compareInt); // O(n * log n)

    int midpoint = xs[n / 2]; // midpoint is the median position
    int fuel = 0;

    for (int j = 0; j < n; ++j) { // O(n)
        int srcX = xs[j];

        fuel += abs(midpoint - srcX);
    }

    return fuel;
}

static int partTwoFuelCost(int midpoint, int n, const int xs[n]) {
    int fuel = 0;

    for (int i = 0; i < n; ++i) { // O(n*n)
        int steps = abs(midpoint - xs[i]);

        for (int j = 1; j <= steps; ++j) {
            fuel += j;
        }
    }

    return fuel;
}

static int partTwo(int n, const int xs[n]) {
    int xsSum = 0;

    for (int i = 0; i < n; ++i) { // O(n)
        xsSum += xs[i];
    }

    int midpoint1 = (int)floor((float)xsSum / n); // First midpoint is the low part of the avarage x
    int fuel1 = partTwoFuelCost(midpoint1, n, xs);

    int midpoint2 = (int)ceil((float)xsSum / n); // Second midpoint is the high part of the avarage x
    int fuel2 = partTwoFuelCost(midpoint2, n, xs);

    return fuel1 < fuel2
               ? fuel1
               : fuel2;
}

int main() {
    int xs[4096] = {0};

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
