#ifndef VAJA1_BITREADER_H
#define VAJA1_BITREADER_H
#include <iosfwd>

class BitReader {
    std::ifstream& input;
    unsigned char buffer = 0;
    int count = 0;

public:
    explicit BitReader(std::ifstream& input) : input(input) {}

    int read_bit() {
        if (count == 0) {
            char c;
            if (!input.get(c)) return 0; //padding with zeros
            buffer = static_cast<unsigned char>(c);
            count = 8;
        }
        count--;
        return (buffer >> count) & 1;
    }

    unsigned long long read_bits(int num_bits) {
        unsigned long long value = 0;
        for (int i = 0; i < num_bits; i++) {
            value = (value << 1) | read_bit();
        }
        return value;
    }
};

#endif //VAJA1_BITREADER_H
