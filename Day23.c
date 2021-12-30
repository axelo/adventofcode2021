#include "Helpers.c"

typedef struct {
    int x;
    int y;
} Amp;

typedef struct {
    int n;
    Amp as[17];
} Amps;

typedef struct {
    int32_t energy;
    int8_t m[5][11];
} Map;

typedef struct {
    int n;
    Map ms[10];
} Maps;

#define QUEUE_CAP 200000

static Map queue[QUEUE_CAP];
static int64_t nQueue = 0;
static int64_t iQueue = 0;

static Map parse(const char *input, bool part2) {
    int charsRead = 0;
    char a1[2] = {0};
    char a2[2] = {0};
    char a3[2] = {0};
    char a4[2] = {0};

    Map map = {0};

    map.m[1][0] = '#';
    map.m[1][10] = '#';

    map.m[1][1] = '#';
    map.m[2][1] = '#';
    map.m[3][1] = '#';
    map.m[4][1] = '#';

    map.m[1][3] = '#';
    map.m[2][3] = '#';
    map.m[3][3] = '#';
    map.m[4][3] = '#';

    map.m[1][5] = '#';
    map.m[2][5] = '#';
    map.m[3][5] = '#';
    map.m[4][5] = '#';

    map.m[1][7] = '#';
    map.m[2][7] = '#';
    map.m[3][7] = '#';
    map.m[4][7] = '#';

    map.m[1][9] = '#';
    map.m[2][9] = '#';
    map.m[3][9] = '#';
    map.m[4][9] = '#';

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
    // printf("%s %s %s %s\n", a1, a2, a3, a4);

    map.m[y][2] = a1[0];
    map.m[y][4] = a2[0];
    map.m[y][6] = a3[0];
    map.m[y][8] = a4[0];

    return map;
}

static void dump(int roomSize, const Map *map, const Amp amp) {
    int hx = amp.x;
    int hy = amp.y;

    for (int y = 0; y <= roomSize; ++y) {
        for (int x = 0; x < 11; ++x) {
            char t = map->m[y][x] == 0 ? ' ' : map->m[y][x];
            if (x == hx && y == hy) {
                printf("\033[0;33m%c\033[0m", t);
            } else {
                printf("%c", t);
            }
        }
        printf("\n");
    }
}

static void findMalplacedAmps(int roomSize, const Map *map, Amps *amps) {
    amps->n = 0;

    for (int x = 0; x < 11; ++x) {
        if (map->m[0][x] != 0) {
            assert(amps->n < 16);
            amps->as[amps->n].x = x;
            amps->as[amps->n].y = 0;
            ++amps->n;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][2] != 0 && map->m[y][2] != 'A') {
            for (int ay = y; ay >= 1; --ay) {
                if (map->m[ay][2] != 0) {
                    assert(amps->n < 16);
                    amps->as[amps->n].x = 2;
                    amps->as[amps->n].y = ay;
                    ++amps->n;
                }
            }
            break;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][4] != 0 && map->m[y][4] != 'B') {
            for (int by = y; by >= 1; --by) {
                if (map->m[by][4] != 0) {
                    assert(amps->n < 16);
                    amps->as[amps->n].x = 4;
                    amps->as[amps->n].y = by;
                    ++amps->n;
                }
            }
            break;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][6] != 0 && map->m[y][6] != 'C') {
            for (int by = y; by >= 1; --by) {
                if (map->m[by][6] != 0) {
                    assert(amps->n < 16);
                    amps->as[amps->n].x = 6;
                    amps->as[amps->n].y = by;
                    ++amps->n;
                }
            }
            break;
        }
    }

    for (int y = roomSize; y >= 1; --y) {
        if (map->m[y][8] != 0 && map->m[y][8] != 'D') {
            for (int by = y; by >= 1; --by) {
                if (map->m[by][8] != 0) {
                    assert(amps->n < 16);
                    amps->as[amps->n].x = 8;
                    amps->as[amps->n].y = by;
                    ++amps->n;
                }
            }
            break;
        }
    }
}

