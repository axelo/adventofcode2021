#include "Helpers.c"

#define CAP 4096

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

static int parse(const char *input, Command commands[CAP]) {
    char dirString[32] = {0};
    int charsRead = 0;
    int filled = 0;

    int n = 0;

    while ((filled = sscanf(input, "%32s %u\n%n", dirString, &commands[n].delta, &charsRead)) != EOF) {
        assert(filled == 2 && "parseInput: Failed to parse input");

        Direction dir =
            strcmp(dirString, "forward") == 0 ? Forward
            : strcmp(dirString, "down") == 0  ? Down
            : strcmp(dirString, "up") == 0    ? Up
                                              : Idle;

        assert(dir != Idle && "parseInput: Unknown direction");

        commands[n++].dir = dir;

        assert(n < CAP);

        input += charsRead;
    }

    return n;
}

static int partOne(int n, const Command commands[n]) {
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

    return horizontalPos * depth;
}

static int partTwo(int n, const Command commands[n]) {
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

    return horizontalPos * depth;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Command commands[CAP] = {0};
    int n = parse(input, commands);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, commands),
                   150, 1815044);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, commands),
                   900, 1739283308);

    return 0;
}
