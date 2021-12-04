#include "Helpers.c"
#include <stdbool.h>

// typedef int Board[5][5];

typedef struct {
    int number;
    bool marked;
} Number;

typedef Number Board[5][5];

typedef struct {
    int firstWinningNumber;
    Board firstWinningBoard;
    // Board winningBoard;
} Bingo;

typedef struct {
    int numbers[1024];
    int numbersCount;

    Board boards[512];
    int boardsCount;
} Input;

static void printBoard(Board *board) {
    for (int col = 0; col < 5; ++col) {
        printf("%u %u %u %u %u\n",
               (*board)[col][0].number,
               (*board)[col][1].number,
               (*board)[col][2].number,
               (*board)[col][3].number,
               (*board)[col][4].number);
    }
    printf("\n");
}

static void parseInput(const char *filename, Input *input) {
    char rawInput[32 * 1024] = {0};

    readInput(filename, rawInput, sizeof(rawInput));

    int charsRead = 0;
    int filled = 0;

    char numbersString[1024] = {0};

    assert(sscanf(rawInput, "%1024s\n\n%n", numbersString, &charsRead) == 1);
    assert((size_t)charsRead < sizeof(numbersString));

    char *numbersStringPtr = numbersString;

    while ((filled = sscanf(numbersStringPtr, "%u,%n", input->numbers + input->numbersCount++, &charsRead)) == 1) {
        numbersStringPtr += charsRead;
    }

    assert(input->numbersCount > 0);

    // for (size_t i = 0; i < input->numbersCount; ++i) {
    //     printf("%u\n", input->numbers[i]);
    // }

    char *rawInputBoards = rawInput + (numbersStringPtr - numbersString);

    // printf("BOARDS: %s\n", rawInputBoards);

    for (filled = 0; filled != EOF; ++input->boardsCount) {
        Board *board = &input->boards[input->boardsCount];

        // printf("\n\n");

        for (int col = 0; col < 5 && filled != EOF; ++col) {
            int a, b, c, d, e;

            filled = sscanf(rawInputBoards, "%u %u %u %u %u\n%n", &a, &b, &c, &d, &e, &charsRead);

            if (filled == 5) {
                (*board)[col][0].number = a;
                (*board)[col][1].number = b;
                (*board)[col][2].number = c;
                (*board)[col][3].number = d;
                (*board)[col][4].number = e;
                // printf("r: %d %d %d %d %d\n", a, b, c, d, e);
            }
            // printf("F: %d\n", f);

            rawInputBoards += charsRead;
        }

        if (filled == EOF) {
            --input->boardsCount;
        }
    }

    assert(input->boardsCount > 0);

    printf("Parsed boards:\n");
    for (int k = 0; k < input->boardsCount; ++k) {
        printBoard(&input->boards[k]);
    }
}

static Bingo partOnePlayBingo(Input *input) {
    Bingo result = {0};

    // printBoard(&bingoResult.winningBoard);

    for (int i = 0; i < input->numbersCount; ++i) {
        int number = input->numbers[i];

        // printf("Number: %d\n", number);

        for (int b = 0; b < input->boardsCount; ++b) {
            Board *board = &input->boards[b];

            for (int col = 0; col < 5; ++col) {
                for (int row = 0; row < 5; ++row) {
                    if ((*board)[col][row].number == number) {
                        (*board)[col][row].marked = true;
                    }
                }
            }

            for (int col = 0; col < 5; ++col) {
                int rowMarks = 0;

                for (int row = 0; row < 5; ++row) {
                    if ((*board)[col][row].marked) {
                        ++rowMarks;
                    }
                }

                if (rowMarks == 5) {
                    printf("5 in a row for board %d (%d)\n", b + 1, number);

                    printBoard(board);

                    result.firstWinningNumber = number;
                    memcpy(&result.firstWinningBoard, board, sizeof(Board));

                    return result;
                }
            }

            for (int row = 0; row < 5; ++row) {
                int colMarks = 0;

                for (int col = 0; col < 5; ++col) {
                    if ((*board)[col][row].marked) {
                        ++colMarks;
                    }
                }

                if (colMarks == 5) {
                    printf("5 in a column for board %d (%d)\n", b + 1, number);

                    result.firstWinningNumber = number;
                    memcpy(&result.firstWinningBoard, board, sizeof(Board));

                    return result;
                }
            }
        }
    }

    return result;
}

static int partOne(Input *input) {
    Bingo bingo = partOnePlayBingo(input);

    printf("Last called number: %u\n", bingo.firstWinningNumber);

    printf("Winning board:\n");
    printBoard(&bingo.firstWinningBoard);

    int unmarkedSum = 0;

    for (int col = 0; col < 5; ++col) {
        for (int row = 0; row < 5; ++row) {
            if (!bingo.firstWinningBoard[col][row].marked) {
                unmarkedSum += bingo.firstWinningBoard[col][row].number;
            }
        }
    }

    printf("unmarkedSum: %u\n", unmarkedSum);

    return unmarkedSum * bingo.firstWinningNumber;
}

// static int partTwo(const Input *input) {
//     return -1;
// }

int main() {
    Input input = {0};
    parseInput("./Day04.txt", &input);

    int partOneResult = partOne(&input);
    // assert(partOneResult == 4512);
    assert(partOneResult == 21607);
    printf("Part one: %d \n", partOneResult);

    // int partTwoResult = partTwo(commands, commandCount);
    // assert(partTwoResult == 1739283308);
    // printf("Part two: %d \n", partTwoResult);

    return 0;
}
