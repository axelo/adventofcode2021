#ifndef _HELPERS_
#define _HELPERS_

#include <assert.h>
#include <stdbool.h> // bool
#include <stdint.h>  // uint*_h
#include <stdio.h>   // printf, file functions
#include <string.h>  // memcpy,sscanf
#include <time.h>    // timespec_get

static char Helpers_readInputFile_buffer[24 * 1024];

#ifdef EXAMPLE
static bool Helpers_readInputFile_useExample = true;
#else
static bool Helpers_readInputFile_useExample = false;
#endif

static const char *Helpers_readInputFile(uint8_t day) {
    char filename[64];

    sprintf(filename, "Day%02d%s.txt", day, Helpers_readInputFile_useExample ? ".example" : "");

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

#endif
