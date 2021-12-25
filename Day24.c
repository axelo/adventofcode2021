#include "Helpers.c"

#define CAP 300

typedef enum {
    Halt = 0,
    Inp,
    Mul,
    Eql,
    Mod,
    Div,
    Add
} Operation;

typedef enum {
    W = 0,
    X,
    Y,
    Z
} Var;

typedef enum {
    FromVar = 1,
    FromImm
} SrcType;

typedef struct {
    Operation op;
    Var dest;
    SrcType src;
    union {
        Var srcVar;
        int srcImm;
    };
} Instruction;

#define NOT_FOUND_CAP 500
#define INDEX_CAP 16384

static int64_t notFoundCache[14][INDEX_CAP][NOT_FOUND_CAP];
static int nNotFoundCache[14][INDEX_CAP] = {0};

static Var varFromChar(char c) {
    switch (c) {
    case 'w': return W;
    case 'x': return X;
    case 'y': return Y;
    case 'z': return Z;
    default: assert(false && "Unknown variable");
    }
}

static Operation opFromString(const char *str) {
    if (strcmp(str, "mul") == 0) {
        return Mul;
    } else if (strcmp(str, "eql") == 0) {
        return Eql;
    } else if (strcmp(str, "mod") == 0) {
        return Mod;
    } else if (strcmp(str, "add") == 0) {
        return Add;
    } else if (strcmp(str, "div") == 0) {
        return Div;
    } else {
        assert(false && "Unknown operation");
    }
}

static void parse(const char *input, Instruction instructions[14][CAP]) {
    int charsRead = 0;
    int filled = 1;
    char op[5] = {0};
    char dest[6] = {0};
    char src[6] = {0};
    int imm = 0;

    int n = 0;
    int chunk = -1;

    while (filled > 0) {
        filled = sscanf(input, "inp %1[wxyz]\n%n", dest, &charsRead);

        if (filled == 1 && charsRead > 0) {
            input += charsRead;

            n = 0;
            ++chunk;
            instructions[chunk][n++] = (Instruction){.op = Inp,
                                                     .dest = varFromChar(dest[0]),
                                                     .src = FromImm,
                                                     .srcImm = 0};
        } else {
            filled = sscanf(input, "%4s %1[wxyz] %d\n%n", op, dest, &imm, &charsRead);

            if (filled == 3 && charsRead > 0) {
                input += charsRead;

                instructions[chunk][n++] = (Instruction){.op = opFromString(op),
                                                         .dest = varFromChar(dest[0]),
                                                         .src = FromImm,
                                                         .srcImm = imm};
            } else {
                filled = sscanf(input, "%4s %1[wxyz] %1[wxyz]\n%n", op, dest, src, &charsRead);

                if (filled == 3 && charsRead > 0) {
                    input += charsRead;

                    instructions[chunk][n++] = (Instruction){.op = opFromString(op),
                                                             .dest = varFromChar(dest[0]),
                                                             .src = FromVar,
                                                             .srcVar = varFromChar(src[0])};
                }
            }
        }
    }
}

static bool run(const Instruction *i, int64_t vars[4], int input) {
    int64_t srcValue = i->src == FromImm ? i->srcImm : vars[i->srcVar];

    switch (i->op) {
    case Halt:
        return false;
    case Inp:
        vars[i->dest] = input;
        break;
    case Add:
        vars[i->dest] += srcValue;
        break;
    case Div:
        assert(srcValue != 0);
        vars[i->dest] /= srcValue;
        break;
    case Eql:
        vars[i->dest] = vars[i->dest] == srcValue;
        break;
    case Mod:
        assert(vars[i->dest] >= 0);
        assert(srcValue > 0);
        vars[i->dest] %= srcValue;
        break;
    case Mul:
        vars[i->dest] *= srcValue;
        break;
    }

    return true;
}

static inline int64_t runProgram(const Instruction program[CAP], int64_t initZ, int input) {
    int64_t vars[4] = {0, 0, 0, initZ};

    for (const Instruction *i = program; run(i, vars, input); ++i) {
    }

    return vars[Z];
}

static int64_t findLargestMonad(const Instruction programs[14][CAP], int64_t initZ, uint8_t digitIndex, int64_t currentMonad) {
    size_t index = (uint64_t)initZ & (uint64_t)(INDEX_CAP - 1);

    for (int i = 0; i < nNotFoundCache[digitIndex][index]; ++i) {
        if (notFoundCache[digitIndex][index][i] == initZ) {
            return -1;
        }
    }

    for (int d = 9; d >= 1; --d) {
        int64_t resultZ = runProgram(programs[digitIndex], initZ, d);

        if (digitIndex == 13) {
            if (resultZ == 0) {
                return d + currentMonad * 10;
            }
        } else {
            int64_t nextMonad = findLargestMonad(programs, resultZ, digitIndex + 1, d + currentMonad * 10);

            if (nextMonad > 0) {
                return nextMonad;
            }
        }
    }

    notFoundCache[digitIndex][index][nNotFoundCache[digitIndex][index]++] = initZ;
    assert(nNotFoundCache[digitIndex][index] < NOT_FOUND_CAP);

    return -1;
}

static int64_t findSmallestMonad(const Instruction programs[14][CAP], int64_t initZ, uint8_t digitIndex, int64_t currentMonad) {
    size_t index = (uint64_t)initZ & (uint64_t)(INDEX_CAP - 1);

    for (int i = 0; i < nNotFoundCache[digitIndex][index]; ++i) {
        if (notFoundCache[digitIndex][index][i] == initZ) {
            return -1;
        }
    }

    for (int d = 1; d <= 9; ++d) {
        int64_t resultZ = runProgram(programs[digitIndex], initZ, d);

        if (digitIndex == 13) {
            if (resultZ == 0) {
                return d + currentMonad * 10;
            }
        } else {
            int64_t nextMonad = findSmallestMonad(programs, resultZ, digitIndex + 1, d + currentMonad * 10);

            if (nextMonad > 0) {
                return nextMonad;
            }
        }
    }

    notFoundCache[digitIndex][index][nNotFoundCache[digitIndex][index]++] = initZ;
    assert(nNotFoundCache[digitIndex][index] < NOT_FOUND_CAP);

    return -1;
}

static int64_t partOne(const Instruction programs[14][CAP]) {
    return findLargestMonad(programs, 0, 0, 0);
}

static int64_t partTwo(const Instruction programs[14][CAP]) {
    return findSmallestMonad(programs, 0, 0, 0);
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    Instruction programs[14][CAP] = {0};
    parse(input, programs);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(programs),
                   79999999999999, 99196997985942);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(programs),
                   13111111111111, 84191521311611);

    return 0;
}
