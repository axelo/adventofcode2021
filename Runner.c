#ifndef _RUNNER_C_
#define _RUNNER_C_

#include <time.h> // timespec_get

#ifndef DAY
#error DAY is not defined
#endif

#include <assert.h>
#include <stdio.h>  // printf, file functions
#include <string.h> // sscanf

// Turn A into a string literal without expanding macro definitions.
#define STRINGIZE_NON_EXPAND(A) #A

// Turn A into a string literal after macro-expanding it.
#define STRINGIZE(A) STRINGIZE_NON_EXPAND(A)

#ifdef example
#define INPUT_FILENAME "./Day" STRINGIZE(DAY) ".example.txt"
#else
#define INPUT_FILENAME "./Day" STRINGIZE(DAY) ".txt"
#endif

typedef struct {
    int64_t actual;
    int64_t expectedExample;
    int64_t expected;
} Result;

static int parse(const char *inputString, INPUT input[INPUT_CAP]);
static Result partOne(int n, const INPUT input[n]);
static Result partTwo(int n, const INPUT input[n]);

static int64_t Runner_micros() {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((int64_t)now.tv_sec) * 1000000 + ((int64_t)now.tv_nsec) / 1000;
}

static void Runner_readInput(char *destination, size_t maxSize) {
    FILE *file = fopen(INPUT_FILENAME, "r");

    assert(file != NULL && "fopen: Couldn't open input file");

    assert(fread(destination, 1, maxSize, file) != maxSize && "fread: Couldn't fit the entire file into destination");

    assert(fclose(file) == 0 && "fclose: Couldn't close the input file");
}

int main() {
    char inputString[32 * 1024];
    Runner_readInput(inputString, sizeof(inputString));

    INPUT input[INPUT_CAP] = {0};
    int n = parse(inputString, input);

    int64_t start = Runner_micros();

    Result partOneResult = partOne(n, input);

    int64_t end = Runner_micros();

    printf("Part one: %lld, took %lld us\n", partOneResult.actual, end - start);

#ifdef example
    assert(partOneResult.actual == partOneResult.expectedExample);
#else
    assert(partOneResult.actual == partOneResult.expected);
#endif

    start = Runner_micros();

    Result partTwoResult = partTwo(n, input);

    end = Runner_micros();

    printf("Part two: %lld, took %lld us\n", partTwoResult.actual, end - start);

#ifdef example
    assert(partTwoResult.actual == partTwoResult.expectedExample);
#else
    assert(partTwoResult.actual == partTwoResult.expected);
#endif
}

#endif