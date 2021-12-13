#include "Helpers.c"

#define FOLD_CAP 20
#define POINTS_CAP 1000

typedef struct {
    int pos;
    bool yAxis;
} Fold;

typedef struct {
    int16_t x;
    int16_t y;
} Point;

typedef struct {
    int h;
    int w;
    int nPoints;
    Point points[POINTS_CAP];
} Paper;

static int parse(const char *input, Paper *paper, Fold folds[FOLD_CAP]) {
    int x = 0;
    int y = 0;
    int charsRead = 0;

    paper->nPoints = 0;

    while (sscanf(input, "%d,%d\n%n", &x, &y, &charsRead) == 2) {
        paper->h = y > paper->h ? y : paper->h;
        paper->w = x > paper->w ? x : paper->w;

        paper->points[paper->nPoints++] = (Point){.x = x, .y = y};
        assert(paper->nPoints < POINTS_CAP);

        input += charsRead;
    }

    ++paper->h;
    ++paper->w;

    assert(paper->w & 1);
    assert(paper->h & 1);

    char xOrY = '0';
    int nFolds = 0;

    while (sscanf(input, "fold along %c=%d\n%n", &xOrY, &x, &charsRead) == 2) {
        folds[nFolds++] = (Fold){.pos = x, .yAxis = xOrY == 'y'};
        assert(nFolds < FOLD_CAP);

        input += charsRead;
    }

    return nFolds;
}

static void printDots(int w, int h, const bool dots[h][w]) {
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            printf("%c", dots[y][x] ? '#' : ' ');
        }
        printf("\n");
    }
}

static void fold(const Fold fold, Paper *paper) {
    if (fold.yAxis) {
        for (int i = 0; i < paper->nPoints; ++i) {
            if (paper->points[i].y > fold.pos) {
                paper->points[i].y = paper->h - paper->points[i].y - 1;
            }
        }

        paper->h /= 2;
    } else {
        for (int i = 0; i < paper->nPoints; ++i) {
            if (paper->points[i].x > fold.pos) {
                paper->points[i].x = paper->w - paper->points[i].x - 1;
            }
        }

        paper->w /= 2;
    }
}

static int dotsFromPaper(const Paper *paper, bool dots[paper->h][paper->w]) {
    memset(dots, false, sizeof(dots[0][0]) * paper->h * paper->w);

    int drawnOnce = 0;

    for (int i = 0; i < paper->nPoints; ++i) {
        int x = paper->points[i].x;
        int y = paper->points[i].y;

        if (y < paper->h && x < paper->w && !dots[y][x]) {
            dots[y][x] = true;
            ++drawnOnce;
        }
    }

    return drawnOnce;
}

// Modified from https://stackoverflow.com/a/21001712/390189
static int64_t checksumFromDots(int h, int w, const bool dots[h][w]) {
    int64_t r = ~0;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            r ^= dots[y][x];

            for (int i = 0; i < 8; i++) {
                int64_t t = ~((r & 1) - 1);
                r = (r >> 1) ^ (0xEDB88320 & t);
            }
        }
    }

    return ~r;
}

static int partOne(const Paper *unFoldedPaper, int nFolds, const Fold folds[nFolds]) {
    Paper paper = {0};
    memcpy(&paper, unFoldedPaper, sizeof(paper));

    fold(folds[0], &paper);

    bool dots[paper.h][paper.w];

    return dotsFromPaper(&paper, dots);
}

static int64_t partTwo(const Paper *unfoldedPaper, int nFolds, const Fold folds[nFolds]) {
    Paper paper = {0};
    memcpy(&paper, unfoldedPaper, sizeof(paper));

    for (int i = 0; i < nFolds; ++i) {
        fold(folds[i], &paper);
    }

    bool dots[paper.h][paper.w];

    dotsFromPaper(&paper, dots);

    printDots(paper.w, paper.h, dots);

    return checksumFromDots(paper.w, paper.h, dots);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Paper paper = {0};
    Fold folds[FOLD_CAP] = {0};

    int nFolds = parse(input, &paper, folds);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(&paper, nFolds, folds),
                   17, 814);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(&paper, nFolds, folds),
                   1478277218, 3900100774);

    return 0;
}
