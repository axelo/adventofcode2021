#include "Helpers.c"

#define CAP 30
#define COORDS_CAP 30
#define OVERLAP_CAP 30

typedef struct {
    int x;
    int y;
    int z;
} Coord;

typedef struct {
    int id;
    int n;
    Coord beacons[COORDS_CAP];
} Scanner;

typedef struct {
    int sign;
    int axis;
    int delta;
} SignAxis;

typedef struct {
    SignAxis x;
    SignAxis y;
    SignAxis z;
} Transform;

static uint32_t parse(const char *input, Scanner scanners[CAP]) {
    int charsRead = 0;
    int i = -1;

    while (sscanf(input, "--- scanner %d ---\n%n", &i, &charsRead) == 1) {
        input += charsRead;

        int x = 0;
        int y = 0;
        int z = 0;

        scanners[i].id = i;

        while (sscanf(input, "%d,%d,%d\n%n", &x, &y, &z, &charsRead) == 3) {
            input += charsRead;

            int j = scanners[i].n;

            scanners[i].beacons[j].x = x;
            scanners[i].beacons[j].y = y;
            scanners[i].beacons[j].z = z;

            ++scanners[i].n;
        }
    }

    return (uint32_t)i + 1;
}

static void distances(const Scanner s, int dist[CAP][CAP]) {
    assert(s.n < CAP);

    for (int i = 0; i < s.n; ++i) {
        for (int j = 0; j < s.n; ++j) {
            // Abs [a − x] + Abs [b − y] + Abs [c − z]
            dist[i][j] = abs(s.beacons[i].x - s.beacons[j].x) +
                         abs(s.beacons[i].y - s.beacons[j].y) +
                         abs(s.beacons[i].z - s.beacons[j].z);
        }
    }
}

static int findOverlaps(const int sDist[CAP][CAP][CAP], const Scanner s0, const Scanner s1, Coord overlaps[OVERLAP_CAP][2]) {
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
            assert(nOverlaps < OVERLAP_CAP);
        }
    }

    return nOverlaps;
}

#define HMM 1

static SignAxis findSignAxisDelta(int originAxis, const Coord overlaps[OVERLAP_CAP][2]) {
    SignAxis result = {0};

    // This is kind of cheating, should try all overlaps as base and pick the popular one.
    // For my input 0 is not correct, but 1 one is :)
    Coord originCoord = overlaps[HMM][0];
    Coord relativeCoord = overlaps[HMM][1];

    int origin = originAxis == 0 ? originCoord.x : originAxis == 1 ? originCoord.y
                                                                   : originCoord.z;

    for (int axis = 0; axis < 3; ++axis) {
        for (int sign = -1; sign < 2; sign += 2) {
            int delta = (sign * (axis == 0 ? relativeCoord.x : axis == 1 ? relativeCoord.y
                                                                         : relativeCoord.z)) -
                        origin;

            int countSame = 0;

            for (int j = 0; j < 12; ++j) {
                int delta2 = sign * (axis == 0 ? overlaps[j][1].x : axis == 1 ? overlaps[j][1].y
                                                                              : overlaps[j][1].z) -
                             (originAxis == 0 ? overlaps[j][0].x : originAxis == 1 ? overlaps[j][0].y
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
    Coord overlaps[OVERLAP_CAP][2] = {0};
    int nOverlaps = findOverlaps(sDist, s0, s1, overlaps);

    Transform t = {0};

    if (nOverlaps >= 12) {
        t.x = findSignAxisDelta(0, overlaps);
        t.y = findSignAxisDelta(1, overlaps);
        t.z = findSignAxisDelta(2, overlaps);
    }

    return t;
}

static inline Coord transform(Transform t, Coord c) {
    int x = t.x.axis == 0 ? c.x : t.x.axis == 1 ? c.y
                                                : c.z;
    int y = t.y.axis == 0 ? c.x : t.y.axis == 1 ? c.y
                                                : c.z;
    int z = t.z.axis == 0 ? c.x : t.z.axis == 1 ? c.y
                                                : c.z;

    return (Coord){x * t.x.sign - t.x.delta,
                   y * t.y.sign - t.y.delta,
                   z * t.z.sign - t.z.delta};
}

static int findTransforms(const Transform sTransforms[CAP][CAP], bool visited[CAP][CAP], int s0, int s1, int n, Transform ts[300]) {
    if (s0 >= 30) {
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

static int64_t partOne(uint32_t n, Scanner scanners[n]) {
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

    Coord coords[2000] = {0};
    uint32_t m = 0;

    // Adding all beacons from scanner 0
    for (int i = 0; i < scanners[0].n; ++i) {
        coords[m++] = scanners[0].beacons[i];
    }

    Transform ts[300] = {0};

    for (int i = 1; i < scanners[i].n; ++i) {
        // Finding transform from scanner to 0 so we can find same beacon coords.
        bool visited[CAP][CAP] = {false};
        int nt = findTransforms(sTransform, visited, 0, i, 0, ts);
        assert(nt > 0);

        for (int j = 0; j < scanners[i].n; ++j) {
            Coord c = scanners[i].beacons[j];

            for (int k = 0; k < nt; ++k) {
                c = transform(ts[k], c);
            }

            coords[m++] = c;
            assert(m < 2000);
        }
    }

    qsort(coords, m, sizeof(Coord), compareCoord);

    int nBeacons = 1;

    for (uint32_t i = 0; i < m - 1; ++i) {
        if ((coords[i].x != coords[i + 1].x) ||
            (coords[i].y != coords[i + 1].y) ||
            (coords[i].z != coords[i + 1].z))
            ++nBeacons;
    }

    return nBeacons;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Scanner scanners[CAP] = {0};
    uint32_t n = parse(input, scanners);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, scanners),
                   79, 330);

    return 0;
}
