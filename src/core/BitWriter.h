#ifndef VAJA1_BITWRITER_H
#define VAJA1_BITWRITER_H
#include <fstream>

class BitWriter {
    std::ofstream& output;
    unsigned char buffer = 0;
    int count = 0;

public:
    explicit BitWriter(std::ofstream& output) : output(output) {}

    // Zapise en bit na konec bufferja
    void write_bit(int bit) {
        buffer = (buffer << 1) | bit;
        count++;

        // Ko je buffer poln, se zapise v datoteko
        if (count == 8) {
            output.put(static_cast<char>(buffer));
            buffer = 0;
            count = 0;
        }
    }

    // Zapise num_bits bitov stevila value, od MSB do LSB
    void write_bits(unsigned long long value, int num_bits) {
        for (int i = num_bits - 1; i >= 0; i--) { // naj bere MSB
            write_bit((value >> i) & 1);
        }
    }

    // Zapise se nedokoncan bajt, manjkajoce bite popolni z 0
    // Klicemo na koncu pisanja
    void flush() {
        while (count != 0) write_bit(0);
    }
};

#endif //VAJA1_BITWRITER_H
