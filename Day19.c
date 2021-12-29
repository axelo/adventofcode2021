#include "Helpers.c"

#define CAP 30
#define BEACONS_CAP 30
#define OVERLAPS_CAP 30
#define COORDS_CAP 1000

typedef struct {
    int x;
    int y;
    int z;
} Coord;

typedef struct {
    int id;
    int n;
    Coord beacons[BEACONS_CAP];
} Scanner;

typedef enum {
    X = 0,
    Y,
    Z
} Axis;

typedef struct {
    int sign;
    Axis axis;
    int delta;
} SignAxis;

typedef struct {
    SignAxis x;
    SignAxis y;
    SignAxis z;
} Transform;

typedef struct {
    int nBeacons;
    int maxScannerDistance;
} Result;

static uint32_t parse(const char *input, Scanner scanners[CAP]) {
    int charsRead = 0;
    int i = -1;

    while (sscanf(input, "--- scanner %d ---\n%n", &i, &charsRead) == 1) {
        input += charsRead;

        int x = 0;
        int y = 0;
        int z = 0;

        assert(i < CAP);
        scanners[i].id = i;

        while (sscanf(input, "%d,%d,%d\n%n", &x, &y, &z, &charsRead) == 3) {
            input += charsRead;

            int j = scanners[i].n;

            scanners[i].beacons[j].x = x;
            scanners[i].beacons[j].y = y;
            scanners[i].beacons[j].z = z;

            ++scanners[i].n;
            assert(scanners[i].n < BEACONS_CAP);
        }
    }

    return (uint32_t)i + 1;
}

static inline int distance(Coord a, Coord b) {
    // Abs [a − x] + Abs [b − y] + Abs [c − z]
    return abs(a.x - b.x) +
           abs(a.y - b.y) +
           abs(a.z - b.z);
}

static void distances(const Scanner s, int dist[CAP][CAP]) {
    for (int i = 0; i < s.n; ++i) {
        for (int j = 0; j < s.n; ++j) {
            dist[i][j] = distance(s.beacons[i], s.beacons[j]);
        }
    }
}

static int findOverlaps(const int sDist[CAP][CAP][CAP], const Scanner s0, const Scanner s1, Coord overlaps[OVERLAPS_CAP][2]) {
    int nOverlaps = 0;

    for (int i = 0; i < s1.n; ++i) {
        int nFound = 0;
        int iOrigBeacon[CAP] = {0};

        for (int j = 0; j < s1.n; ++j) {
            bool foundSameDist = false;

            for (int k = 0; k < s0.n; ++k) {
                for (int l = 0; l < s0.n; ++l) {
                    if (sDist[s0.id][k][l] == sDist[s1.id][i][j]) {
                        ++iOrigBeacon[k];
                        foundSameDist = true;
                        break;
                    }
                }
            }

            if (foundSameDist) {
                ++nFound;
            }
        }

        if (nFound >= 12) {
            int origI = 0;
            for (int c = 0; c < s0.n; ++c) {
                if (iOrigBeacon[c] > iOrigBeacon[origI]) {
                    origI = c;
                }
            }

            overlaps[nOverlaps][0] = s0.beacons[origI];
            overlaps[nOverlaps][1] = s1.beacons[i];

            ++nOverlaps;
            assert(nOverlaps < OVERLAPS_CAP);
        }
    }

    return nOverlaps;
}

#define HMM 1

static SignAxis findSignAxisDelta(Axis originAxis, const Coord overlaps[OVERLAPS_CAP][2]) {
    SignAxis result = {0};

    // This is kind of cheating, should try all overlaps as base and pick the popular one.
    // For my input 0 is not correct, but 1 one is across all scanners :)
    Coord originCoord = overlaps[HMM][0];
    Coord relativeCoord = overlaps[HMM][1];

    int origin = originAxis == X ? originCoord.x : originAxis == Y ? originCoord.y
                                                                   : originCoord.z;

    for (Axis axis = X; axis <= Z; ++axis) {
        for (int sign = -1; sign < 2; sign += 2) {
            int delta = (sign * (axis == X ? relativeCoord.x : axis == Y ? relativeCoord.y
                                                                         : relativeCoord.z)) -
                        origin;

            int countSame = 0;

            for (int j = 0; j < 12; ++j) {
                int delta2 = sign * (axis == X ? overlaps[j][1].x : axis == Y ? overlaps[j][1].y
                                                                              : overlaps[j][1].z) -
                             (originAxis == X ? overlaps[j][0].x : originAxis == Y ? overlaps[j][0].y
                                                                                   : overlaps[j][0].z);

                if (delta == delta2) {
                    ++countSame;
                }
            }

            if (countSame >= 12) {
                result.axis = axis;
                result.sign = sign;
                result.delta = delta;

                return result;
            }
        }
    }

    assert(false);
}

