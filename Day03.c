#include "Helpers.c"
#include <inttypes.h> // strtoimax
#include <stdbool.h>  // bool

typedef enum {
    OxygenGeneratorRating = 1,
    CO2ScrubberRating
} Rating;

static int parseInput(const char *filename, int *diags, size_t maxCount) {
    char input[32 * 1024] = {0};

    readInput(filename, input, sizeof(input));

    char *inputPtr = input;
    size_t count = 0;
    int charsRead = 0;
    int filled = 0;
    char diagString[32];

    while ((filled = sscanf(inputPtr, "%12s\n%n", diagString, &charsRead)) != EOF) {
        assert(filled == 1 && "parseInput: Failed to parse input");

        inputPtr += charsRead;

        diags[count++] = strtoimax(diagString, NULL, 2);

        assert(count < maxCount);
    }

    return count;
}

static int partOne(const int *diags, size_t count, uint8_t bitWidth) {
    int bitSums[bitWidth];

    memset(bitSums, 0, sizeof(bitSums[0]) * bitWidth);

    for (size_t i = 0; i < count; ++i) {
        int diag = diags[i];

        for (size_t j = 0; j < bitWidth; ++j) {
            (diag & (1 << j)) ? ++bitSums[j] : --bitSums[j];
        }
    }

    int gammaRate = 0;

    for (size_t i = 0; i < bitWidth; ++i) {
        gammaRate |= bitSums[i] > 0 ? (1 << i) : 0;
    }

    int epsilonRate = ~gammaRate & ((1 << bitWidth) - 1);

    return gammaRate * epsilonRate;
}

static int partTwoRating(const int *diags, size_t count, uint8_t bitWidth, Rating rating) {
    int filtered[count];
    int filteredCount = count;

    memcpy(filtered, diags, sizeof(*diags) * count);

    int bitMask = 1 << (bitWidth - 1);

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

static int partTwo(const int *diags, size_t count, uint8_t bitWidth) {
    return partTwoRating(diags, count, bitWidth, OxygenGeneratorRating) *
           partTwoRating(diags, count, bitWidth, CO2ScrubberRating);
}

int main() {
    int diags[4096] = {0};
    size_t count = parseInput("./Day03.txt", diags, sizeof(diags) / sizeof(int));

    int partOneResult = partOne(diags, count, 12);
    assert(partOneResult == 3633500);
    printf("Part one: %d\n", partOneResult);

    int partTwoResult = partTwo(diags, count, 12);
    assert(partTwoResult == 4550283);
    printf("Part two: %d\n", partTwoResult);
}
