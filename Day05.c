#define DAY 05
#define INPUT Line input[INPUT_CAP]
#define INPUT_CAP 4096

typedef struct {
    int x;
    int y;
} Point;

typedef Point Line[2];

typedef struct {
    int w;
    int h;
} DiagramSize;

#include "Runner.c"

static int parse(const char *inputString, Line lines[INPUT_CAP]) {
    int count = 0;
    int charsRead = 0;
    int filled = 0;

    while ((filled = sscanf(inputString, "%d,%d -> %d,%d\n%n",
                            &lines[count][0].x,
                            &lines[count][0].y,
                            &lines[count][1].x,
                            &lines[count][1].y,
                            &charsRead)) != EOF) {
        assert(filled == 4 && "parseInput: Failed to parse input");

        inputString += charsRead;

        ++count;

        assert(count < INPUT_CAP);
    }

    return count;
}

static DiagramSize diagramSizeFromLines(int count, const Line lines[count]) {
    int maxX = 0;
    int maxY = 0;

    for (int i = 0; i < count; ++i) {
        maxX = lines[i][0].x > maxX   ? lines[i][0].x
               : lines[i][1].x > maxX ? lines[i][1].x
                                      : maxX;

        maxY = lines[i][0].y > maxY   ? lines[i][0].y
               : lines[i][1].y > maxY ? lines[i][1].y
                                      : maxY;
    }

    DiagramSize result = {.w = maxX + 1, .h = maxY + 1};

    return result;
}

static void drawLines(int nLines, const Line lines[nLines], int h, int w, int diagram[h][w]) {
    for (int i = 0; i < nLines; ++i) {
        int x1 = lines[i][0].x;
        int x2 = lines[i][1].x;

        int y1 = lines[i][0].y;
        int y2 = lines[i][1].y;

        int xStep = x1 == x2  ? 0
                    : x1 < x2 ? 1
                              : -1;

        int yStep = y1 == y2  ? 0
                    : y1 < y2 ? 1
                              : -1;

        do {
            ++diagram[y1][x1];
            x1 += xStep;
            y1 += yStep;
        } while (x1 != x2 || y1 != y2);

        ++diagram[y2][x2];
    }
}

static int countOverlaps(int h, int w, const int diagram[h][w]) {
    int overlaps = 0;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (diagram[y][x] > 1) {
                ++overlaps;
            }
        }
    }

    return overlaps;
}

static Result partOne(int count, const Line lines[count]) {
    Line vhLines[count];
    memset(vhLines, 0, sizeof(Line) * count);

    int countVH = 0;

    for (int i = 0; i < count; ++i) {
        if ((lines[i][0].x == lines[i][1].x) || (lines[i][0].y == lines[i][1].y)) {
            memcpy(vhLines + countVH++, lines[i], sizeof(Line));
        }
    }

    DiagramSize size = diagramSizeFromLines(countVH, vhLines);

    int diagram[size.h][size.w];
    memset(diagram, 0, size.w * size.h * sizeof(int));

    drawLines(countVH, vhLines, size.h, size.w, diagram);

    return (Result){countOverlaps(size.h, size.w, diagram), 5, 7318};
}

static Result partTwo(int count, const Line lines[count]) {
    DiagramSize size = diagramSizeFromLines(count, lines);

    int diagram[size.h][size.w];
    memset(diagram, 0, size.w * size.h * sizeof(int));

    drawLines(count, lines, size.h, size.w, diagram);

    return (Result){countOverlaps(size.h, size.w, diagram), 12, 19939};
}
