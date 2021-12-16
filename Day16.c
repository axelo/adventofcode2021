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

static int partOne(int n, const uint8_t bits[CAP], int *versionSum) {
    printf("Next packet, available bits (n): %d\n", n);

    if (n < 6) {
        return n;
    }

    printf("\n");
    for (int i = 0; i < n; ++i) {
        printf("%d", bits[i]);
    }
    printf("\n");

    uint8_t version = (bits[0] << 2) | (bits[1] << 1) | bits[2]; // First three bits encode the packet version
    uint8_t typeId = (bits[3] << 2) | (bits[4] << 1) | bits[5];  // The next three bits encode the packet type ID

    *versionSum += version;

    printf("version: %d\n", version);
    printf("typeId: %d\n", typeId);

    int b = 6;

    if (typeId == 4) {
        // Literal value packets encode a single binary number.
        // To do this, the binary number is padded with leading zeroes until its length is a multiple of four bits,
        // and then it is broken into groups of four bits.
        // Each group is prefixed by a 1 bit except the last group, which is prefixed by a 0 bit.
        // bool moreGroups = bits[b++];

        int binaryNumber = 0;

        for (bool moreGroups = true; moreGroups; moreGroups = bits[b], b += 5) {
            int group = (bits[b + 1] << 3) | (bits[b + 2] << 2) | (bits[b + 3] << 1) | (bits[b + 4]);

            binaryNumber <<= 4;
            binaryNumber |= group;

            printf("group: %d\n", group);
        }

        printf("Binary number: %d (b: %d)\n", binaryNumber, b);

        return b;
    } else {
        uint8_t lengthType = bits[b++];

        printf("lengthType: %d\n", lengthType);

        if (lengthType == 0) {
            // length is a 15-bit number representing the number of bits in the sub-packets.
            int16_t length = 0;
            for (int i = 0; i < 15; ++i) {
                length <<= 1;
                length |= bits[b++];
            }

            do {
                int consumed = partOne(length, bits + b, versionSum);
                b += consumed;
                length -= consumed;
                printf("Consumed: %d\n", consumed);
            } while (length > 0);

            return b;
        } else {
            // length is a 15-bit number representing the number of bits in the sub-packets.
            int16_t nSubPackets = 0;
            for (int i = 0; i < 11; ++i) {
                nSubPackets <<= 1;
                nSubPackets |= bits[b++];
            }

            printf("nSubPackets: %d\n", nSubPackets);

            for (int i = 0; i < nSubPackets; ++i) {
                int consumed = partOne(n - b, bits + b, versionSum);
                b += consumed;
            }

            return b;
        }
    }
}

static int partTwo(int n, const uint8_t bits[CAP]) {
    return -1;
}

int main() {
    const char *input = Helpers_readInputFile(__FILE__);

    uint8_t bits[CAP] = {0};
    int n = parse(input, bits);

    int versionSum = 0;

    // Helpers_assert(PART1, Helpers_clock(),
    //                partOne(n, bits, &versionSum),
    //                31, 945);

    partOne(n, bits, &versionSum);

    printf("versionSum: %d\n", versionSum);

    // Helpers_assert(PART2, Helpers_clock(),
    //                partTwo(n, xs),
    //                -1, -2);

    return 0;
}
