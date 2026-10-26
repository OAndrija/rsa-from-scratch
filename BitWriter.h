//
// Created by andrija on 10/24/26.
//

#ifndef VAJA1_BITWRITER_H
#define VAJA1_BITWRITER_H
#include <fstream>

class BitWriter {
    std::ofstream& output;
    unsigned char buffer = 0;
    int count = 0;

public:
    explicit BitWriter(std::ofstream& output) : output(output) {}

    void write_bit(int bit) {
        buffer = (buffer << 1) | bit;
        count++;

        if (count == 8) {
            output.put(static_cast<char>(buffer));
            buffer = 0;
            count = 0;
        }
    }

    void write_bits(unsigned long long value, int num_bits) {
        for (int i = 0; i < num_bits; i++) {
            write_bit((value >> i) & 1);
        }
    }

    void flush() {
        while (count != 0) write_bit(0); //padding empty bits with zeros
    }
};

#endif //VAJA1_BITWRITER_H
