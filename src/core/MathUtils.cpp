#include "MathUtils.h"

// Najmanjse n bitno stevilo
// Za 5 bitov -> 10000
unsigned long long min_from_bits(int bits) {
    return 1ULL << (bits - 1);
}

// Najvecje n bitno stevilo
// Za 5 bitov -> 100000 - 1 = 11111
unsigned long long max_from_bits(int bits) {
    return (1ULL << bits) - 1;
}

long long mod(long long a, long long n) {
    long long reminder = a % n;

    if (reminder < 0) {
        reminder += n;
    }

    return reminder;
}

// https://algomaster.io/learn/dsa/logarithms
// Steje, kolikokrat n delimo z 2, pre da pride do 1
int log2_down(unsigned long long n) {
    int result = 0;

    while (n > 1) {
        n /= 2;
        result++;
    }

    return result;
}

// https://stackoverflow.com/questions/3272424/compute-fast-log-base-2-ceiling/51351885#51351885
int log2_up(unsigned long long n) {
    if (n <= 1) {
        return 0;
    }

    // (n-1) zagotljava da se potence stevila dva obravnavajo pravilo
    return log2_down(n - 1) + 1;
}

// Razsirjeni Evklidov algoritam
// Posice stevilo d, ki je gcd(a, b) ter x in y, za katere velja d = a*b + b*y
void extended_euclid(long long a, long long b, long long& d, long long& x, long long& y) {
    if (b == 0) {
        d = a;
        x = 1;
        y = 0;
    } else {
        long long n_d, n_x, n_y;

        extended_euclid(b, mod(a, b), n_d, n_x, n_y);

        d = n_d;
        x = n_y;
        y = n_x - (a / b) * n_y;
    }
}

// Izracun modulske linearne enacbe a*x kongrurentno b mod n
// Pri racunanju multiplikativnega inverza, ko je b = 1 in gcd(a, n) = 1, obstaja natanko ena resitev modula n
std::vector<long long> modular_linear_equation_solver(long long a, long long b, long long n) {
    long long d, x, y;

    extended_euclid(a, n, d, x, y);

    if (b % d != 0) {
        return {};
    }

    long long x0 = mod(x * (b / d), n);
    std::vector<long long> solutions;

    for (int i = 0; i < d; i++) {
        long long solution = mod(x0 + i * (n / d), n);
        solutions.push_back(solution);
    }

    return solutions;
}

unsigned long long modular_exponentiation(unsigned long long a, unsigned long long b, unsigned long long n) {
    unsigned long long d = 1;
    int j = 0;
    unsigned long long temp = b;

    // Iscemo indeks MSB stevila b
    while (temp > 1) {
        temp = temp >> 1;
        j++;
    }

    for (int i = j; i >= 0; i--) {
        d = ( d * d ) % n;
        if ((b >> i) & 1) { // Ce je i-ti bit stevila b = 1, d = d * a % n
            d = (d * a) % n;
        }
    }

    return d;
}