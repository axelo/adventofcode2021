#include "Helpers.c"

static void parse(const char *input, int64_t fish[9]) {
    int charsRead = 0;
    int filled = 0;
    int timer = 0;

    memset(fish, 0, 9 * sizeof(fish[0]));

    while ((filled = sscanf(input, "%d,%n", &timer, &charsRead)) != EOF) {
        assert(filled == 1);
        assert(timer >= 0 && timer <= 8);

        ++fish[timer];
        input += charsRead;
    }
}

static int64_t simulate(const int64_t initialFish[9], int days) {
    int64_t f[9] = {0};
    memcpy(f, initialFish, sizeof(f));

    for (int day = 0; day < days; ++day) {
        int64_t zero = f[0];
        f[0] = f[1];
        f[1] = f[2];
        f[2] = f[3];
        f[3] = f[4];
        f[4] = f[5];
        f[5] = f[6];
        f[6] = f[7] + zero;
        f[7] = f[8];
        f[8] = zero;
    }

    return f[0] + f[1] + f[2] + f[3] + f[4] + f[5] + f[6] + f[7] + f[8];
}

static int64_t partOne(const int64_t fish[9]) {
    return simulate(fish, 80);
}

static int64_t partTwo(const int64_t fish[9]) {
    return simulate(fish, 256);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int64_t fish[9] = {0};
    parse(input, fish);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(fish),
                   5934, 391671);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(fish),
                   26984457539, 1754000560399);

    return 0;
}
