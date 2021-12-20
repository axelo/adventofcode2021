#include "Helpers.c"

static int parseAlgorithm(const char *input, bool algorithm[512]) {
    char algoString[513] = {0};
    char firstInputRow[101] = {0};
    int charsRead = 0;

    assert(sscanf(input, "%512s\n\n%100s\n%n", algoString, firstInputRow, &charsRead) == 2);

    for (int i = 0; i < 512; ++i) {
        algorithm[i] = algoString[i] == '#';
    }

    return strlen(firstInputRow);
}

static void parseInputImage(const char *input, int n, bool image[n][n]) {
    input += 512 + 2; // Assume two new lines.

    char row[101] = {0};
    int charsRead = 0;

    for (int y = 0; y < n; ++y) {
        assert(sscanf(input, "%100s\n%n", row, &charsRead) == 1);
        input += charsRead;

        for (int x = 0; x < n; ++x) {
            image[y][x] = row[x] == '#' ? 1 : 0;
        }
    }
}

static int indexFromInput(int sx, int sy, int inSize, int size, const bool inputImage[size][size], bool infinityBit) {
    int index = 0;

    for (int y = sy - 1; y <= sy + 1; ++y) {
        for (int x = sx - 1; x <= sx + 1; ++x) {
            index <<= 1;
            index |= (x >= 0 && y >= 0 && x < inSize && y < inSize)
                         ? inputImage[y][x]
                         : infinityBit;
        }
    }

    return index;
}

static void enhance(const bool algorithm[512], int inSize, const bool inputImage[inSize][inSize], int outSize, bool outputImage[outSize][outSize]) {
    assert(outSize > inSize);

    int sizeBytes = sizeof(bool) * outSize * outSize;

    bool input[outSize][outSize];
    bool output[outSize][outSize];
    memset(output, false, sizeBytes);

    int infinityBit = 0;

    for (int y = 0; y < inSize + 2; ++y) {
        for (int x = 0; x < inSize + 2; ++x) {
            int index = indexFromInput(x - 1, y - 1, inSize, inSize, inputImage, infinityBit);
            output[y][x] = algorithm[index];
        }
    }

    for (inSize += 2; inSize < outSize; inSize += 2) {
        memcpy(input, output, sizeBytes);

        infinityBit = infinityBit ? algorithm[511] : algorithm[0]; // 0b111111111 = 511

        for (int y = 0; y < inSize + 2; ++y) {
            for (int x = 0; x < inSize + 2; ++x) {
                int index = indexFromInput(x - 1, y - 1, inSize, outSize, input, infinityBit);

                output[y][x] = algorithm[index];
            }
        }
    }

    memcpy(outputImage, output, sizeBytes);
}

static int countLitPixels(int n, const bool image[n][n]) {
    int count = 0;

    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            if (image[y][x]) {
                ++count;
            }
        }
    }

    return count;
}

static int sizeAfterPasses(int n, int passes) {
    return n + 2 * passes;
}

static int partOne(const bool algorithm[512], int n, const bool inputImage[n][n]) {
    int passes = 2;
    int size = sizeAfterPasses(n, passes);
    bool outputImage[size][size];

    enhance(algorithm, n, inputImage, size, outputImage);

    return countLitPixels(size, outputImage);
}

static int partTwo(const bool algorithm[512], int n, const bool inputImage[n][n]) {
    int passes = 50;
    int size = sizeAfterPasses(n, passes);
    bool outputImage[size][size];
    enhance(algorithm, n, inputImage, size, outputImage);

    return countLitPixels(size, outputImage);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    bool algorithm[512] = {0};
    int n = parseAlgorithm(input, algorithm);

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