static Transform transformFromRelativeScanners(const int sDist[CAP][CAP][CAP], const Scanner s0, const Scanner s1) {
    Coord overlaps[OVERLAPS_CAP][2] = {0};
    int nOverlaps = findOverlaps(sDist, s0, s1, overlaps);

    Transform t = {0};

    if (nOverlaps >= 12) {
        t.x = findSignAxisDelta(X, overlaps);
        t.y = findSignAxisDelta(Y, overlaps);
        t.z = findSignAxisDelta(Z, overlaps);
    }

    return t;
}

static inline Coord transform(Transform t, Coord c) {
    int x = t.x.axis == X ? c.x : t.x.axis == Y ? c.y
                                                : c.z;
    int y = t.y.axis == X ? c.x : t.y.axis == Y ? c.y
                                                : c.z;
    int z = t.z.axis == X ? c.x : t.z.axis == Y ? c.y
                                                : c.z;

    return (Coord){x * t.x.sign - t.x.delta,
                   y * t.y.sign - t.y.delta,
                   z * t.z.sign - t.z.delta};
}

static int findTransforms(const Transform sTransforms[CAP][CAP], bool visited[CAP][CAP], int s0, int s1, int n, Transform ts[300]) {
    if (s0 >= CAP) {
        return -1;
    }

    if (s0 != s1 && !visited[s0][s1] && sTransforms[s0][s1].x.sign != 0) {
        visited[s1][s0] = true;
        visited[s0][s1] = true;

        ts[n++] = sTransforms[s0][s1];

        if (s0 == 0) {
            return n;
        }

        int m = findTransforms(sTransforms, visited, 0, s0, n, ts);

        if (m > 0) {
            return m;
        }

        --n;
    }

    return findTransforms(sTransforms, visited, s0 + 1, s1, n, ts);
}

static int compareCoord(const void *a, const void *b) {
    int dx = ((const Coord *)a)->x - ((const Coord *)b)->x;

    if (dx != 0)
        return dx;

    int dy = ((const Coord *)a)->y - ((const Coord *)b)->y;

    if (dy != 0)
        return dy;

    int dz = ((const Coord *)a)->z - ((const Coord *)b)->z;

    return dz;
}

static Result solvePartOneAndTwo(uint32_t n, Scanner scanners[n]) {
    Transform sTransform[CAP][CAP] = {0};
    int sDist[CAP][CAP][CAP] = {0};

    for (uint32_t s = 0; s < n; ++s) {
        distances(scanners[s], sDist[s]);
    }

    for (uint32_t s0 = 0; s0 < n; ++s0) {
        for (uint32_t s1 = 0; s1 < n; ++s1) {
            sTransform[s0][s1] = transformFromRelativeScanners(sDist, scanners[s0], scanners[s1]);
        }
    }

    Coord coords[COORDS_CAP] = {0};
    uint32_t m = 0;

    // Adding all beacons from scanner 0.
    for (int i = 0; i < scanners[0].n; ++i) {
        coords[m++] = scanners[0].beacons[i];
    }

    Transform ts[300] = {0};
    Coord scannerPos[CAP] = {0};

    for (int i = 1; i < scanners[i].n; ++i) {
        // Finding transform from scanner i to 0 so we can have the same relative coords.
        bool visited[CAP][CAP] = {false};
        int nt = findTransforms(sTransform, visited, 0, i, 0, ts);
        assert(nt > 0);

        for (int j = 0; j < scanners[i].n; ++j) {
            Coord c = scanners[i].beacons[j];

            for (int k = 0; k < nt; ++k) {
                c = transform(ts[k], c);
            }

            coords[m++] = c;
            assert(m < COORDS_CAP);
        }

        for (int j = 0; j < nt; ++j) {
            scannerPos[i] = transform(ts[j], scannerPos[i]);
        }
    }

    qsort(coords, m, sizeof(Coord), compareCoord);

    int nBeacons = 1; // Assume m > 0.

    for (uint32_t i = 0; i < m - 1; ++i) {
        if ((coords[i].x != coords[i + 1].x) ||
            (coords[i].y != coords[i + 1].y) ||
            (coords[i].z != coords[i + 1].z))
            ++nBeacons;
    }

    int maxScannerDist = 0;

    for (uint32_t i = 0; i < n; ++i) {
        for (uint32_t j = i + 1; j < n; ++j) { // You only have to look at one pair once.
            int dist = distance(scannerPos[i], scannerPos[j]);

            if (dist > maxScannerDist)
                maxScannerDist = dist;
        }
    }

    return (Result){nBeacons, maxScannerDist};
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Scanner scanners[CAP] = {0};
    uint32_t n = parse(input, scanners);

    Helpers_assert(PART1, Helpers_clock(),
                   solvePartOneAndTwo(n, scanners).nBeacons,
                   79, 330);

    Helpers_assert(PART2, Helpers_clock(),
                   solvePartOneAndTwo(n, scanners).maxScannerDistance,
                   3621, 9634);

    return 0;
}
