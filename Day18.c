#include "Helpers.c"

#define CAP 100
#define NODES_CAP 30000

typedef struct NodeType {
    int value;
    struct NodeType *left;
    struct NodeType *right;
    struct NodeType *parent;
    int id;
} Node;

Node nodes[NODES_CAP] = {0};
int n = 0;

static int parse(const char *input, Node *numbers[CAP]) {
    int nNumbers = 0;
    n = 0;

    memset(nodes, 0, sizeof(nodes));

    while (*input != 0) {
        // printf("Parsing number %d\n", nNumbers);
        assert(nNumbers < CAP);

        Node *p = &nodes[n++];
        p->value = 0xdeadbeef;
        p->id = n - 1;

        assert(n < NODES_CAP);

        numbers[nNumbers++] = p;

        while (*input != 0 && *input != '\n') {
            char c = *(input++);
            assert(p != NULL && "Unbalanced snailfish number");
            // printf("%c - %d\n", c, n);

            if (c == '[') {
                assert(p->left == NULL && "Left already occupied");

                nodes[n].parent = p;
                p->left = &nodes[n++];
                p = p->left;
                p->id = n - 1;
                p->value = -1;
            } else if (c >= '0' && c <= '9') {
                char d = *input;

                if (d >= '0' && d <= '9') {
                    p->value = (c - '0') * 10 + (d - '0');
                    ++input;
                } else {
                    p->value = c - '0';
                }

                p = p->parent;

            } else if (c == ',') {

                assert(p->right == NULL && "Right already occupied");

                nodes[n].parent = p;
                p->right = &nodes[n++];
                p = p->right;
                p->id = n - 1;
                p->value = -1;

            } else if (c == ']') {
                assert(p->left != NULL && p->right != NULL && "Missing pair");
                p = p->parent;
            } else {
                assert(false && "Unknown character");
            }
        }

        assert(p == NULL && "Unbalanced snailfish number");

        ++input;
    }

    return nNumbers;
}

static Node *findLeftMostValue(Node *node) {
    bool triedLeft[NODES_CAP] = {false};
    bool triedRight[NODES_CAP] = {false};

    while (node != NULL) {
        if (node->left == NULL || node->right == NULL) {
            return node;
        } else {
            if (triedLeft[node->id] && triedRight[node->id]) {
                node = node->parent;
            } else if (!triedLeft[node->id]) {
                triedLeft[node->id] = true;
                node = node->left;
            } else {
                triedRight[node->id] = true;
                node = node->right;
            }
        }
    }

    return NULL;
}

static Node *findLeftMostValue2(Node *node) {
    bool triedLeft[NODES_CAP] = {false};
    bool triedRight[NODES_CAP] = {false};

    while (node != NULL) {
        if (node->left == NULL || node->right == NULL) {
            return node;
        } else {
            if (triedLeft[node->id] && triedRight[node->id]) {
                node = node->parent;
            } else if (!triedRight[node->id]) {
                triedRight[node->id] = true;
                node = node->right;
            } else {
                triedLeft[node->id] = true;
                node = node->left;
            }
        }
    }

    return NULL;
}

static bool explode(Node *root) {
    Node *c = root;
    int level = 0;

    bool triedLeft[NODES_CAP] = {false};
    bool triedRight[NODES_CAP] = {false};

    while (c != NULL) {
        assert((c->left != NULL && c->right != NULL) || (c->left == NULL && c->right == NULL));

        // printf("At level: %d\n", level);

        if (c->left == NULL || c->right == NULL) {
            // printf("Not a pair, backup up, %d\n", c->value);

            if (level <= 0) {
                printf("Nothing more to backup too\n");
                return false;
            }

            --level;
            // c = parents[level];
            c = c->parent;
        } else {
            if (level >= 4) {
                // printf("Woop, deep enough!\n");
                break;
            } else {
                if (triedLeft[c->id] && triedRight[c->id]) {
                    // printf("Already been here once, id: %d!\n", c->id);

                    --level;
                    // c = parents[level];
                    c = c->parent;
                    // return 2;
                } else if (!triedLeft[c->id]) {
                    // printf("Going left \n");
                    // parents[level] = c;
                    triedLeft[c->id] = true;
                    ++level;
                    c = c->left;
                } else {
                    // printf("Going right \n");
                    // parents[level] = c;
                    triedRight[c->id] = true;

                    ++level;
                    c = c->right;
                }
            }
        }
    }

    if (c == NULL) {
        // printf("Found no nested pairs\n");
        return false;
    }

    // printf("Found node id: %d, at level: %d\n", c->id, level);

    int leftNumber = c->left->value;
    int rightNumber = c->right->value;

    assert(leftNumber != -1337);
    assert(rightNumber != -1337); // Understand why deeper than 4 (after split).

    // printf("Left number %d, right number: %d, id: %d\n", leftNumber, rightNumber, c->id);

    // Find left most number from c
    Node *leftMostC = c->parent;
    Node *prev = c;

    while (leftMostC != NULL) {
        if (leftMostC->left == NULL) {
            // printf("Found left most value: %d\n", leftMostC->value);
            break;
        } else if (leftMostC->left == prev) {
            prev = leftMostC;
            leftMostC = leftMostC->parent;

        } else {
            // prev = leftMostC;
            // leftMostC = leftMostC->left;
            leftMostC = findLeftMostValue2(leftMostC->left);
            break;
        }
    }

    if (leftMostC != NULL) {
        // printf("Left most c value: %d, id: %d\n", leftMostC->value, leftMostC->parent->id);

        leftMostC->value += leftNumber;
    } else {
        // printf("Found no left most value\n");
    }

    // Find right most number from c
    Node *rightMostC = c->parent;
    prev = c;

    while (rightMostC != NULL) {
        if (rightMostC->right == NULL) {
            // printf("Found right most value: %d\n", rightMostC->value);
            break;
        } else if (rightMostC->right == prev) {
            prev = rightMostC;
            rightMostC = rightMostC->parent;
        } else {
            // prev = rightMostC;
            // rightMostC = rightMostC->right;
            rightMostC = findLeftMostValue(rightMostC->right);
            break;
        }
    }

    if (rightMostC != NULL) {
        // printf("Right most c value: %d, id: %d\n", rightMostC->value, rightMostC->parent->id);
        rightMostC->value += rightNumber;
    } else {
        // printf("Found no right most value\n");
    }

    c->left = NULL;
    c->right = NULL;
    c->value = 0;

    return true;
}

