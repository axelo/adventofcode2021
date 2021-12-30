#include "Helpers.c"

#define CAP 100

typedef struct {
    int x;
    int y;
} Amp;

typedef struct {
    int n;
    Amp as[8];
} Amps;

typedef struct {
    int energy;
    Amp amp;
    char m[4][11];
} Map;

typedef struct {
    int n;
    Map ms[10];
} Maps;

static Map parse(const char *input) {
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

    map.m[1][3] = '#';
    map.m[2][3] = '#';

    map.m[1][5] = '#';
    map.m[2][5] = '#';

    map.m[1][7] = '#';
    map.m[2][7] = '#';

    map.m[1][9] = '#';
    map.m[2][9] = '#';

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

    assert(sscanf(input, "  #%1[^#]#%1[^#]#%1[^#]#%1[^#]#\n", a1, a2, a3, a4) == 4);

    map.m[2][2] = a1[0];
    map.m[2][4] = a2[0];
    map.m[2][6] = a3[0];
    map.m[2][8] = a4[0];

    return map;
}

static void dump(Map map) {
    int hx = map.amp.x;
    int hy = map.amp.y;

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 11; ++x) {
            char t = map.m[y][x] == 0 ? ' ' : map.m[y][x];
            if (x == hx && y == hy) {
                printf("\033[0;33m%c\033[0m", t);
            } else {
                printf("%c", t);
            }
        }
        printf("\n");
    }
}

static Amps findMalplacedAmps(Map map) {
    Amps amps = {0};

    for (int x = 0; x < 11; ++x) {
        if (map.m[0][x] >= 'A' && map.m[0][x] <= 'D') {
            amps.as[amps.n].x = x;
            amps.as[amps.n].y = 0;
            ++amps.n;
        }
    }

    for (int x = 0; x < 11; ++x) {
        if (map.m[1][x] >= 'A' && map.m[1][x] <= 'D') {
            if ((x == 2 && (map.m[2][x] != 'A' || map.m[1][x] != 'A')) ||
                (x == 4 && (map.m[2][x] != 'B' || map.m[1][x] != 'B')) ||
                (x == 6 && (map.m[2][x] != 'C' || map.m[1][x] != 'C')) ||
                (x == 8 && (map.m[2][x] != 'D' || map.m[1][x] != 'D'))) {
                amps.as[amps.n].x = x;
                amps.as[amps.n].y = 1;
                ++amps.n;
            }
        }
    }

    for (int x = 0; x < 11; ++x) {
        if (map.m[2][x] >= 'A' && map.m[2][x] <= 'D') {
            if ((x == 2 && (map.m[2][x] != 'A')) ||
                (x == 4 && (map.m[2][x] != 'B')) ||
                (x == 6 && (map.m[2][x] != 'C')) ||
                (x == 8 && (map.m[2][x] != 'D'))) {
                amps.as[amps.n].x = x;
                amps.as[amps.n].y = 2;
                ++amps.n;
            }
        }
    }

    return amps;
}

