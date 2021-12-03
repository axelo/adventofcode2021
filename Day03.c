#include "Helpers.c"
#include <inttypes.h> // strtoimax
#include <stdbool.h>  // bool

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
    int bitCounts[8 * sizeof(int)] = {0};

    assert(bitWidth <= sizeof(bitCounts) / sizeof(bitCounts[0]));

    for (size_t i = 0; i < count; ++i) {
        int diag = diags[i];

        for (size_t j = 0; j < bitWidth; ++j) {
            int bit = diag & (1 << j);

            bit ? ++bitCounts[j] : --bitCounts[j];
        }
    }

    int gammaRate = 0;

    for (size_t i = 0; i < bitWidth; ++i) {
        gammaRate |= bitCounts[i] > 0 ? (1 << i) : 0;
    }

    int epsilonRate = ~gammaRate & ((1 << bitWidth) - 1);

    return gammaRate * epsilonRate;
}

void binprintf(int v, uint8_t bitWidth) {
    unsigned int mask = 1 << (bitWidth - 1);
    while (mask) {
        printf("%d", (v & mask ? 1 : 0));
        mask >>= 1;
    }
    printf("\n");
}

static int partTwoOxygenGeneratorRating(const int *diags, size_t count, uint8_t bitWidth, bool co2Rating) {
    int filtered[count];
    int filteredCount = count;

    memcpy(filtered, diags, sizeof(*diags) * count);

    int bitMask = 1 << (bitWidth - 1);

    while (filteredCount > 1) {
        int bitSum = 0;

        for (int i = 0; i < filteredCount; ++i) {
            // binprintf(filtered[i], bitWidth);

            (filtered[i] & bitMask) ? ++bitSum : --bitSum;
        }

        int j = 0;

        for (int i = 0; i < filteredCount; ++i) {
            int isBitSet = filtered[i] & bitMask;

            if ((bitSum > 0 && isBitSet) ||
                (bitSum < 0 && !isBitSet) ||
                (bitSum == 0 && isBitSet)) {

                filtered[j++] = filtered[i];
            }
        }

        filteredCount = j;

        // printf("mask: %d - most ones: %d - numbers left: %d\n", bitMask, bitSum, filteredCount);

        bitMask >>= 1;
    }

    assert(filteredCount == 1);

    return filtered[0];
}

static int partTwoO2ScrubberRating(const int *diags, size_t count, uint8_t bitWidth, bool co2Rating) {
    int filtered[count];
    int filteredCount = count;

    memcpy(filtered, diags, sizeof(*diags) * count);

    int bitMask = 1 << (bitWidth - 1);

    while (filteredCount > 1) {
        int bitSum = 0;

        for (int i = 0; i < filteredCount; ++i) {
            // binprintf(filtered[i], bitWidth);

            (filtered[i] & bitMask) ? ++bitSum : --bitSum;
        }

        int j = 0;

        for (int i = 0; i < filteredCount; ++i) {
            int isBitSet = filtered[i] & bitMask;

            if ((bitSum > 0 && !isBitSet) ||
                (bitSum < 0 && isBitSet) ||
                (bitSum == 0 && !isBitSet)) {

                filtered[j++] = filtered[i];
            }
        }

        filteredCount = j;

        // printf("mask: %d - most ones: %d - numbers left: %d\n", bitMask, bitSum, filteredCount);

        bitMask >>= 1;
    }

    assert(filteredCount == 1);

    return filtered[0];
}

static int partTwo(const int *diags, size_t count, uint8_t bitWidth) {
    int oxygenGeneratorRating = partTwoOxygenGeneratorRating(diags, count, bitWidth);
    int co2ScrubberRating = partTwoO2ScrubberRating(diags, count, bitWidth, true);

    return oxygenGeneratorRating * co2ScrubberRating;
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
