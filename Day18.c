#include "Helpers.c"

#define NUMBERS_CAP 100
#define NODES_CAP 2000
#define NODES_REUSE_CAP 16 // Must be a power of 2.

typedef struct {
    uint16_t parent;
    uint16_t left;
    uint16_t right;
    uint16_t value;
} Node;

typedef struct {
    int n;
    Node ns[NODES_CAP];
    int nReuse;
    int reuse[NODES_REUSE_CAP];
} Nodes;

static int parse(const char *input, Nodes *nodes, int numbers[NUMBERS_CAP]) {
    int nNumbers = 0;
    nodes->n = 1;

    int current = nodes->n;

    while (*input != 0) {
        numbers[nNumbers++] = nodes->n;
        assert(nNumbers <= NUMBERS_CAP);

        current = nodes->n;

        ++nodes->n;
        assert(nodes->n < NODES_CAP);

        while (*input != 0 && *input != '\n') {
            char c = *(input++);
            assert(current != 0 && "Unbalanced snailfish number");

            if (c == '[') {
                assert(nodes->ns[current].left == 0 && "Left already occupied");

                int left = nodes->n++;
                nodes->ns[left].parent = current;

                nodes->ns[current].left = left;

                current = left;
            } else if (c >= '0' && c <= '9') {
                nodes->ns[current].value = c - '0';

                current = nodes->ns[current].parent;
            } else if (c == ',') {
                assert(nodes->ns[current].right == 0 && "Right already occupied");

                int right = nodes->n++;
                nodes->ns[right].parent = current;

                nodes->ns[current].right = right;

                current = right;
            } else if (c == ']') {
                assert(nodes->ns[current].left != 0 && nodes->ns[current].right != 0 && "Missing pair");

                current = nodes->ns[current].parent;
            } else {
                assert(false && "Unknown character");
            }
        }

        assert(current == 0 && "Unbalanced snailfish number");

        ++input;
    }

    return nNumbers;
}

static inline int emptyNodeIndex(Nodes *nodes) {
    int i = (nodes->nReuse - 1) & (NODES_REUSE_CAP - 1);
    int j = nodes->reuse[i];

    if (j) {
        nodes->reuse[i] = -1;
        nodes->nReuse = i;
        return j;
    } else {
        return nodes->n++;
    }
}

static inline void markReusableNodeIndex(int i, Nodes *nodes) {
    nodes->reuse[nodes->nReuse] = i;
    nodes->nReuse = (nodes->nReuse + 1) & (NODES_REUSE_CAP - 1);
}

static int addition(int ia, int ib, Nodes *nodes) {
    int root = emptyNodeIndex(nodes);

    nodes->ns[root].parent = 0;
    nodes->ns[root].left = ia;
    nodes->ns[root].right = ib;

    nodes->ns[ia].parent = root;
    nodes->ns[ib].parent = root;

    return root;
}

static int findLeftMostNumberNode(int i, const Nodes *nodes) {
    if (i < 0) {
        return -1;
    } else if (nodes->ns[i].left == 0) {
        return i;
    } else {
        int left = findLeftMostNumberNode(nodes->ns[i].left, nodes);

        return left != -1
                   ? left
                   : findLeftMostNumberNode(nodes->ns[i].right, nodes);
    }
}

static int findRightMostNumberNode(int i, const Nodes *nodes) {
    if (i < 0) {
        return -1;
    } else if (nodes->ns[i].left == 0) {
        return i;
    } else {
        int right = findRightMostNumberNode(nodes->ns[i].right, nodes);

        return right != -1
                   ? right
                   : findRightMostNumberNode(nodes->ns[i].left, nodes);
    }
}

static int findNodeToExplode(int i, int level, const Nodes *nodes) {
    if (i < 0 || nodes->ns[i].left == 0) {
        return -1;
    } else if (level >= 4) {
        return i;
    } else {
        int left = findNodeToExplode(nodes->ns[i].left, level + 1, nodes);

        return left != -1
                   ? left
                   : findNodeToExplode(nodes->ns[i].right, level + 1, nodes);
    }
}

