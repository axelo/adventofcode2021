#include "Helpers.c"

#define TEMPLATE_CAP 32
#define RULES_CAP 100
#define PAIRS_CAP 100

typedef struct {
    uint8_t p0;
    uint8_t p1;
    int64_t n;
} Pair;

static int parse(const char *input, uint8_t template[TEMPLATE_CAP], uint8_t rules[RULES_CAP][RULES_CAP]) {
    int charsRead = 0;

    assert(sscanf(input, "%31s\n%n", template, &charsRead) != EOF);

    input += charsRead;

    uint8_t p0 = 0;
    uint8_t p1 = 0;
    uint8_t r = 0;

    while (sscanf(input, "%c%c -> %c\n%n", &p0, &p1, &r, &charsRead) == 3) {
        assert(p0 <= 'Z' && p1 <= 'Z' && r <= 'Z');

        rules[p0][p1] = r;

        input += charsRead;
    }

    return strlen((char *)template);
}

static Pair *findPair(int nPairs, Pair pairs[nPairs], char p0, char p1) {
    for (int i = 0; i < nPairs; ++i) {
        if (pairs[i].p0 == p0 && pairs[i].p1 == p1) {
            return &pairs[i];
        }
    }
    return NULL;
}

static int insertPair(int nPairs, Pair pairs[nPairs], uint8_t p0, uint8_t p1, int64_t n) {
    Pair *existingPair = findPair(nPairs, pairs, p0, p1);

    if (existingPair == NULL) {
        pairs[nPairs++] = (Pair){p0, p1, n};
        assert(nPairs < PAIRS_CAP);
    } else {
        existingPair->n += n;
    }

    return nPairs;
}

static int64_t pairFromTemplate(int n, const uint8_t template[n], const uint8_t rules[RULES_CAP][RULES_CAP], int steps) {
    Pair pairs1[PAIRS_CAP] = {0};
    Pair pairs2[PAIRS_CAP] = {0};

    Pair *prevPairs = pairs1;
    Pair *nextPairs = pairs2;
    int nPrev = 0;
    int nNext = 0;

    // Insert initial pairs from template into nextPairs.
    for (int i = 0; i < n - 1; ++i) {
        nNext = insertPair(nNext, nextPairs, template[i], template[i + 1], 1);
    }

    for (int step = 0; step < steps; ++step) {
        // Flip nextPairs pointer with prevPairs.
        if (prevPairs == pairs2) {
            prevPairs = pairs1;
            nextPairs = pairs2;
        } else {
            prevPairs = pairs2;
            nextPairs = pairs1;
        }

        nPrev = nNext;
        nNext = 0;

        // Consider every previous pair and insert new pairs based on rules.
        // Pass along the previous pair count on insertion as we will do the "same" insertion for every pair count.
        for (int i = 0; i < nPrev; ++i) {
            uint8_t insert = rules[prevPairs[i].p0][prevPairs[i].p1];

            if (insert) {
                nNext = insertPair(nNext, nextPairs, prevPairs[i].p0, insert, prevPairs[i].n);
                nNext = insertPair(nNext, nextPairs, insert, prevPairs[i].p1, prevPairs[i].n);
            } else {
                nNext = insertPair(nNext, nextPairs, prevPairs[i].p0, prevPairs[i].p1, prevPairs[i].n);
            }
        }
    }

    int64_t counts[RULES_CAP] = {0};

    // Include last character in the count as it isn't counted twice when starting from first char in pair.
    counts[template[n - 1]] = 1;

    // Only count first character of the pair as we have doubled them.
    for (int i = 0; i < nNext; ++i) {
        counts[nextPairs[i].p0] += nextPairs[i].n;
    }

    int64_t max = 0;
    int64_t min = INT64_MAX;

    for (int i = 0; i < RULES_CAP; ++i) {
        max = counts[i] > max ? counts[i] : max;
        min = counts[i] > 0 && counts[i] < min ? counts[i] : min;
    }

    return max - min;
}

static int64_t partOne(int n, const uint8_t template[n], const uint8_t rules[RULES_CAP][RULES_CAP]) {
    return pairFromTemplate(n, template, rules, 10);
}

static int64_t partTwo(int n, const uint8_t template[n], const uint8_t rules[RULES_CAP][RULES_CAP]) {
    return pairFromTemplate(n, template, rules, 40);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    uint8_t template[TEMPLATE_CAP] = {0};
    uint8_t rules[RULES_CAP][RULES_CAP] = {0};
    int n = parse(input, template, rules);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, template, rules),
                   1588, 2947);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, template, rules),
                   2188189693529, 3232426226464);

    return 0;
}
