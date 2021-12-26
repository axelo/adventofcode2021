#include "Helpers.c"

#define TEMPLATE_CAP 32

static int parse(const char *input, uint8_t template[TEMPLATE_CAP], uint8_t rules[26][26]) {
    int charsRead = 0;

    assert(sscanf(input, "%31s\n%n", template, &charsRead) != EOF);
    int n = charsRead - 2;
    input += charsRead;

    uint8_t p0 = 0;
    uint8_t p1 = 0;
    uint8_t r = 0;

    while (sscanf(input, "%c%c -> %c\n%n", &p0, &p1, &r, &charsRead) == 3) {
        assert(p0 <= 'Z' && p1 <= 'Z' && r <= 'Z');

        rules[p0 - 'A'][p1 - 'A'] = r - 'A';

        input += charsRead;
    }

    return n;
}

static int64_t pairFromTemplate(int n, const uint8_t template[n], const uint8_t rules[26][26], int steps) {
    // Allocate stack memory for all possible pair combinations for all steps.
    int64_t pairsByStep[steps + 1][26][26];
    memset(pairsByStep, 0, sizeof(int64_t) * 26 * 26 * (uint32_t)(steps + 1));

    // Insert initial pairs from template into step 0.
    for (int i = 0; i < n - 1; ++i) {
        ++pairsByStep[0][template[i] - 'A'][template[i + 1] - 'A'];
    }

    for (int step = 1; step <= steps; ++step) {
        // For every pair possibility, if pair exist from previous step, insert new pairs by rules.
        for (int p0 = 0; p0 < 26; ++p0) {
            for (int p1 = 0; p1 < 26; ++p1) {
                if (pairsByStep[step - 1][p0][p1] > 0) {
                    uint8_t insert = rules[p0][p1];
                    assert(insert);

                    // p0 and insert pair
                    pairsByStep[step][p0][insert] += pairsByStep[step - 1][p0][p1];

                    // insert and p1 pair
                    pairsByStep[step][insert][p1] += pairsByStep[step - 1][p0][p1];
                }
            }
        }
    }

    int64_t twiceCounts[26] = {0};

    // All but first and last character are counted twice due to insert rules.
    twiceCounts[template[0] - 'A'] = 1;
    twiceCounts[template[n - 1] - 'A'] = 1;

    for (int i = 0; i < 26; ++i) {
        for (int j = 0; j < 26; ++j) {
            twiceCounts[i] += pairsByStep[steps][i][j];
            twiceCounts[j] += pairsByStep[steps][i][j];
        }
    }

    int64_t min = INT64_MAX;
    int64_t max = 0;

    for (int i = 0; i < 26; ++i) {
        if (twiceCounts[i] > 0) {
            int64_t count = twiceCounts[i] / 2;
            min = count < min ? count : min;
            max = count > max ? count : max;
        }
    }

    return max - min;
}

static int64_t partOne(int n, const uint8_t template[n], const uint8_t rules[26][26]) {
    return pairFromTemplate(n, template, rules, 10);
}

static int64_t partTwo(int n, const uint8_t template[n], const uint8_t rules[26][26]) {
    return pairFromTemplate(n, template, rules, 40);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    uint8_t template[TEMPLATE_CAP] = {0};
    uint8_t rules[26][26] = {0};
    int n = parse(input, template, rules);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, template, rules),
                   1588, 2947);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, template, rules),
                   2188189693529, 3232426226464);

    return 0;
}
