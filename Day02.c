#define DAY 02
#define INPUT Command
#define INPUT_CAP 4096

typedef enum {
    Idle = 0,
    Forward,
    Down,
    Up
} Direction;

typedef struct {
    Direction dir;
    int delta;
} Command;

#include "Runner.c"

static int parse(const char *inputString, Command commands[INPUT_CAP]) {
    char dirString[32] = {0};
    int charsRead = 0;
    int filled = 0;

    int n = 0;

    while ((filled = sscanf(inputString, "%32s %u\n%n", dirString, &commands[n].delta, &charsRead)) != EOF) {
        assert(filled == 2 && "parseInput: Failed to parse input");

        Direction dir =
            strcmp(dirString, "forward") == 0 ? Forward
            : strcmp(dirString, "down") == 0  ? Down
            : strcmp(dirString, "up") == 0    ? Up
                                              : Idle;

        assert(dir != Idle && "parseInput: Unknown direction");

        commands[n++].dir = dir;

        assert(n < INPUT_CAP);

        inputString += charsRead;
    }

    return n;
}

static Result partOne(int n, const Command commands[n]) {
    int horizontalPos = 0;
    int depth = 0;

    for (const Command *command = commands; command < commands + n; ++command) {
        switch (command->dir) {
        case Up:
            depth -= command->delta;
            break;
        case Down:
            depth += command->delta;
            break;
        case Forward:
            horizontalPos += command->delta;
            break;
        case Idle:
            break;
        }
    }

    return (Result){horizontalPos * depth, 150, 1815044};
}

static Result partTwo(int n, const Command commands[n]) {
    int horizontalPos = 0;
    int depth = 0;
    int aim = 0;

    for (const Command *command = commands; command < commands + n; ++command) {
        switch (command->dir) {
        case Up:
            aim -= command->delta;
            break;
        case Down:
            aim += command->delta;
            break;
        case Forward:
            horizontalPos += command->delta;
            depth += aim * command->delta;
            break;
        case Idle:
            break;
        }
    }

    return (Result){horizontalPos * depth, 900, 1739283308};
}
