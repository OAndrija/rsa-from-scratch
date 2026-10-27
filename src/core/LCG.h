#ifndef VAJA1_LCG_H
#define VAJA1_LCG_H

class LCG {
private:
    unsigned long long R;

    static constexpr unsigned long long m = (1ULL << 32);
    static constexpr unsigned long long a = 69069;
    static constexpr unsigned long long b = 1;

public:
    LCG(unsigned long long R0 = 0) { R = R0; }

    unsigned long long next() {
        R = (a * R + b) % m;
        return R;
    }

    unsigned long long random(unsigned long long x, unsigned long long y) {
        return x + next() % (y - x + 1);
    }
};

#endif //VAJA1_LCG_H