static void moveOnce(int roomSize, const Amp amp, const Map *map, Maps *maps) {

    // printf("MOVE FROM:\n");
    // dump(roomSize, map, amp);
    maps->n = 0;

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

    // printf("(%d,%d): %c, in hallway? %s. In room? %s. In correct room? %s\n", x, y, t,
    //        isInHallway ? "true" : "false", isInRoom ? "true" : "false", isInCorrectRoom ? "true" : "false");

    bool isInRoom = y > 0;

    if (isInRoom) {
        // Can we go to the hallway?
        int ySteps = 0;

        for (int y2 = y - 1; y2 >= 0; --y2) {
            if (map->m[y2][x] != 0) {
                ySteps = 0;
                break;
            }
            ++ySteps;
        }

        // printf("Can we go to the hallway? %s\n", empty ? "true" : "false");

        if (ySteps > 0) {
            // yes, can we past the hallway to the correct room?
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

            if (canWalkToHallway) {
                // yes, can we enter the room?
                // (map->m[2][destX] == t && map->m[1][destX] == 0)
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

                if (roomY > 0) {
                    // Move into the room
                    maps->ms[maps->n] = *map;

                    maps->ms[maps->n].energy += energyPerStep * (abs(destX - x) + ySteps + roomY);

                    // maps->ms[maps->n].amp.x = destX;
                    // maps->ms[maps->n].amp.y = roomY;

                    maps->ms[maps->n].m[y][x] = 0;
                    maps->ms[maps->n].m[roomY][destX] = t;

                    ++maps->n;
                    assert(maps->n < 10);

                    return;
                }
            }

            // Nope, pick a valid position in the hallway
            for (int x2 = x - 1; x2 >= 0; --x2) {
                if (map->m[0][x2] != 0) {
                    break;
                }

                if (x2 == 2 || x2 == 4 || x2 == 6 || x2 == 8) {
                    continue;
                }

                maps->ms[maps->n] = *map;

                maps->ms[maps->n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                // maps->ms[maps->n].amp.x = x2;
                // maps->ms[maps->n].amp.y = 0;

                maps->ms[maps->n].m[y][x] = 0;
                maps->ms[maps->n].m[0][x2] = t;

                ++maps->n;
                assert(maps->n < 10);
            }

            for (int x2 = x + 1; x2 < 11; ++x2) {
                if (map->m[0][x2] != 0) {
                    break;
                }

                if (x2 == 2 || x2 == 4 || x2 == 6 || x2 == 8) {
                    continue;
                }

                // printf("Could try (%d,%d)\n", x2, 1);
                maps->ms[maps->n] = *map;

                maps->ms[maps->n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                // maps->ms[maps->n].amp.x = x2;
                // maps->ms[maps->n].amp.y = 0;

                maps->ms[maps->n].m[y][x] = 0;
                maps->ms[maps->n].m[0][x2] = t;

                ++maps->n;
                assert(maps->n < 10);
            }

            return;
        } else {
            // no, can't go to the hallway
            // printf("Stuck in wrong room!\n");
            return;
        }
    } else {
        // Once an amphipod stops moving in the hallway, it will stay in that spot until it can move into a room.
        // Amphipods will never move from the hallway into a room unless that room is their destination room and that room contains no amphipods which do not also have that room as their own destination.
        bool canWalkToHallway = true;

        if (x < destX) {
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
        // printf("xEmpty to %d? %s\n", destX, xEmpty ? "true" : "false");

        if (canWalkToHallway) { // TODO!!!!
            // yes, can we enter the room?
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

            if (roomY > 0) {
                // Move into the room
                maps->ms[maps->n] = *map;

                maps->ms[maps->n].energy += energyPerStep * (abs(destX - x) + roomY);

                // maps->ms[maps->n].amp.x = destX;
                // maps->ms[maps->n].amp.y = roomY;

                maps->ms[maps->n].m[y][x] = 0;
                maps->ms[maps->n].m[roomY][destX] = t;

                ++maps->n;
                assert(maps->n < 10);
            }

            return;
            // if (map->m[2][destX] == 0) {
            //     // printf("Could try %d,%d\n", 3, destX);

            //     maps->ms[maps->n] = *map;

            //     maps->ms[maps->n].energy += energyPerStep * (abs(destX - x) + abs(2 - y));

            //     maps->ms[maps->n].m[y][x] = 0;
            //     maps->ms[maps->n].m[2][destX] = t;

            //     ++maps->n;
            //     assert(maps->n < 10);

            //     return;
            // } else if (map->m[2][destX] == t && map->m[1][destX] == 0) {
            //     // printf("Could try %d,%d\n", 2, destX);

            //     maps->ms[maps->n] = *map;

            //     maps->ms[maps->n].energy += energyPerStep * (abs(destX - x) + abs(1 - y));

            //     maps->ms[maps->n].m[y][x] = 0;
            //     maps->ms[maps->n].m[1][destX] = t;

            //     ++maps->n;
            //     assert(maps->n < 10);

            //     return;
            // } else {
            //     // printf("Can't move into correct room\n");
            //     return;
            // }
        }

        return;
    }
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

static int partOne(int roomSize, const Map *map) {
    int minEnergy = INT_MAX;

    Amps amps = {0};
    Maps maps = {0};

    queue[nQueue++] = *map;

    while (iQueue < nQueue) {
        Map *m = &queue[iQueue++];

        findMalplacedAmps(roomSize, m, &amps);

        if (amps.n == 0) {
            dump(roomSize, m, (Amp){-1, -1});

            if (m->energy < minEnergy) {
                minEnergy = m->energy;
            }
        }

        for (int i = 0; i < amps.n; ++i) {
            moveOnce(roomSize, amps.as[i], m, &maps);

            for (int j = 0; j < maps.n; ++j) {
                if (maps.ms[j].energy < minEnergy) {
                    bool visited = false;

                    for (int64_t k = iQueue; k < nQueue; ++k) {
                        if (isMapEqual(roomSize, &maps.ms[j], &queue[k])) {
                            if (queue[k].energy > maps.ms[j].energy) {
                                queue[k].energy = maps.ms[j].energy;
                            }

                            visited = true;
                            break;
                        }
                    }

                    if (!visited) {
                        assert(nQueue < QUEUE_CAP);
                        queue[nQueue++] = maps.ms[j];
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
