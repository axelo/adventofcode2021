#define DAY 04
#define INPUT Input
#define INPUT_CAP 1

#include <stdbool.h>

typedef struct {
    int number;
    bool marked;
} Number;

typedef Number Board[5][5];

typedef struct {
    int firstWinningNumber;
    Board firstWinningBoard;

    int lastWinningNumber;
    Board lastWinningBoard;
} Bingo;

typedef struct {
    int numbers[256];
    int numbersCount;

    Board boards[256];
    int boardsCount;
} Input;

#include "Runner.c"

static int parse(const char *inputString, Input input[INPUT_CAP]) {
    int charsRead = 0;
    int filled = 0;

    char numbersString[1024] = {0};

    assert(sscanf(inputString, "%1024s\n\n%n", numbersString, &charsRead) == 1);
    assert((size_t)charsRead < sizeof(numbersString));

    char *numbersStringPtr = numbersString;

    while ((filled = sscanf(numbersStringPtr, "%u,%n", input->numbers + input->numbersCount++, &charsRead)) == 1) {
        numbersStringPtr += charsRead;
    }

    assert(input->numbersCount > 0);

    const char *rawInputBoards = inputString + (numbersStringPtr - numbersString);

    for (filled = 0; filled != EOF; ++input->boardsCount) {
        Board *board = &input->boards[input->boardsCount];

        for (int col = 0; col < 5 && filled != EOF; ++col) {
            filled = sscanf(rawInputBoards, "%u %u %u %u %u\n%n",
                            &(*board)[col][0].number,
                            &(*board)[col][1].number,
                            &(*board)[col][2].number,
                            &(*board)[col][3].number,
                            &(*board)[col][4].number,
                            &charsRead);

            rawInputBoards += charsRead;
        }

        if (filled == EOF) {
            --input->boardsCount;
        }
    }

    assert(input->boardsCount > 0);

    return 1;
}

static void playBingo(Input *input, Bingo *bingo) {
    bool firstWin = false;

    bool alreadyWonBoards[input->boardsCount];
    memset(alreadyWonBoards, false, input->boardsCount * sizeof(bool));

    for (int i = 0; i < input->numbersCount; ++i) {
        int number = input->numbers[i];

        for (int b = 0; b < input->boardsCount; ++b) {
            Board *board = input->boards + b;

            for (int col = 0; col < 5; ++col) {
                for (int row = 0; row < 5; ++row) {
                    if ((*board)[col][row].number == number) {
                        (*board)[col][row].marked = true;
                    }
                }
            }

            for (int col = 0; col < 5; ++col) {
                int rowMarks = 0;
                int colMarks = 0;

                for (int row = 0; row < 5; ++row) {
                    if ((*board)[col][row].marked) {
                        ++rowMarks;
                    }

                    if ((*board)[row][col].marked) {
                        ++colMarks;
                    }
                }

                if (rowMarks == 5 || colMarks == 5) {
                    if (!firstWin) {
                        firstWin = true;
                        bingo->firstWinningNumber = number;
                        memcpy(bingo->firstWinningBoard, board, sizeof(Board));
                    }

                    if (!alreadyWonBoards[b]) {
                        alreadyWonBoards[b] = true;
                        bingo->lastWinningNumber = number;
                        memcpy(bingo->lastWinningBoard, board, sizeof(Board));
                    }
                }
            }
        }
    }
}

static int sumUnmarked(const Board *board) {
    int sum = 0;

    for (int col = 0; col < 5; ++col) {
        for (int row = 0; row < 5; ++row) {
            if (!(*board)[col][row].marked) {
                sum += (*board)[col][row].number;
            }
        }
    }

    return sum;
}

static Result solvePartOne(int n, const Input input[n]) {
    assert(n == 1);

    Input inputCopy = {0};
    memcpy(&inputCopy, input, sizeof(inputCopy));

    Bingo bingo = {0};
    playBingo(&inputCopy, &bingo);

    return (Result){sumUnmarked(&bingo.firstWinningBoard) * bingo.firstWinningNumber, 4512, 21607};
}

static Result solvePartTwo(int n, const Input input[n]) {
    assert(n == 1);

    Input inputCopy = {0};
    memcpy(&inputCopy, input, sizeof(inputCopy));

    Bingo bingo = {0};
    playBingo(&inputCopy, &bingo);

    return (Result){sumUnmarked(&bingo.lastWinningBoard) * bingo.lastWinningNumber, 1924, 19012};
}
