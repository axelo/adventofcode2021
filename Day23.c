#include "Helpers.c"

#define QUEUE_CAP 200000

typedef struct {
    int x;
    int y;
} Amp;

typedef struct {
    int32_t energy;
    int8_t m[5][11];
} Map;

static Map queue[QUEUE_CAP];
static int nQueue = 0;
static int iQueue = 0;

#define INDEX_CAP 0x3fff
#define BUCKET_CAP 800

static int visitedTable[INDEX_CAP + 1][BUCKET_CAP];
static int16_t nVisitedTable[INDEX_CAP + 1] = {0};

static Map parse(const char *input, bool part2) {
    int charsRead = 0;
    char a1[2] = {0};
    char a2[2] = {0};
    char a3[2] = {0};
    char a4[2] = {0};

    Map map = {0};

    sscanf(input, "#############\n%n", &charsRead);
    input += charsRead;

    sscanf(input, "#...........#\n%n", &charsRead);
    input += charsRead;

    assert(sscanf(input, "###%1[^#]#%1[^#]#%1[^#]#%1[^#]###\n%n", a1, a2, a3, a4, &charsRead) == 4);
    input += charsRead;

    map.m[1][2] = a1[0];
    map.m[1][4] = a2[0];
    map.m[1][6] = a3[0];
    map.m[1][8] = a4[0];

    if (part2) {
        // #D#C#B#A#
        map.m[2][2] = 'D';
        map.m[2][4] = 'C';
        map.m[2][6] = 'B';
        map.m[2][8] = 'A';

        // #D#B#A#C#
        map.m[3][2] = 'D';
        map.m[3][4] = 'B';
        map.m[3][6] = 'A';
        map.m[3][8] = 'C';
    }

    int y = part2 ? 4 : 2;

    assert(sscanf(input, "  #%1[^#]#%1[^#]#%1[^#]#%1[^#]#\n%n", a1, a2, a3, a4, &charsRead) == 4);
    input += charsRead;

    map.m[y][2] = a1[0];
    map.m[y][4] = a2[0];
    map.m[y][6] = a3[0];
    map.m[y][8] = a4[0];

    return map;
}

static void dump(int roomSize, const Map *map) {
    for (int y = 0; y <= roomSize; ++y) {
        for (int x = 0; x < 11; ++x) {
            printf("%c", map->m[y][x] == 0 ? ' ' : map->m[y][x]);
        }
        printf("\n");
    }
}

static int findMalplacedAmps(int roomSize, const Map *map, Amp amps[16]) {
    int n = 0;

    for (int x = 0; x < 11; ++x) {
        if (map->m[0][x] != 0) {
            assert(n < 16);
            amps[n].x = x;
            amps[n].y = 0;
            ++n;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][2] != 0 && map->m[y][2] != 'A') {
            for (int ay = y; ay >= 1; --ay) {
                if (map->m[ay][2] != 0) {
                    assert(n < 16);
                    amps[n].x = 2;
                    amps[n].y = ay;
                    ++n;
                }
            }
            break;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][4] != 0 && map->m[y][4] != 'B') {
            for (int by = y; by >= 1; --by) {
                if (map->m[by][4] != 0) {
                    assert(n < 16);
                    amps[n].x = 4;
                    amps[n].y = by;
                    ++n;
                }
            }
            break;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][6] != 0 && map->m[y][6] != 'C') {
            for (int by = y; by >= 1; --by) {
                if (map->m[by][6] != 0) {
                    assert(n < 16);
                    amps[n].x = 6;
                    amps[n].y = by;
                    ++n;
                }
            }
            break;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][8] != 0 && map->m[y][8] != 'D') {
            for (int by = y; by >= 1; --by) {
                if (map->m[by][8] != 0) {
                    assert(n < 16);
                    amps[n].x = 8;
                    amps[n].y = by;
                    ++n;
                }
            }
            break;
        }
    }

    return n;
}

