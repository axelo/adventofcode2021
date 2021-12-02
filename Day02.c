#include "Helpers.c"

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

static int parseInput(const char *filename, Command *commands, size_t maxCount) {
    char input[32 * 1024] = {0};

    readInput(filename, input, sizeof(input));

    char *inputPtr = input;
    char dirString[32] = {0};
    int charsRead = 0;
    int filled = 0;

    size_t commandCount = 0;

    while ((filled = sscanf(inputPtr, "%32s %u\n%n", dirString, &commands[commandCount].delta, &charsRead)) != EOF) {
        assert(filled == 2 && "parseInput: Failed to parse input");

        Direction dir =
            strcmp(dirString, "forward") == 0 ? Forward
            : strcmp(dirString, "down") == 0  ? Down
            : strcmp(dirString, "up") == 0    ? Up
                                              : Idle;

        assert(dir != Idle && "parseInput: Unknown direction");

        commands[commandCount++].dir = dir;

        assert(commandCount < maxCount);

        inputPtr += charsRead;
    }

    return commandCount;
}

static int partOne(const Command *commands, size_t commandCount) {
    int horizontalPos = 0;
    int depth = 0;

    for (const Command *command = commands; command < commands + commandCount; ++command) {
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

static int partTwo(const Command *commands, size_t commandCount) {
    int horizontalPos = 0;
    int depth = 0;
    int aim = 0;

    for (const Command *command = commands; command < commands + commandCount; ++command) {
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
    Command commands[4096] = {0};
    size_t commandCount = parseInput("./Day02.txt", commands, sizeof(commands) / sizeof(Command));

    int partOneResult = partOne(commands, commandCount);
    assert(partOneResult == 1815044);
    printf("Part one: %d \n", partOneResult);

    int partTwoResult = partTwo(commands, commandCount);
    assert(partTwoResult == 1739283308);
    printf("Part two: %d \n", partTwoResult);

    return 0;
}
