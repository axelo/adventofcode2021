#include "Helpers.c"

#define N UINT16_MAX
#define CONN_CAP 32

typedef uint16_t CaveId;

typedef struct {
    int nConnections;
    CaveId connections[CONN_CAP];
} Cave;

static const CaveId startCaveId = 0;
static const CaveId endCaveId = 1;

static CaveId idFromString(const char *s) {
    switch (strnlen(s, 6)) {
    case 5: return startCaveId;
    case 3: return endCaveId;
    case 2: return (s[0] << 8) | s[1];
    case 1: return (s[0] << 8) | s[0];
    default: assert(false);
    }
}

static void parse(const char *input, Cave caves[N]) {
    char from[8] = {0};
    char to[8] = {0};
    int charsRead = 0;
    int filled;

    while ((filled = sscanf(input, "%7[^-]-%7s\n%n", from, to, &charsRead)) == 2) {
        uint16_t idA = idFromString(from);
        uint16_t idB = idFromString(to);

        caves[idA].connections[caves[idA].nConnections++] = idB;
        assert(caves[idA].nConnections < CONN_CAP);

        caves[idB].connections[caves[idB].nConnections++] = idA;
        assert(caves[idB].nConnections < CONN_CAP);

        input += charsRead;
    }
}

static int countPaths(const Cave caves[N], const CaveId id, const uint8_t maxVisitsOnce, uint8_t visited[N], bool visitedMaxOnce) {
    if (id == endCaveId) {
        return 1;
    }

    if ((!visitedMaxOnce && visited[id] >= maxVisitsOnce) ||
        ((visitedMaxOnce || id == startCaveId) && visited[id] > 0)) {
        return 0;
    }

    bool isBigCave = id != startCaveId && id <= (('Z' << 8) | 'Z');

    if (!isBigCave && ++visited[id] >= maxVisitsOnce) {
        visitedMaxOnce = true;
    }

    int n = 0;

    for (int i = 0; i < caves[id].nConnections; ++i) {
        n += countPaths(caves, caves[id].connections[i], maxVisitsOnce, visited, visitedMaxOnce);
    }

    if (visited[id] > 0) {
        --visited[id];
    }

    return n;
}

static int partOne(const Cave caves[N]) {
    uint8_t visited[N] = {0};

    return countPaths(caves, startCaveId, 1, visited, false);
}

static int partTwo(const Cave caves[N]) {
    uint8_t visited[N] = {0};

    return countPaths(caves, startCaveId, 2, visited, false);
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