static bool explode(int i, Nodes *nodes) {
    int j = findNodeToExplode(i, 0, nodes);

    if (j == -1) {
        return false;
    } else {
        int closestParentWithLeft = nodes->ns[j].parent;
        int prevChild = j;
        int rightMostNumberNode = -1;

        while (closestParentWithLeft != 0) {
            if (nodes->ns[closestParentWithLeft].left == prevChild) {
                prevChild = closestParentWithLeft;
                closestParentWithLeft = nodes->ns[closestParentWithLeft].parent;
            } else {
                rightMostNumberNode = findRightMostNumberNode(nodes->ns[closestParentWithLeft].left, nodes);
                break;
            }
        }

        int closestParentWithRight = nodes->ns[j].parent;
        prevChild = j;
        int leftMostNumberNode = -1;

        while (closestParentWithRight != 0) {
            if (nodes->ns[closestParentWithRight].right == 0) {
                break;
            } else if (nodes->ns[closestParentWithRight].right == prevChild) {
                prevChild = closestParentWithRight;
                closestParentWithRight = nodes->ns[closestParentWithRight].parent;
            } else {
                leftMostNumberNode = findLeftMostNumberNode(nodes->ns[closestParentWithRight].right, nodes);
                break;
            }
        }

        if (rightMostNumberNode != -1) {
            nodes->ns[rightMostNumberNode].value += nodes->ns[nodes->ns[j].left].value;
        }

        if (leftMostNumberNode != -1) {
            nodes->ns[leftMostNumberNode].value += nodes->ns[nodes->ns[j].right].value;
        }

        markReusableNodeIndex(nodes->ns[j].left, nodes);
        markReusableNodeIndex(nodes->ns[j].right, nodes);

        nodes->ns[j].left = 0;
        nodes->ns[j].right = 0;
        nodes->ns[j].value = 0;

        return true;
    }
}

static int findNodeToSplit(int i, const Nodes *nodes) {
    if (i <= 0) {
        return -1;
    } else if (nodes->ns[i].left == 0) {
        return nodes->ns[i].value >= 10 ? i : -1;
    } else {
        int left = findNodeToSplit(nodes->ns[i].left, nodes);

        return left != -1 ? left : findNodeToSplit(nodes->ns[i].right, nodes);
    }
}

static bool split(int i, Nodes *nodes) {
    int j = findNodeToSplit(i, nodes);

    if (j != -1) {
        int left = emptyNodeIndex(nodes);
        nodes->ns[left].parent = j;
        nodes->ns[left].value = nodes->ns[j].value / 2;

        assert(nodes->n < NODES_CAP);

        int right = emptyNodeIndex(nodes);
        nodes->ns[right].parent = j;
        nodes->ns[right].value = ((float)nodes->ns[j].value / 2) + 0.5;

        assert(nodes->n < NODES_CAP);

        nodes->ns[j].left = left;
        nodes->ns[j].right = right;

        return true;
    } else {
        return false;
    }
}

static void reduce(int i, Nodes *nodes) {
    do {
        while (explode(i, nodes)) {
        }
    } while (split(i, nodes));
}

static int magnitude(int i, const Nodes *nodes) {
    return nodes->ns[i].left == 0
               ? nodes->ns[i].value
               : 3 * magnitude(nodes->ns[i].left, nodes) + 2 * magnitude(nodes->ns[i].right, nodes);
}

static void dump(int i, const Nodes *nodes) {
    if (nodes->ns[i].left == 0) {
        printf("%d", nodes->ns[i].value);
    } else {
        printf("[");
        dump(nodes->ns[i].left, nodes);
        printf(",");
        dump(nodes->ns[i].right, nodes);
        printf("]");
    }

    if (nodes->ns[i].parent == 0) {
        printf("\n");
    }
}

static int partOne(const Nodes *nodes, int n, int numbers[n]) {
    Nodes ns;
    memcpy(&ns, nodes, sizeof(Nodes));

    int number = numbers[0];

    for (int i = 1; i < n; ++i) {
        number = addition(number, numbers[i], &ns);
        reduce(number, &ns);
    }

    dump(number, &ns);

    return magnitude(number, &ns);
}

static int partTwo(const Nodes *nodes, int n, int numbers[n]) {
    Nodes ns;

    int largestMagnitude = INT_MIN;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                memcpy(&ns, nodes, sizeof(Nodes));

                int number = addition(numbers[i], numbers[j], &ns);
                reduce(number, &ns);

                int m = magnitude(number, &ns);

                if (m > largestMagnitude) {
                    largestMagnitude = m;
                }
            }
        }
    }

    return largestMagnitude;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Nodes nodes = {0};
    int numbers[NUMBERS_CAP] = {0};
    int n = parse(input, &nodes, numbers);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(&nodes, n, numbers),
                   4140, 4289);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(&nodes, n, numbers),
                   3993, 4807);

    return 0;
}
