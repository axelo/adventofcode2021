#include "Helpers.c"

typedef struct {
    uint64_t p1;
    uint64_t p2;
} Wins;

static void parse(const char *input, int *startP1, int *startP2) {
    assert(sscanf(input, "Player 1 starting position: %d\nPlayer 2 starting position: %d", startP1, startP2) == 2);
}

static int partOne(int start1, int start2) {
    int pos1 = start1 - 1; // Translate from 1...10 to 0..9.
    int pos2 = start2 - 1;
    int score1 = 0;
    int score2 = 0;
    int dice = 0;
    int rolls = 0;

    for (;;) {
        dice = dice == 100 ? 1 : dice + 1;
        pos1 = (pos1 + dice) % 10;

        dice = dice == 100 ? 1 : dice + 1;
        pos1 = (pos1 + dice) % 10;

        dice = dice == 100 ? 1 : dice + 1;
        pos1 = (pos1 + dice) % 10;

        rolls += 3;
        score1 += pos1 + 1;

        if (score1 >= 1000) {
            break;
        }

        dice = dice == 100 ? 1 : dice + 1;
        pos2 = (pos2 + dice) % 10;

        dice = dice == 100 ? 1 : dice + 1;
        pos2 = (pos2 + dice) % 10;

        dice = dice == 100 ? 1 : dice + 1;
        pos2 = (pos2 + dice) % 10;

        rolls += 3;
        score2 += pos2 + 1;

        if (score2 >= 1000) {
            break;
        }
    }

    return (score1 > score2 ? score2 : score1) * rolls;
}

static Wins playWithDirac(const int diceSumFreq[10], uint8_t score1, uint8_t score2, bool isP1Turn, uint8_t pos1, uint8_t pos2, Wins cachedWins[31][31][2][10][10]) {
    Wins cached = cachedWins[score1][score2][isP1Turn][pos1][pos2];

    if (cached.p1 > 0 || cached.p2 > 0) {
        return cached;
    } else if (score1 >= 21) {
        return (Wins){.p1 = 1, .p2 = 0};
    } else if (score2 >= 21) {
        return (Wins){.p1 = 00, .p2 = 1};
    } else {
        Wins totalWins = {0};

        for (int diceSum = 3; diceSum < 10; ++diceSum) {
            Wins universeWins;

            if (isP1Turn) {
                int newPos1 = (pos1 + diceSum) % 10;
                int newScore1 = score1 + newPos1 + 1;

                universeWins = playWithDirac(diceSumFreq, newScore1, score2, false, newPos1, pos2, cachedWins);
            } else {
                int newPos2 = (pos2 + diceSum) % 10;
                int newScore2 = score2 + newPos2 + 1;

                universeWins = playWithDirac(diceSumFreq, score1, newScore2, true, pos1, newPos2, cachedWins);
            }

            totalWins.p1 += universeWins.p1 * diceSumFreq[diceSum];
            totalWins.p2 += universeWins.p2 * diceSumFreq[diceSum];
        }

        // Remember this outcome so we can reuse it other universes.
        cachedWins[score1][score2][isP1Turn][pos1][pos2] = totalWins;

        return totalWins;
    }
}

static uint64_t partTwo(int startPos1, int startPos2) {
    int diracDiceSumFreq[10] = {0};

    for (int d1 = 1; d1 <= 3; ++d1) {
        for (int d2 = 1; d2 <= 3; ++d2) {
            for (int d3 = 1; d3 <= 3; ++d3) {
                ++diracDiceSumFreq[d1 + d2 + d3];
            }
        }
    }

    Wins cachedWins[31][31][2][10][10] = {0};

    // startPos is assumed 1..10, translate to 0..9.
    Wins totalWins = playWithDirac(diracDiceSumFreq, 0, 0, true, startPos1 - 1, startPos2 - 1, cachedWins);

    return totalWins.p1 > totalWins.p2
               ? totalWins.p1
               : totalWins.p2;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    int start1;
    int start2;
    parse(input, &start1, &start2);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(start1, start2),
                   739785, 1004670);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(start1, start2),
                   444356092776315, 492043106122795);

    return 0;
}
