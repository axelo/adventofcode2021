#include <stdbool.h> // bool
#include <stdlib.h>  // qsort

#define DAY 10
#define INPUT char input[INPUT_CAP][SYNTAX_CAP]
#define INPUT_CAP 100
#define SYNTAX_CAP 200

#include "Runner.c"

typedef struct {
    int position;
    char open[SYNTAX_CAP];
    int nOpen;
} SyntaxResult;

static int parse(const char *inputString, char lines[INPUT_CAP][SYNTAX_CAP]) {
    int charsRead;
    int n = 0;

    while (sscanf(inputString, "%s\n%n", lines[n], &charsRead) != EOF) {
        n++;
        assert(n < INPUT_CAP);
        inputString += charsRead;
    }

    return n;
}

static char openCharFromClosing(char c) {
    switch (c) {
    case ')': return '(';
    case ']': return '[';
    case '}': return '{';
    case '>': return '<';
    default: assert(false);
    }
}

static int scoreFromUnexpectedClosingChar(char c) {
    switch (c) {
    case ')': return 3;
    case ']': return 57;
    case '}': return 1197;
    case '>': return 25137;
    default: assert(false);
    }
}

static int scoreFromMissingClosingChar(char c) {
    switch (c) {
    case '(': return 1;
    case '[': return 2;
    case '{': return 3;
    case '<': return 4;
    default: assert(false);
    }
}

static void validateSyntax(const char *line, SyntaxResult *result) {
    const char *lineBegin = line;

    result->nOpen = 0;
    result->position = 0;

    for (char c = *line; c != 0; c = *++line) {
        switch (c) {
        case '(':
        case '[':
        case '{':
        case '<':
            result->open[result->nOpen++] = c;
            break;

        case ')':
        case ']':
        case '}':
        case '>':
            if (result->nOpen > 0 && result->open[result->nOpen - 1] == openCharFromClosing(c)) {
                --result->nOpen;
                break;
            } else {
                result->position = line - lineBegin;
                return;
            }
        }
    }
}

static Result partOne(int n, const char lines[n][SYNTAX_CAP]) {
    SyntaxResult result = {0};
    int score = 0;

    for (int i = 0; i < n; ++i) {
        validateSyntax(lines[i], &result);

        if (result.nOpen > 0 && result.position > 0) {
            score += scoreFromUnexpectedClosingChar(lines[i][result.position]);
        }
    }

    return (Result){score, 26397, 364389};
}

static int compareUInt64(const void *a, const void *b) {
    return (*(const uint64_t *)a > *(const uint64_t *)b)
               ? 1
               : -1;
}

static Result partTwo(int n, const char lines[n][SYNTAX_CAP]) {
    SyntaxResult result = {0};
    uint64_t scores[n];
    int nScores = 0;

    for (int i = 0; i < n; ++i) {
        validateSyntax(lines[i], &result);

        if (result.nOpen > 0 && result.position == 0) {
            uint64_t score = 0;

            for (int j = result.nOpen - 1; j >= 0; --j) {
                score = (5 * score) + scoreFromMissingClosingChar(result.open[j]);
            }

            scores[nScores++] = score;
        }
    }

    qsort(scores, nScores, sizeof(scores[0]), compareUInt64); // O(n * log n)

    return (Result){scores[nScores / 2], 288957, 2870201088};
}
