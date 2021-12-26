#include "Helpers.c"

static uint32_t parseAlgorithm(const char *input, bool algorithm[512]) {
    char algoString[513] = {0};
    char firstInputRow[101] = {0};
    int charsRead = 0;

    assert(sscanf(input, "%512s\n\n%100s\n%n", algoString, firstInputRow, &charsRead) == 2);

    for (int i = 0; i < 512; ++i) {
        algorithm[i] = algoString[i] == '#';
    }

    return (uint32_t)strlen(firstInputRow);
}

static void parseInputImage(const char *input, uint32_t n, bool image[n][n]) {
    input += 512 + 2; // Assume two new lines.

    char row[101] = {0};
    int charsRead = 0;

    for (uint32_t y = 0; y < n; ++y) {
        assert(sscanf(input, "%100s\n%n", row, &charsRead) == 1);
        input += charsRead;

        for (uint32_t x = 0; x < n; ++x) {
            image[y][x] = row[x] == '#' ? 1 : 0;
        }
    }
}

static uint32_t indexFromInput(int sx, int sy, uint32_t inSize, uint32_t size, const bool inputImage[size][size], bool infinityBit) {
    uint32_t index = 0;

    for (int y = sy - 1; y <= sy + 1; ++y) {
        for (int x = sx - 1; x <= sx + 1; ++x) {
            index <<= 1;
            index |= (x >= 0 && y >= 0 && (uint32_t)x < inSize && (uint32_t)y < inSize)
                         ? inputImage[y][x]
                         : infinityBit;
        }
    }

    return index;
}

static void enhance(const bool algorithm[512], uint32_t inSize, const bool inputImage[inSize][inSize], uint32_t outSize, bool outputImage[outSize][outSize]) {
    assert(outSize > inSize);

    size_t sizeBytes = sizeof(bool) * outSize * outSize;

    bool input[outSize][outSize];
    bool output[outSize][outSize];
    memset(output, false, sizeBytes);

    int infinityBit = 0;

    for (int y = 0; y < (int)inSize + 2; ++y) {
        for (int x = 0; x < (int)inSize + 2; ++x) {
            uint32_t index = indexFromInput(x - 1, y - 1, inSize, inSize, inputImage, infinityBit);
            output[y][x] = algorithm[index];
        }
    }

    for (inSize += 2; inSize < outSize; inSize += 2) {
        memcpy(input, output, sizeBytes);

        infinityBit = infinityBit ? algorithm[511] : algorithm[0]; // 0b111111111 = 511, 0b000000000 = 0.

        for (int y = 0; y < (int)inSize + 2; ++y) {
            for (int x = 0; x < (int)inSize + 2; ++x) {
                uint32_t index = indexFromInput(x - 1, y - 1, inSize, outSize, input, infinityBit);
                output[y][x] = algorithm[index];
            }
        }
    }

    memcpy(outputImage, output, sizeBytes);
}

static uint32_t countLitPixels(uint32_t n, const bool image[n][n]) {
    uint32_t count = 0;

    for (uint32_t y = 0; y < n; ++y) {
        for (uint32_t x = 0; x < n; ++x) {
            if (image[y][x]) {
                ++count;
            }
        }
    }

    return count;
}

static uint32_t sizeAfterPasses(uint32_t n, uint32_t passes) {
    return n + 2 * passes;
}

static uint32_t partOne(const bool algorithm[512], uint32_t n, const bool inputImage[n][n]) {
    uint32_t size = sizeAfterPasses(n, 2);
    bool outputImage[size][size];

    enhance(algorithm, n, inputImage, size, outputImage);

    return countLitPixels(size, outputImage);
}

static uint32_t partTwo(const bool algorithm[512], uint32_t n, const bool inputImage[n][n]) {
    uint32_t size = sizeAfterPasses(n, 50);
    bool outputImage[size][size];
    enhance(algorithm, n, inputImage, size, outputImage);

    return countLitPixels(size, outputImage);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    bool algorithm[512] = {0};
    uint32_t n = parseAlgorithm(input, algorithm);

    bool inputImage[n][n];
    parseInputImage(input, n, inputImage);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(algorithm, n, inputImage),
                   35, 5359);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(algorithm, n, inputImage),
                   3351, 12333);

    return 0;
}
