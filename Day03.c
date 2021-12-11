#include "Helpers.c"

#define CAP 4096

#ifdef EXAMPLE
#define BIT_WIDTH 5
#else
#define BIT_WIDTH 12
#endif

typedef enum {
    OxygenGeneratorRating = 1,
    CO2ScrubberRating
} Rating;

static int parse(const char *input, int diags[CAP]) {
    int n = 0;
    int charsRead = 0;
    int filled = 0;
    char diagString[32];

    while ((filled = sscanf(input, "%12s\n%n", diagString, &charsRead)) != EOF) {
        assert(filled == 1 && "parseInput: Failed to parse input");

        input += charsRead;

        diags[n++] = strtoimax(diagString, NULL, 2);

        assert(n < CAP);
    }

    return n;
}

static int partOne(int n, const int diags[n]) {
    int bitSums[BIT_WIDTH] = {0};

    for (size_t i = 0; i < (size_t)n; ++i) {
        int diag = diags[i];

        for (size_t j = 0; j < BIT_WIDTH; ++j) {
            (diag & (1 << j)) ? ++bitSums[j] : --bitSums[j];
        }
    }

    int gammaRate = 0;

    for (size_t i = 0; i < BIT_WIDTH; ++i) {
        gammaRate |= bitSums[i] > 0 ? (1 << i) : 0;
    }

    int epsilonRate = ~gammaRate & ((1 << BIT_WIDTH) - 1);

    return gammaRate * epsilonRate;
}

static int partTwoRating(int n, const int diags[n], Rating rating) {
    int filtered[n];
    int filteredCount = n;

    memcpy(filtered, diags, sizeof(*diags) * n);

    int bitMask = 1 << (BIT_WIDTH - 1);

    while (filteredCount > 1 && bitMask > 0) {
        int bitSum = 0;

        for (int i = 0; i < filteredCount; ++i) {
            (filtered[i] & bitMask) ? ++bitSum : --bitSum;
        }

        int j = 0;

        for (int i = 0; i < filteredCount; ++i) {
            bool isBitSet = filtered[i] & bitMask;

            bool bitSetCondition = (rating == OxygenGeneratorRating && isBitSet) ||
                                   (rating == CO2ScrubberRating && !isBitSet);

            if (((bitSum > 0 && bitSetCondition) ||
                 (bitSum < 0 && !bitSetCondition) ||
                 (bitSum == 0 && bitSetCondition))) {

                filtered[j++] = filtered[i];
            }
        }

        filteredCount = j;

        bitMask >>= 1;
    }

    assert(filteredCount == 1);

    return filtered[0];
}

static int partTwo(int n, const int diags[n]) {
    return partTwoRating(n, diags, OxygenGeneratorRating) * partTwoRating(n, diags, CO2ScrubberRating);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int diags[CAP] = {0};
    int n = parse(input, diags);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, diags),
                   198, 3633500);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, diags),
                   230, 4550283);

    return 0;
}