static Maps moveOnce(Map map) {
    int x = map.amp.x;
    int y = map.amp.y;

    bool isInHallway = y == 0;

    bool isInRoom = y >= 1;

    char t = map.m[y][x];

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

    bool isInCorrectRoom =
        isInRoom && x == destX;

    Maps maps = {0};

    // printf("(%d,%d): %c, in hallway? %s. In room? %s. In correct room? %s\n", x, y, t,
    //        isInHallway ? "true" : "false", isInRoom ? "true" : "false", isInCorrectRoom ? "true" : "false");

    if (isInCorrectRoom) {
        if (y == 2 || (map.m[2][x] == t)) { // TOD:!!! 2= 1
            printf("Done!");
            return maps;
        }
    }

    if (isInRoom) {
        // Can we go to the hallway?
        bool empty = true;
        int ySteps = 0;
        for (int y2 = y - 1; y2 >= 0; --y2) {
            ++ySteps;
            if (map.m[y2][x] != 0) {
                empty = false;
                break;
            }
        }

        // printf("Can we go to the hallway? %s\n", empty ? "true" : "false");

        if (empty) {
            // yes, can we past the hallway to the correct room?
            if (destX > x) {
                for (int x2 = x + 1; x2 <= destX; ++x2) {
                    if (map.m[0][x2] != 0) {
                        empty = false;
                        break;
                    }
                }
            } else {
                for (int x2 = x - 1; x2 >= destX; --x2) {
                    if (map.m[0][x2] != 0) {
                        empty = false;
                        break;
                    }
                }
            }

            if (empty) {
                // yes, can we enter the room?
                if (map.m[2][destX] == 0) {
                    // Bottom of room.
                    maps.ms[maps.n] = map;

                    maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + ySteps + 2);

                    maps.ms[maps.n].amp.x = destX;
                    maps.ms[maps.n].amp.y = 2;

                    maps.ms[maps.n].m[y][x] = 0;
                    maps.ms[maps.n].m[2][destX] = t;

                    ++maps.n;
                    assert(maps.n < 10);

                    return maps;

                } else if (map.m[2][destX] == t && map.m[1][destX] == 0) {
                    // Top of room.
                    maps.ms[maps.n] = map;

                    maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + ySteps + 1);

                    maps.ms[maps.n].amp.x = destX;
                    maps.ms[maps.n].amp.y = 1;

                    maps.ms[maps.n].m[y][x] = 0;
                    maps.ms[maps.n].m[1][destX] = t;

                    ++maps.n;
                    assert(maps.n < 10);

                    return maps;
                }
            }

            // Nope, pick a valid position in the hallway
            for (int x2 = x - 1; x2 >= 0; --x2) {
                if (map.m[0][x2] != 0) {
                    break;
                }

                if (x2 == 2 || x2 == 4 || x2 == 6 || x2 == 8) {
                    continue;
                }

                // printf("Could try (%d,%d)\n", x2, 1);
                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                maps.ms[maps.n].amp.x = x2;
                maps.ms[maps.n].amp.y = 0;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[0][x2] = t;
                ++maps.n;
                assert(maps.n < 10);
            }

            for (int x2 = x + 1; x2 < 11; ++x2) {
                if (map.m[0][x2] != 0) {
                    break;
                }

                if (x2 == 2 || x2 == 4 || x2 == 6 || x2 == 8) {
                    continue;
                }

                // printf("Could try (%d,%d)\n", x2, 1);
                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                maps.ms[maps.n].amp.x = x2;
                maps.ms[maps.n].amp.y = 0;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[0][x2] = t;
                ++maps.n;
                assert(maps.n < 10);
            }

            return maps;
        } else {
            // no, can't go to the hallway
            // printf("Stuck in wrong room!\n");
            return maps;
        }
    }

    if (isInHallway) {
        // Once an amphipod stops moving in the hallway, it will stay in that spot until it can move into a room.
        // Amphipods will never move from the hallway into a room unless that room is their destination room and that room contains no amphipods which do not also have that room as their own destination.

        bool xEmpty = true;

        if (x < destX) {
            for (int x2 = x + 1; x2 <= destX; ++x2) {
                if (map.m[0][x2] != 0) {
                    xEmpty = false;
                    break;
                }
            }
        } else {
            for (int x2 = x - 1; x2 >= destX; --x2) {
                if (map.m[0][x2] != 0) {
                    xEmpty = false;
                    break;
                }
            }
        }
        // printf("xEmpty to %d? %s\n", destX, xEmpty ? "true" : "false");

        if (xEmpty) {
            if (map.m[2][destX] == 0) {
                // printf("Could try %d,%d\n", 3, destX);

                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + abs(2 - y));

                maps.ms[maps.n].amp.x = destX;
                maps.ms[maps.n].amp.y = 2;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[2][destX] = t;

                ++maps.n;
                assert(maps.n < 10);

                return maps;
            } else if (map.m[2][destX] == t && map.m[1][destX] == 0) {
                // printf("Could try %d,%d\n", 2, destX);

                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + abs(1 - y));

                maps.ms[maps.n].amp.x = destX;
                maps.ms[maps.n].amp.y = 1;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[1][destX] = t;

                ++maps.n;
                assert(maps.n < 10);

                return maps;
            } else {
                // printf("Can't move into correct room\n");
                return maps;
            }
        }

        return maps;
    }

    return maps;
}

static int64_t ccc = 0;
static int64_t maxMoves = 0;

static int64_t partOne(Map map, int64_t minEnergy) {
    Amps amps = findMalplacedAmps(map);

    if (amps.n == 0) {

        ++ccc;

        if ((ccc % 200000) == 0) {
            printf("Done: %lld - %d\n", ccc, map.energy);
            // printf("** All done! **\n");
            dump(map);
            // printf("****\n");
        }
        return map.energy < minEnergy ? map.energy : minEnergy;
    }

    for (int i = 0; i < amps.n; ++i) {
        // printf("\nMalplaced amp: (%d,%d): %c\n",
        //        amps.as[i].x, amps.as[i].y,
        //        map.m[amps.as[i].y][amps.as[i].x]);

        Map map2 = map;
        map2.amp = amps.as[i];

        Maps maps = moveOnce(map2);

        // printf("Possible new maps: %d\n", maps.n);

        if (maps.n > maxMoves) {
            maxMoves = maps.n;
        }

        for (int j = 0; j < maps.n; ++j) {
            int64_t e2 = partOne(maps.ms[j], minEnergy);

            if (e2 < minEnergy) {
                minEnergy = e2;
            }
        }
    }

    return minEnergy;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Map map = parse(input);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(map, INT_MAX),
                   12521, 14350);

    printf("maxMoves: %lld\n", maxMoves);

    // Helpers_assert(PART2, Helpers_clock(),
    //                partTwo(n, xs),
    //                -1, -2);

    return 0;
}
