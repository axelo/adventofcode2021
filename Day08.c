#include "Helpers.c"

typedef struct {
    int signals[10];
    int outputs[4];
} Entry;

#define ENTRY_CAP 200

static int segmentFromChar(char c) {
    switch (c) {
    case 'a': return 1;
    case 'b': return 2;
    case 'c': return 4;
    case 'd': return 8;
    case 'e': return 16;
    case 'f': return 32;
    case 'g': return 64;
    default:
        assert(0);
    }
}

static int segmentsFromString(const char *s) {
    int segments = 0;

    while (*s != 0) {
        segments |= segmentFromChar(*s++);
    }

    return segments;
}

static int countSegments(int segments) {
    int c = 0;

    while (segments > 0) {
        c += segments & 1;
        segments >>= 1;
    }

    return c;
}

static int parseInput(const char *filename, Entry entries[ENTRY_CAP]) {
    char input[32 * 1024];
    readInput(filename, input, sizeof(input));

    char *inputPtr = input;
    char *inputEndPtr = inputPtr + strlen(input);
    int charsRead = 0;
    int n = 0;
    char segmentsString[8] = {0};

    for (; inputPtr < inputEndPtr; ++n) {
        for (int i = 0; i < 10; ++i) {
            int filled = sscanf(inputPtr, "%8[^ ]%n", segmentsString, &charsRead);
            assert(filled == 1);

            entries[n].signals[i] = segmentsFromString(segmentsString);

            inputPtr += charsRead + 1;
        }

        sscanf(inputPtr, "| %n", &charsRead);
        inputPtr += charsRead;

        for (int i = 0; i < 4; ++i) {
            int filled = sscanf(inputPtr, "%8[^ \n]%n", segmentsString, &charsRead);
            assert(filled == 1);

            entries[n].outputs[i] = segmentsFromString(segmentsString);

            inputPtr += charsRead + 1;
        }
    }

    return n;
}

static int partOne(int n, const Entry entries[n]) {
    int count = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 4; ++j) {
            switch (countSegments(entries[i].outputs[j])) {
            case 2: // 1
            case 3: // 7
            case 4: // 4
            case 7: // 8
                ++count;
                break;
            }
        }
    }

    return count;
}

static int valueFromOutputs(const int decodedDigits[10], const int outputs[4]) {
    int value = 0;

    for (int i = 0, base = 1000; i < 4; ++i, base /= 10) {
        for (int j = 0; j < 10; ++j) {
            if (outputs[i] == decodedDigits[j]) {
                value += j * base;
                break;
            }
        }
    }

    return value;
}

static int partTwo(int n, const Entry entries[n]) {
    int sum = 0;

    for (int i = 0; i < n; ++i) {
        int decodedDigits[10] = {0};

        for (int j = 0; j < 10; ++j) {
            int signals = entries[i].signals[j];
            int activeSegments = countSegments(signals);

            if (activeSegments == 2) {
                decodedDigits[1] = signals;
            } else if (activeSegments == 3) {
                decodedDigits[7] = signals;
            } else if (activeSegments == 4) {
                decodedDigits[4] = signals;
            } else if (activeSegments == 7) {
                decodedDigits[8] = signals;
            }
        }

        int cOrF = decodedDigits[1];
        int bOrD = decodedDigits[4] & (~decodedDigits[1]);

        for (int j = 0; j < 10; ++j) {
            int signals = entries[i].signals[j];
            int activeSegments = countSegments(signals);

            if (activeSegments == 5) {
                // 2, 3 or 5
                if (countSegments(cOrF & signals) == 2) { // If c and f set then it's a 3
                    decodedDigits[3] = signals;
                } else if (countSegments(bOrD & signals) == 1) { // If b or d set then it's a 2
                    decodedDigits[2] = signals;
                } else { // Otherwise 5
                    decodedDigits[5] = signals;
                }
            } else if (activeSegments == 6) {
                // 0, 6 or 9
                if (countSegments(cOrF & signals) == 1) { // If c or f set then it's a 6
                    decodedDigits[6] = signals;
                } else if (countSegments(bOrD & signals) == 2) { // If b and d set then it's a 9
                    decodedDigits[9] = signals;
                } else { // Otherwise 0
                    decodedDigits[0] = signals;
                }
            }
        }

        sum += valueFromOutputs(decodedDigits, entries[i].outputs);
    }

    return sum;
}

int main() {
    Entry entries[ENTRY_CAP] = {0};

    int n = parseInput("./Day08.txt", entries);
    assert(n > 0);

    int partOneResult = partOne(n, entries);
    printf("Part one: %d\n", partOneResult);
    // assert(partOneResult == 26); // Example
    assert(partOneResult == 519);

    int partTwoResult = partTwo(n, entries);
    printf("Part two: %d\n", partTwoResult);
    // assert(partTwoResult == 61229); // Example
    assert(partTwoResult == 1027483);
}
