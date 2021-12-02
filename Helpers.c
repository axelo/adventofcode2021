#ifndef _HELPERS_
#define _HELPERS_

#include <assert.h>
#include <stdio.h>  // printf, file functions
#include <string.h> // sscanf

static void readInput(const char *filename, char *destination, size_t maxSize) {
    FILE *file = fopen(filename, "r");

    assert(file != NULL && "fopen: Couldn't open input file");

    assert(fread(destination, 1, maxSize, file) != maxSize && "fread: Couldn't fit the entire file into destination");

    assert(fclose(file) == 0 && "fclose: Couldn't close the input file");
}

#endif
