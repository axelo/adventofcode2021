#ifndef _HELPERS_
#define _HELPERS_

#include <assert.h>
#include <inttypes.h> // strtoimax
#include <limits.h>   // *INT_MAX
#include <math.h>     // ceil, floor
#include <stdbool.h>  // bool
#include <stdint.h>   // uint*_h
#include <stdio.h>    // printf, file functions
#include <stdlib.h>   // qsort
#include <string.h>   // memcpy, sscanf
#include <time.h>     // timespec_get

typedef enum {
    PART1 = 1,
    PART2
} PART;

static char Helpers_readInputFile_buffer[24 * 1024];

#ifdef EXAMPLE
static bool Helpers_readInputFile_useExample = true;
#else
static bool Helpers_readInputFile_useExample = false;
#endif

static const char *Helpers_readInputFile(const char *dayFilename) {
    char day[6] = {0};
    strncpy(day, dayFilename, 5);

    char filename[64];
    sprintf(filename, "%s%s.txt", day, Helpers_readInputFile_useExample ? ".example" : "");

    printf("Reading input file %s\n", filename);

    FILE *file = fopen(filename, "r");

    assert(file != NULL && "fopen: Couldn't open input file");

    assert(fread(Helpers_readInputFile_buffer,
                 1, sizeof(Helpers_readInputFile_buffer),
                 file) != sizeof(Helpers_readInputFile_buffer) &&
           "fread: Couldn't fit the entire file into destination");

    assert(fclose(file) == 0 && "fclose: Couldn't close the input file");

    return Helpers_readInputFile_buffer;
}

int64_t Helpers_clock() {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((int64_t)now.tv_sec) * 1000000 + ((int64_t)now.tv_nsec) / 1000;
}

void Helpers_assert(PART part, int64_t start, int64_t actual, int64_t expectedFromExample, int64_t expectedFromInput) {
    int64_t end = Helpers_clock();

    int64_t expected = Helpers_readInputFile_useExample
                           ? expectedFromExample
                           : expectedFromInput;

    printf("Part %d: %lld, expected %lld, took %lld us\n", part, actual, expected, end - start);

    assert(actual == expected);
}

#endif
