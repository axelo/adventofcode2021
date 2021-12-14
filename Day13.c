#include "Helpers.c"

#define FOLD_CAP 20
#define POINTS_CAP 1000

typedef struct {
    uint16_t pos;
    bool yAxis;
} Fold;

typedef struct {
    uint16_t x;
    uint16_t y;
} Point;

typedef struct {
    uint16_t h;
    uint16_t w;
    int nPoints;
    Point points[POINTS_CAP];
} Paper;

static int parse(const char *input, Paper *paper, Fold folds[FOLD_CAP]) {
    uint16_t x = 0;
    uint16_t y = 0;
    int charsRead = 0;

    paper->nPoints = 0;

    while (sscanf(input, "%hd,%hd\n%n", &x, &y, &charsRead) == 2) {
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

    while (sscanf(input, "fold along %c=%hd\n%n", &xOrY, &x, &charsRead) == 2) {
        folds[nFolds++] = (Fold){.pos = x, .yAxis = xOrY == 'y'};
        assert(nFolds < FOLD_CAP);

        input += charsRead;
    }

    return nFolds;
}

static int dotsFromPaper(const Paper *paper, bool dots[paper->h][paper->w]) {
    memset(dots, false, sizeof(dots[0][0]) * paper->h * paper->w);

    int drawnOnce = 0;

    for (int i = 0; i < paper->nPoints; ++i) {
        int x = paper->points[i].x;
        int y = paper->points[i].y;

        assert(x < paper->w && y < paper->h);

        if (!dots[y][x]) {
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

static int64_t printPaper(const Paper *paper) {
    bool dots[paper->h][paper->w];
    dotsFromPaper(paper, dots);

    for (int y = 0; y < paper->h; ++y) {
        for (int x = 0; x < paper->w; ++x) {
            printf(dots[y][x] ? "#" : " ");
        }
        printf("\n");
    }

    return checksumFromDots(paper->h, paper->w, dots);
}

static void fold(const Fold fold, Paper *paper) {
    if (fold.yAxis) {
        for (int i = 0; i < paper->nPoints; ++i) {
            if (paper->points[i].y > fold.pos) {
                paper->points[i].y = paper->h - paper->points[i].y - 1;
            }
        }

        paper->h = fold.pos;
    } else {
        for (int i = 0; i < paper->nPoints; ++i) {
            if (paper->points[i].x > fold.pos) {
                paper->points[i].x = paper->w - paper->points[i].x - 1;
            }
        }

        paper->w = fold.pos;
    }
}

static int partOne(const Paper *unfoldedPaper, int nFolds, const Fold folds[nFolds]) {
    Paper paper = {0};
    memcpy(&paper, unfoldedPaper, sizeof(paper));

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

    return printPaper(&paper);
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
