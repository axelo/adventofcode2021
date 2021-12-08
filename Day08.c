#define DAY 08
#define INPUT Entry
#define INPUT_CAP 200

typedef struct {
    int signals[10];
    int outputs[4];
} Entry;

#include "Runner.c"

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

    for (; *s != 0; ++s) {
        segments |= segmentFromChar(*s);
    }

    return segments;
}

static int parse(const char *inputString, Entry entries[INPUT_CAP]) {
    const char *inputStringEnd = inputString + strlen(inputString);
    int charsRead = 0;
    int n = 0;
    char segmentsString[8] = {0};

    for (; inputString < inputStringEnd; ++n) {
        for (int i = 0; i < 10; ++i) {
            int filled = sscanf(inputString, "%8[^ ]%n", segmentsString, &charsRead);
            assert(filled == 1);

            entries[n].signals[i] = segmentsFromString(segmentsString);

            inputString += charsRead + 1;

            assert(n < INPUT_CAP);
        }

        sscanf(inputString, "| %n", &charsRead);
        inputString += charsRead;

        for (int i = 0; i < 4; ++i) {
            int filled = sscanf(inputString, "%8[^ \n]%n", segmentsString, &charsRead);
            assert(filled == 1);

            entries[n].outputs[i] = segmentsFromString(segmentsString);

            inputString += charsRead + 1;
        }
    }

    assert(n > 0);
    return n;
}

static int countSegments(int segments) {
    int c = 0;

    for (; segments > 0; segments >>= 1) {
        c += segments & 1;
    }

    return c;
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

static Result partOne(int n, const Entry entries[n]) {
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

    return (Result){count, 26, 519};
}

static Result partTwo(int n, const Entry entries[n]) {
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

        // We known the two active segments are C and F, but not exactly which one
        int cAndF = decodedDigits[1];

        // We known the two active segments are B and D, but not exactly which one
        int bAndD = decodedDigits[4] & (~decodedDigits[1]);

        for (int j = 0; j < 10; ++j) {
            int signals = entries[i].signals[j];
            int activeSegments = countSegments(signals);

            if (activeSegments == 5) {
                // 2, 3 or 5
                if (countSegments(cAndF & signals) == 2) { // If c and f active then it's a 3
                    decodedDigits[3] = signals;
                } else if (countSegments(bAndD & signals) == 1) { // If b or d active then it's a 2
                    decodedDigits[2] = signals;
                } else { // Otherwise 5
                    decodedDigits[5] = signals;
                }
            } else if (activeSegments == 6) {
                // 0, 6 or 9
                if (countSegments(cAndF & signals) == 1) { // If c or f active then it's a 6
                    decodedDigits[6] = signals;
                } else if (countSegments(bAndD & signals) == 2) { // If b and d active then it's a 9
                    decodedDigits[9] = signals;
                } else { // Otherwise 0
                    decodedDigits[0] = signals;
                }
            }
        }

        sum += valueFromOutputs(decodedDigits, entries[i].outputs);
    }

    return (Result){sum, 61229, 1027483};
}
