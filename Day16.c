#include "Helpers.c"

#define CAP 10000

static int parse(const char *input, uint8_t bits[CAP]) {
    int nInput = strlen(input);

    int n = 0;

    for (int i = 0; i < nInput; ++i) {
        if (input[i] == '\n') {
            break;
        }

        uint8_t nibble = input[i] > '9' ? input[i] - 'A' + 10 : input[i] - '0';

        for (int j = 3; j >= 0; --j) {
            bits[n++] = (nibble >> j) & 1;
            assert(n < CAP);
        }
    }

    return n;
}

static int64_t performOperation(uint8_t typeId, int64_t a, int64_t b) {
    switch (typeId) {
    case 0: return a + b;
    case 1: return a * b;
    case 2: return a < b ? a : b;
    case 3: return a > b ? a : b;
    case 5: return a > b;
    case 6: return a < b;
    case 7: return a == b;
    default:
        assert(false && "Unknown typeId");
    }
}

// Returns consumed bits.
static int decode(int n, const uint8_t bits[n], int *versionSum, int64_t *value) {
    if (n < 6) {
        assert(false && "Not enough bits for a valid packet");
    }

    *versionSum += (bits[0] << 2) |
                   (bits[1] << 1) |
                   bits[2];

    uint8_t typeId = (bits[3] << 2) |
                     (bits[4] << 1) |
                     bits[5];

    int bitIndex = 6;

    if (typeId == 4) {
        *value = 0;

        for (bool more = true; more; more = bits[bitIndex], bitIndex += 5) {
            *value <<= 4;
            *value |= (bits[bitIndex + 1] << 3) |
                      (bits[bitIndex + 2] << 2) |
                      (bits[bitIndex + 3] << 1) |
                      (bits[bitIndex + 4]);
        }

        return bitIndex;
    } else {
        uint8_t lengthType = bits[bitIndex++];

        if (lengthType == 0) {
            int16_t length = 0;

            for (int i = 0; i < 15; ++i) {
                length <<= 1;
                length |= bits[bitIndex++];
            }

            int consumed = decode(length, bits + bitIndex, versionSum, value);
            bitIndex += consumed;
            length -= consumed;

            int64_t subValue = 0;

            while (length > 0) {
                consumed = decode(length, bits + bitIndex, versionSum, &subValue);
                bitIndex += consumed;
                length -= consumed;

                *value = performOperation(typeId, *value, subValue);
            };

            return bitIndex;
        } else {
            int16_t nSubPackets = 0;

            for (int i = 0; i < 11; ++i) {
                nSubPackets <<= 1;
                nSubPackets |= bits[bitIndex++];
            }

            assert(nSubPackets > 0);

            bitIndex += decode(n - bitIndex, bits + bitIndex, versionSum, value);

            int64_t subValue = 0;

            for (int i = 1; i < nSubPackets; ++i) {
                bitIndex += decode(n - bitIndex, bits + bitIndex, versionSum, &subValue);

                *value = performOperation(typeId, *value, subValue);
            }

            return bitIndex;
        }
    }
}

static int64_t partOne(int n, const uint8_t bits[CAP]) {
    int versionSum = 0;
    int64_t value = 0;

    decode(n, bits, &versionSum, &value);

    return versionSum;
}

static int64_t partTwo(int n, const uint8_t bits[CAP]) {
    int versionSum = 0;
    int64_t value = 0;

    decode(n, bits, &versionSum, &value);

    return value;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    uint8_t bits[CAP] = {0};
    int n = parse(input, bits);

    Helpers_assert(PART1, Helpers_clock(),
                   partOne(n, bits),
                   20, 945);

    Helpers_assert(PART2, Helpers_clock(),
                   partTwo(n, bits),
                   1, 10637009915279);

    return 0;
}
