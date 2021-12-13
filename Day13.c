#include "Helpers.c"

#define DOTS_CAP 1400
#define FOLD_CAP 20

typedef struct {
    int pos;
    bool yAxis;
} Fold;

typedef struct {
    int h;
    int w;
    bool dots[DOTS_CAP][DOTS_CAP];
} Paper;

static int parse(const char *input, Paper *paper, Fold folds[FOLD_CAP]) {
    int x = 0;
    int y = 0;
    int charsRead = 0;

    while (sscanf(input, "%d,%d\n%n", &x, &y, &charsRead) == 2) {
        assert(y < DOTS_CAP);
        assert(x < DOTS_CAP);

        paper->dots[y][x] = true;
        paper->h = y > paper->h ? y : paper->h;
        paper->w = x > paper->w ? x : paper->w;

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

static void printPaper(const Paper *paper) {
    for (int y = 0; y < paper->h; ++y) {
        for (int x = 0; x < paper->w; ++x) {
            printf("%c", paper->dots[y][x] ? '#' : '.');
        }
        printf("\n");
    }
}

static void fold(const Fold fold, Paper *paper) {
    if (fold.yAxis) {
        for (int y = fold.pos + 1; y < paper->h; ++y) {
            for (int x = 0; x < paper->w; ++x) {
                paper->dots[paper->h - y - 1][x] |= paper->dots[y][x];
            }
        }

        paper->h /= 2;
    } else {
        for (int y = 0; y < paper->h; ++y) {
            for (int x = fold.pos + 1; x < paper->w; ++x) {
                paper->dots[y][paper->w - x - 1] |= paper->dots[y][x];
            }
        }

        paper->w /= 2;
    }
}

static int countDots(const Paper *paper) {
    int count = 0;

    for (int y = 0; y < paper->h; ++y) {
        for (int x = 0; x < paper->w; ++x) {
            if (paper->dots[y][x]) {
                ++count;
            }
        }
    }

    return count;
}

// Modified from https://stackoverflow.com/a/21001712/390189
static int64_t checksum(const Paper *paper) {
    int64_t r = ~0;

    for (int y = 0; y < paper->h; ++y) {
        for (int x = 0; x < paper->w; ++x) {
            r ^= paper->dots[y][x];

            for (int i = 0; i < 8; i++) {
                int64_t t = ~((r & 1) - 1);
                r = (r >> 1) ^ (0xEDB88320 & t);
            }
        }
    }

    return ~r;
}

static int partOne(const Paper *paper, int nFolds, const Fold folds[nFolds]) {
    Paper copy;
    memcpy(&copy, paper, sizeof(copy));

    fold(folds[0], &copy);

    return countDots(&copy);
}

static int64_t partTwo(const Paper *paper, int nFolds, const Fold folds[nFolds]) {
    Paper copy;
    memcpy(&copy, paper, sizeof(copy));

    for (int i = 0; i < nFolds; ++i) {
        fold(folds[i], &copy);
    }

    printPaper(&copy);

    return checksum(&copy);
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
