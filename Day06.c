#include "Helpers.c"

static void parseInput(const char *filename, int64_t fish[9]) {
    char input[32 * 1024];
    readInput(filename, input, sizeof(input));

    char *inputPtr = input;
    int charsRead = 0;
    int filled = 0;
    int timer = 0;

    memset(fish, 0, 9 * sizeof(fish[0]));

    while ((filled = sscanf(inputPtr, "%d,%n", &timer, &charsRead)) != EOF) {
        assert(filled == 1);
        assert(timer >= 0 && timer <= 8);

        ++fish[timer];
        inputPtr += charsRead;
    }
}

static inline int64_t sumFish(int64_t f[9]) {
    return f[0] + f[1] + f[2] + f[3] + f[4] + f[5] + f[6] + f[7] + f[8];
}

static int64_t simulate(const int64_t initialFish[9], int days) {
    int64_t fish[9] = {0};
    memcpy(fish, initialFish, sizeof(fish));

    for (int day = 0; day < days; ++day) {
        int64_t zero = fish[0];
        fish[0] = fish[1];
        fish[1] = fish[2];
        fish[2] = fish[3];
        fish[3] = fish[4];
        fish[4] = fish[5];
        fish[5] = fish[6];
        fish[6] = fish[7] + zero;
        fish[7] = fish[8];
        fish[8] = zero;
    }

    return sumFish(fish);
}

static int64_t partOne(const int64_t f[9]) {
    return simulate(f, 80);
}

static int64_t partTwo(const int64_t f[9]) {
    return simulate(f, 256);
}

int main() {
    int64_t fish[9];
    parseInput("./Day06.txt", fish);

    int64_t partOneResult = partOne(fish);
    printf("Part one: %llu\n", partOneResult);
    // assert(partOneResult == 5934); // Example
    assert(partOneResult == 391671);

    (void)partTwo;
    int64_t partTwoResult = partTwo(fish);
    printf("Part two: %llu\n", partTwoResult);
    // assert(partTwoResult == 26984457539); // Example
    assert(partTwoResult == 1754000560399);

    return 0;
}
