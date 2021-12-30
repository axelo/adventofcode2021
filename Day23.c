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
    char m[4][13];
} Map;

typedef struct {
    int n;
    Map ms[300];
} Maps;

static Map parse(const char *input) {
    int charsRead = 0;
    char a1[2] = {0};
    char a2[2] = {0};
    char a3[2] = {0};
    char a4[2] = {0};

    Map map = {0};

    for (int x = 0; x < 13; ++x) {
        map.m[0][x] = '#';
    }

    map.m[1][0] = map.m[1][12] = '#';

    map.m[2][0] = map.m[2][12] = '#';
    map.m[2][1] = map.m[2][11] = '#';
    map.m[2][2] = map.m[2][10] = '#';
    map.m[2][4] = map.m[2][8] = map.m[2][6] = '#';

    map.m[3][2] = map.m[3][10] = map.m[3][4] = map.m[3][8] = map.m[3][6] = '#';

    sscanf(input, "#############\n%n", &charsRead);
    input += charsRead;

    sscanf(input, "#...........#\n%n", &charsRead);
    input += charsRead;

    assert(sscanf(input, "###%1[^#]#%1[^#]#%1[^#]#%1[^#]###\n%n", a1, a2, a3, a4, &charsRead) == 4);
    input += charsRead;

    map.m[2][3] = a1[0];
    map.m[2][5] = a2[0];
    map.m[2][7] = a3[0];
    map.m[2][9] = a4[0];

    assert(sscanf(input, "  #%1[^#]#%1[^#]#%1[^#]#%1[^#]#\n", a1, a2, a3, a4) == 4);

    map.m[3][3] = a1[0];
    map.m[3][5] = a2[0];
    map.m[3][7] = a3[0];
    map.m[3][9] = a4[0];

    return map;
}

static void dump(Map map) {
    int hx = map.amp.x;
    int hy = map.amp.y;

    // printf("Energy used: %d\n", map.energy);

    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 13; ++x) {
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

    for (int x = 1; x < 12; ++x) {
        if (map.m[1][x] >= 'A' && map.m[1][x] <= 'D') {
            amps.as[amps.n].x = x;
            amps.as[amps.n].y = 1;
            ++amps.n;
        }
    }

    for (int x = 1; x < 12; ++x) {
        if (map.m[2][x] >= 'A' && map.m[2][x] <= 'D') {
            if ((x == 3 && (map.m[3][x] != 'A' || map.m[2][x] != 'A')) ||
                (x == 5 && (map.m[3][x] != 'B' || map.m[2][x] != 'B')) ||
                (x == 7 && (map.m[3][x] != 'C' || map.m[2][x] != 'C')) ||
                (x == 9 && (map.m[3][x] != 'D' || map.m[2][x] != 'D'))) {
                amps.as[amps.n].x = x;
                amps.as[amps.n].y = 2;
                ++amps.n;
            }
        }
    }

    for (int x = 1; x < 12; ++x) {
        if (map.m[3][x] >= 'A' && map.m[3][x] <= 'D') {
            if ((x == 3 && (map.m[3][x] != 'A')) ||
                (x == 5 && (map.m[3][x] != 'B')) ||
                (x == 7 && (map.m[3][x] != 'C')) ||
                (x == 9 && (map.m[3][x] != 'D'))) {
                amps.as[amps.n].x = x;
                amps.as[amps.n].y = 3;
                ++amps.n;
            }
        }
    }

    return amps;
}

