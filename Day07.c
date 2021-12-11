#include "Helpers.c"

#define CAP 4096

static int parse(const char *input, int xs[CAP]) {
    int charsRead = 0;
    int filled = 0;

    int x = 0;
    int n = 0;

    while ((filled = sscanf(input, "%d,%n", &x, &charsRead)) != EOF) {
        assert(filled == 1);

        xs[n++] = x;
        assert(n < CAP);

        input += charsRead;
    }

    assert(n > 0);
    return n;
}

static int compareInt(const void *a, const void *b) {
    return (*(const int *)a > *(const int *)b)
               ? 1
               : -1;
}

static int partOne(int n, const int xs[n]) {
    int sortedXs[n];
    memcpy(sortedXs, xs, n * sizeof(xs[0]));

    qsort(sortedXs, n, sizeof(xs[0]), compareInt); // O(n * log n)

    int fuels[n];
    memset(fuels, 0, n * sizeof(fuels[0]));

    int midpoint = sortedXs[n / 2]; // Midpoint is the median position
    int fuel = 0;

    for (int j = 0; j < n; ++j) { // O(n)
        fuel += abs(midpoint - sortedXs[j]);
    }

    return fuel;
}

static int partTwoFuelCost(int midpoint, int n, const int xs[n]) {
    int fuel = 0;

    for (int i = 0; i < n; ++i) { // O(n)
        int steps = abs(midpoint - xs[i]);

        fuel += (steps * (steps + 1)) / 2; // Formula for 1+2+3+n
    }

    return fuel;
}

static int partTwo(int n, const int xs[n]) {
    int xsSum = 0;

    for (int i = 0; i < n; ++i) { // O(n)
        xsSum += xs[i];
    }

    float average = (float)xsSum / n;

    int midpoint1 = (int)floor(average); // First midpoint is the low part of the avarage x
    int fuel1 = partTwoFuelCost(midpoint1, n, xs);

    int midpoint2 = (int)ceil(average); // Second midpoint is the high part of the avarage x
    int fuel2 = partTwoFuelCost(midpoint2, n, xs);

    int fuel = fuel1 < fuel2 ? fuel1 : fuel2;

    return fuel;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int xs[CAP] = {0};
    int n = parse(input, xs);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, xs),
                   37, 340056);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, xs),
                   168, 96592275);

    return 0;
}