static Node *addition(Node *nodeA, Node *nodeB) {
    Node *root = &nodes[n++];
    root->value = 0xdeadbeef;
    root->id = n - 1;

    root->left = nodeA;
    root->right = nodeB;

    nodeA->value = -1;
    nodeA->parent = root;

    nodeB->value = -1;
    nodeB->parent = root;

    return root;
}

static bool split(Node *node) {
    bool triedLeft[NODES_CAP] = {false};
    bool triedRight[NODES_CAP] = {false};

    Node *found = NULL;

    while (node != NULL) {
        if (node->left == NULL || node->right == NULL) {
            if (node->value >= 10) {
                found = node;
                break;
            } else {
                node = node->parent;
            }
        } else {
            if (triedLeft[node->id] && triedRight[node->id]) {
                node = node->parent;
            } else if (!triedLeft[node->id]) {
                triedLeft[node->id] = true;
                node = node->left;
            } else {
                triedRight[node->id] = true;
                node = node->right;
            }
        }
    }

    if (found != NULL) {
        // printf("Found node to split, %d: %d\n", found->id, found->value);

        assert(found->value != -1337);

        Node *left = &nodes[n++];
        left->id = n - 1;
        left->value = found->value / 2;
        left->parent = found;

        assert(n < NODES_CAP);

        Node *right = &nodes[n++];
        right->id = n - 1;
        right->value = ((float)found->value / 2) + 0.5;
        right->parent = found;

        assert(n < NODES_CAP);

        found->left = left;
        found->right = right;
        found->value = -1337;

        return true;
    } else {
        // printf("Found no node to split\n");
        return false;
    }
}

static void reduce(Node *number) {
    bool retry = true;
    while (retry) {
        retry = false;

        while (explode(number)) {
            retry = true;
            // printf("exploded\n");
            // dump(number, 0);
        }

        if (split(number)) {
            retry = true;
            // printf("splitted\n");
            // dump(number, 0);
        }
    }
}

static int magnitude(const Node *node) {
    if (node->left == NULL || node->right == NULL) {
        return node->value;
    }
    return 3 * magnitude(node->left) + 2 * magnitude(node->right);
}

static void dump(const Node *node, int level) {
    if (node->left == NULL || node->right == NULL) {
        printf("%d", node->value);
    } else {
        printf("[");
        dump(node->left, level + 1);
        printf(",");
        dump(node->right, level + 1);
        // printf("] <%d|%d>", level, node->id);
        // printf("] %d", node->id);
        printf("]");
    }

    if (node->value == (int)0xdeadbeef) {
        printf("\n");
    }
}

static int partOne(int nNumbers, Node *numbers[CAP]) {
    Node *number = numbers[0];

    for (int i = 1; i < nNumbers; ++i) {
        number = addition(number, numbers[i]);
        reduce(number);
    }

    printf("reduced\n");
    dump(number, 0);
    // [[[[6,7],[6,8]],[[0,7],[0,7]]],[[5,5],9]]

    int m = magnitude(number);

    printf("magnitude: %d, nodes: %d\n", magnitude(number), n);

    return m;
}

static int partTwo(int nNumbers123, const char *input) {
    int largestMagnitude = INT_MIN;

    for (int i = 0; i < nNumbers123; ++i) {
        for (int j = 0; j < nNumbers123; ++j) {
            if (i != j) {

                Node *numbers[CAP];
                parse(input, numbers);

                Node *number = addition(numbers[i], numbers[j]);
                reduce(number);

                int m = magnitude(number);

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

    Node *numbers[CAP] = {NULL};
    int nNumbers = parse(input, numbers);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(nNumbers, numbers),
                   4140, 4289);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(nNumbers, input),
                   3993, 4807);

    return 0;
}
