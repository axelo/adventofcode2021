#include "Helpers.c"

#define N ((('z' << 8) | 'z') + 1)
#define CONN_CAP 8

#define START_CAVE_ID 0
#define END_CAVE_ID 1
#define BIG_CAVE_ID_MAX (('Z' << 8) | 'Z')

typedef uint16_t CaveId;

typedef struct {
    int nConnections;
    CaveId connections[CONN_CAP];
} Cave;

static CaveId idFromString(const char *s) {
    switch (strnlen(s, 6)) {
    case 5: return START_CAVE_ID;
    case 3: return END_CAVE_ID;
    case 2: return (CaveId)((s[0] << 8) | s[1]);
    case 1: return (CaveId)((s[0] << 8) | s[0]);
    default: assert(false);
    }
}

static void parse(const char *input, Cave caves[N]) {
    char a[8] = {0};
    char b[8] = {0};
    int charsRead = 0;
    int filled;

    while ((filled = sscanf(input, "%7[^-]-%7s\n%n", a, b, &charsRead)) == 2) {
        CaveId idA = idFromString(a);
        CaveId idB = idFromString(b);

        caves[idA].connections[caves[idA].nConnections++] = idB;
        caves[idB].connections[caves[idB].nConnections++] = idA;

        assert(caves[idA].nConnections < CONN_CAP);
        assert(caves[idB].nConnections < CONN_CAP);

        input += charsRead;
    }
}

static int countPathsHelp(const Cave caves[N], const CaveId id, const uint8_t maxVisitsOnce, uint8_t visited[N], bool hasVisitedMaxOnce) {
    if (id == END_CAVE_ID) {
        return 1;
    }

    if ((!hasVisitedMaxOnce && visited[id] >= maxVisitsOnce) ||
        ((hasVisitedMaxOnce || id == START_CAVE_ID) && visited[id] > 0)) {
        return 0;
    }

    bool isSmallCave = id == START_CAVE_ID || id > BIG_CAVE_ID_MAX;

    if (isSmallCave) {
        ++visited[id];
    };

    hasVisitedMaxOnce |= visited[id] >= maxVisitsOnce;

    int n = 0;

    for (int i = 0; i < caves[id].nConnections; ++i) {
        n += countPathsHelp(caves, caves[id].connections[i], maxVisitsOnce, visited, hasVisitedMaxOnce);
    }

    if (visited[id] > 0) {
        --visited[id];
    }

    return n;
}

static int countPaths(const Cave caves[N], const uint8_t maxVisitsOnce) {
    uint8_t visited[N] = {0};
    return countPathsHelp(caves, START_CAVE_ID, maxVisitsOnce, visited, false);
}

static int partOne(const Cave caves[N]) {
    return countPaths(caves, 1);
}

static int partTwo(const Cave caves[N]) {
    return countPaths(caves, 2);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Cave caves[N] = {0};
    parse(input, caves);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(caves),
                   10, 4885);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(caves),
                   36, 117095);

    return 0;
}