static int moveOnce(int roomSize, const Amp amp, const Map *map, Map outMaps[10]) {
    int n = 0;

    int x = amp.x;
    int y = amp.y;
    char t = map->m[y][x];

    assert(t != 0);

    int destX =
        t == 'A'   ? 2
        : t == 'B' ? 4
        : t == 'C' ? 6
                   : 8;

    int energyPerStep =
        t == 'A'   ? 1
        : t == 'B' ? 10
        : t == 'C' ? 100
                   : 1000;

    if (y > 0) { // Is inside a room. Can we go to the hallway?
        int ySteps = 0;

        for (int y2 = y - 1; y2 >= 0; --y2) {
            if (map->m[y2][x] != 0) {
                ySteps = 0;
                break;
            }
            ++ySteps;
        }

        if (ySteps > 0) { // Yes, can we walk past the hallway to the correct room?
            bool canWalkToHallway = true;

            if (destX > x) {
                for (int x2 = x + 1; x2 <= destX; ++x2) {
                    if (map->m[0][x2] != 0) {
                        canWalkToHallway = false;
                        break;
                    }
                }
            } else {
                for (int x2 = x - 1; x2 >= destX; --x2) {
                    if (map->m[0][x2] != 0) {
                        canWalkToHallway = false;
                        break;
                    }
                }
            }

            if (canWalkToHallway) { // Yes, can we enter the room?
                int roomY = 0;

                for (int y2 = roomSize; y2 >= 1; --y2) {
                    if (map->m[y2][destX] == 0) {
                        roomY = y2;
                        break;
                    } else if (map->m[y2][destX] == t) {
                        continue;
                    } else {
                        break;
                    }
                }

                if (roomY > 0) { // Yes, move into the room.
                    outMaps[n] = *map;
                    outMaps[n].energy += energyPerStep * (abs(destX - x) + ySteps + roomY);

                    outMaps[n].m[y][x] = 0;
                    outMaps[n].m[roomY][destX] = t;

                    ++n;
                    assert(n < 10);

                    return n;
                }
            }

            // Nope, collect valid positions in the hallway.
            for (int x2 = x - 1; x2 >= 0; --x2) {
                if (map->m[0][x2] != 0) {
                    break;
                }

                if (x2 == 2 || x2 == 4 || x2 == 6 || x2 == 8) {
                    continue;
                }

                outMaps[n] = *map;
                outMaps[n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                outMaps[n].m[y][x] = 0;
                outMaps[n].m[0][x2] = t;

                ++n;
                assert(n < 10);
            }

            for (int x2 = x + 1; x2 < 11; ++x2) {
                if (map->m[0][x2] != 0) {
                    break;
                }

                if (x2 == 2 || x2 == 4 || x2 == 6 || x2 == 8) {
                    continue;
                }

                outMaps[n] = *map;
                outMaps[n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                outMaps[n].m[y][x] = 0;
                outMaps[n].m[0][x2] = t;

                ++n;
                assert(n < 10);
            }
        }
    } else { // In the hallway. Can we walk to the correct room hallway?
        bool canWalkToRoomHallway = true;

        if (x < destX) {
            for (int x2 = x + 1; x2 <= destX; ++x2) {
                if (map->m[0][x2] != 0) {
                    canWalkToRoomHallway = false;
                    break;
                }
            }
        } else {
            for (int x2 = x - 1; x2 >= destX; --x2) {
                if (map->m[0][x2] != 0) {
                    canWalkToRoomHallway = false;
                    break;
                }
            }
        }

        if (canWalkToRoomHallway) { // Yes, can we enter the room?
            int roomY = 0;

            for (int y2 = roomSize; y2 >= 1; --y2) {
                if (map->m[y2][destX] == 0) {
                    roomY = y2;
                    break;
                } else if (map->m[y2][destX] == t) {
                    continue;
                } else {
                    break;
                }
            }

            if (roomY > 0) { // Yes, move into the room
                outMaps[n] = *map;
                outMaps[n].energy += energyPerStep * (abs(destX - x) + roomY);

                outMaps[n].m[y][x] = 0;
                outMaps[n].m[roomY][destX] = t;

                ++n;
                assert(n < 10);
            }
        }
    }

    return n;
}

static bool isMapEqual(int roomSize, const Map *a, const Map *b) {
    for (int y = 0; y <= roomSize; ++y) {
        for (int x = 0; x < 11; ++x) {
            if (a->m[y][x] != b->m[y][x]) {
                return false;
            }
        }
    }
    return true;
}

// I winged this one, did some checks on distributions. Look ok for my input.
static inline uint64_t mapHash(const int8_t m[5][11]) {
    return (((uint64_t)(m[1][2]) & 3) << 0) |
           (((uint64_t)(m[1][4]) & 3) << 1) |
           (((uint64_t)(m[1][6]) & 3) << 2) |
           (((uint64_t)(m[1][8]) & 3) << 3) |
           (((uint64_t)(m[2][2]) & 3) << 4) |
           (((uint64_t)(m[2][4]) & 3) << 5) |
           (((uint64_t)(m[2][6]) & 3) << 6) |
           (((uint64_t)(m[2][8]) & 3) << 7) |
           (((uint64_t)(m[0][0]) & 3) << 8) |
           (((uint64_t)(m[0][1]) & 3) << 9) |
           (((uint64_t)(m[0][3]) & 3) << 10) |
           (((uint64_t)(m[0][5]) & 3) << 11) |
           (((uint64_t)(m[0][7]) & 3) << 13);
}

static int partOne(int roomSize, const Map *map) {
    int minEnergy = INT_MAX;

    Amp amps[16] = {0};
    Map maps[10] = {0};

    iQueue = 0;
    nQueue = 0;
    queue[nQueue++] = *map;

    memset(nVisitedTable, 0, sizeof(nVisitedTable));

    while (iQueue < nQueue) {
        Map *m = &queue[iQueue++];

        int nAmps = findMalplacedAmps(roomSize, m, amps);

        if (nAmps == 0) {
            dump(roomSize, m);

            if (m->energy < minEnergy) {
                minEnergy = m->energy;
            }
        }

        for (int i = 0; i < nAmps; ++i) {
            int nMaps = moveOnce(roomSize, amps[i], m, maps);

            for (int j = 0; j < nMaps; ++j) {
                if (maps[j].energy < minEnergy) {
                    uint64_t hash = mapHash(maps[j].m);
                    int hashIndex = hash & INDEX_CAP;

                    int visitedMap = -1;

                    for (int k = 0; k < nVisitedTable[hashIndex]; ++k) {
                        int queueIndex = visitedTable[hashIndex][k];
                        if (isMapEqual(roomSize, &queue[queueIndex], &maps[j])) {
                            visitedMap = queueIndex;
                            break;
                        }
                    }

                    if (visitedMap >= 0) {
                        if (queue[visitedMap].energy > maps[j].energy) {
                            queue[visitedMap].energy = maps[j].energy;
                        }
                    } else {
                        assert(nQueue < QUEUE_CAP);
                        assert(nVisitedTable[hashIndex] < BUCKET_CAP);

                        visitedTable[hashIndex][nVisitedTable[hashIndex]++] = nQueue;
                        queue[nQueue++] = maps[j];
                    }
                }
            }
        }
    }

    return minEnergy;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Map mapPart1 = parse(input, false);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(2, &mapPart1),
                   12521, 14350);

    Map mapPart2 = parse(input, true);

    Helpers_assert(PART2, Helpers_clock(),
                   partOne(4, &mapPart2),
                   44169, 49742);

    return 0;
}
