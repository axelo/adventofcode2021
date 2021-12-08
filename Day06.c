#define DAY 06
#define INPUT int64_t
#define INPUT_CAP 9

#include "Runner.c"

static int parse(const char *inputString, int64_t fish[9]) {
    int charsRead = 0;
    int filled = 0;
    int timer = 0;

    memset(fish, 0, 9 * sizeof(fish[0]));

    while ((filled = sscanf(inputString, "%d,%n", &timer, &charsRead)) != EOF) {
        assert(filled == 1);
        assert(timer >= 0 && timer <= 8);

        ++fish[timer];
        inputString += charsRead;
    }

    return 9;
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

static Result solvePartOne(int n, const int64_t fish[n]) {
    assert(n == 9);
    return (Result){simulate(fish, 80), 5934, 391671};
}

static Result solvePartTwo(int n, const int64_t fish[n]) {
    assert(n == 9);
    return (Result){simulate(fish, 256), 26984457539, 1754000560399};
}
