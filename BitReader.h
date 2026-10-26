#ifndef VAJA1_BITREADER_H
#define VAJA1_BITREADER_H
#include <iosfwd>

// https://www.geeksforgeeks.org/cpp/cpp-bitwise-operators/
class BitReader {
    std::ifstream& input;
    unsigned char buffer = 0;   // trenutni bajt
    int count = 0;  // pove, koliko bitov bufferja je se neprebranih

public:
    explicit BitReader(std::ifstream& input) : input(input) {}

    // Prebere en bit, 0 ali 1
    int read_bit() {
        // Ce smo porabili vse bite trenutneg bajta, preberemo naslednji bajt
        if (count == 0) {
            char c;
            if (!input.get(c)) return 0;
            buffer = static_cast<unsigned char>(c);
            count = 8;
        }
        // Vzamemo naslednji bit, od MSB do LSB
        count--;
        return (buffer >> count) & 1;
    }

    // Prebere num_bits bitov in jih zdruzi v eno stevilo
    unsigned long long read_bits(int num_bits) {
        unsigned long long value = 0;
        for (int i = 0; i < num_bits; i++) {
            value = (value << 1) | read_bit();
        }
        return value;
    }
};

#endif //VAJA1_BITREADER_H