static Maps moveOnce(Map map) {
    // printf("moveOnce from:\n");
    // dump(map);

    int x = map.amp.x;
    int y = map.amp.y;

    bool isInHallway = y == 1;

    bool isInRoom = y >= 2;

    char t = map.m[y][x];

    int destX =
        t == 'A'   ? 3
        : t == 'B' ? 5
        : t == 'C' ? 7
                   : 9;

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
        if (y == 3 || (map.m[3][x] == t)) {
            printf("Done!");
            return maps;
        }
    }

    if (isInRoom) {
        // Can we go to the hallway?
        bool empty = true;
        int ySteps = 0;
        for (int y2 = y - 1; y2 >= 1; --y2) {
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
                    if (map.m[1][x2] != 0) {
                        empty = false;
                        break;
                    }
                }
            } else {
                for (int x2 = x - 1; x2 >= destX; --x2) {
                    if (map.m[1][x2] != 0) {
                        empty = false;
                        break;
                    }
                }
            }

            if (empty) {
                // yes, can we enter the room?
                if (map.m[3][destX] == 0) {
                    // Bottom of room.
                    maps.ms[maps.n] = map;

                    maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + ySteps + 2);

                    maps.ms[maps.n].amp.x = destX;
                    maps.ms[maps.n].amp.y = 3;

                    maps.ms[maps.n].m[y][x] = 0;
                    maps.ms[maps.n].m[3][destX] = t;

                    ++maps.n;
                    assert(maps.n < 300);

                    return maps;

                } else if (map.m[3][destX] == t && map.m[2][destX] == 0) {
                    // Top of room.
                    maps.ms[maps.n] = map;

                    maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + ySteps + 1);

                    maps.ms[maps.n].amp.x = destX;
                    maps.ms[maps.n].amp.y = 2;

                    maps.ms[maps.n].m[y][x] = 0;
                    maps.ms[maps.n].m[2][destX] = t;

                    ++maps.n;
                    assert(maps.n < 300);

                    return maps;
                }
            }

            // Nope, pick a valid position in the hallway
            for (int x2 = x - 1; x2 >= 0; --x2) {
                if (map.m[1][x2] != 0) {
                    break;
                }

                if (x2 == 3 || x2 == 5 || x2 == 7 || x2 == 9) {
                    continue;
                }

                // printf("Could try (%d,%d)\n", x2, 1);
                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                maps.ms[maps.n].amp.x = x2;
                maps.ms[maps.n].amp.y = 1;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[1][x2] = t;
                ++maps.n;
                assert(maps.n < 300);
            }

            for (int x2 = x + 1; x2 < 13; ++x2) {
                if (map.m[1][x2] != 0) {
                    break;
                }

                if (x2 == 3 || x2 == 5 || x2 == 7 || x2 == 9) {
                    continue;
                }

                // printf("Could try (%d,%d)\n", x2, 1);
                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(x2 - x) + ySteps);

                maps.ms[maps.n].amp.x = x2;
                maps.ms[maps.n].amp.y = 1;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[1][x2] = t;
                ++maps.n;
                assert(maps.n < 300);
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
                if (map.m[1][x2] != 0) {
                    xEmpty = false;
                    break;
                }
            }
        } else {
            for (int x2 = x - 1; x2 >= destX; --x2) {
                if (map.m[1][x2] != 0) {
                    xEmpty = false;
                    break;
                }
            }
        }
        // printf("xEmpty to %d? %s\n", destX, xEmpty ? "true" : "false");

        if (xEmpty) {
            if (map.m[3][destX] == 0) {
                // printf("Could try %d,%d\n", 3, destX);

                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + abs(3 - y));

                maps.ms[maps.n].amp.x = destX;
                maps.ms[maps.n].amp.y = 3;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[3][destX] = t;

                ++maps.n;
                assert(maps.n < 300);

                return maps;
            } else if (map.m[3][destX] == t && map.m[2][destX] == 0) {
                // printf("Could try %d,%d\n", 2, destX);

                maps.ms[maps.n] = map;

                maps.ms[maps.n].energy += energyPerStep * (abs(destX - x) + abs(2 - y));

                maps.ms[maps.n].amp.x = destX;
                maps.ms[maps.n].amp.y = 2;

                maps.ms[maps.n].m[y][x] = 0;
                maps.ms[maps.n].m[2][destX] = t;

                ++maps.n;
                assert(maps.n < 300);

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

static int64_t partOne(Map map, int64_t energy) {
    Amps amps = findMalplacedAmps(map);

    if (amps.n == 0) {

        ++ccc;

        if ((ccc % 20000) == 0) {
            printf("Done: %lld - %d\n", ccc, map.energy);
            // printf("** All done! **\n");
            dump(map);
            // printf("****\n");
        }
        return map.energy < energy ? map.energy : energy;
    }

    for (int i = 0; i < amps.n; ++i) {
        // printf("\nMalplaced amp: (%d,%d): %c\n",
        //        amps.as[i].x, amps.as[i].y,
        //        map.m[amps.as[i].y][amps.as[i].x]);

        Map map2 = map;
        map2.amp = amps.as[i];

        Maps maps = moveOnce(map2);

        // printf("Possible new maps: %d\n", maps.n);

        for (int j = 0; j < maps.n; ++j) {
            int64_t e2 = partOne(maps.ms[j], energy);

            if (e2 < energy) {
                energy = e2;
            }
        }
    }

    return energy;

    // Move middle B
    map.amp.x = 7;
    map.amp.y = 2;
    Maps maps = moveOnce(map);
    dump(maps.ms[1]);

    // Move middle C
    Map map2 = maps.ms[1];
    map2.amp.x = 5;
    map2.amp.y = 2;
    Maps maps2 = moveOnce(map2);
    dump(maps2.ms[0]);

    // Move middle D
    Map map3 = maps2.ms[0];
    map3.amp.x = 5;
    map3.amp.y = 3;
    Maps maps3 = moveOnce(map3);
    dump(maps3.ms[0]);

    // Move B again.
    Map map4 = maps3.ms[0];
    map4.amp.x = 4;
    map4.amp.y = 1;
    Maps maps4 = moveOnce(map4);
    dump(maps4.ms[0]);

    // Move other B.
    Map map5 = maps4.ms[0];
    map5.amp.x = 3;
    map5.amp.y = 2;
    Maps maps5 = moveOnce(map5);
    dump(maps5.ms[0]);

    // Move right D.
    Map map6 = maps5.ms[0];
    map6.amp.x = 9;
    map6.amp.y = 2;
    Maps maps6 = moveOnce(map6);
    dump(maps6.ms[0]);

    // Move right A.
    Map map7 = maps6.ms[0];
    map7.amp.x = 9;
    map7.amp.y = 3;
    Maps maps7 = moveOnce(map7);
    dump(maps7.ms[0]);

    // Move right hallway D.
    Map map8 = maps7.ms[0];
    map8.amp.x = 8;
    map8.amp.y = 1;
    Maps maps8 = moveOnce(map8);
    dump(maps8.ms[0]);

    // Move other hallway D.
    Map map9 = maps8.ms[0];
    map9.amp.x = 6;
    map9.amp.y = 1;
    Maps maps9 = moveOnce(map9);
    dump(maps9.ms[0]);

    // Move hallway A.
    Map map10 = maps9.ms[0];
    map10.amp.x = 10;
    map10.amp.y = 1;
    Maps maps10 = moveOnce(map10);
    dump(maps10.ms[0]);

    printf("Total used energy: %d\n", maps10.ms[0].energy);

    // for (int i = 0; i < maps10.n; ++i) {
    //     printf("Alt %i\n", i);
    //     dump(maps10.ms[i]);
    // }

    return -1;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Map map = parse(input);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(map, INT_MAX),
                   12521, -2);

    // Helpers_assert(PART2, Helpers_clock(),
    //                partTwo(n, xs),
    //                -1, -2);

    return 0;
}
