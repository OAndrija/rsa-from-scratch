#include <iostream>
#include <fstream>
#include <chrono>

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

unsigned long long naive(LCG &lcg, unsigned long long x, unsigned long long y) {

    unsigned long long p = lcg.random(x,y);

    if (p % 2 == 0) {
        p++;
    }

    while (true) {
        unsigned long long j = 3;

        while (j * j <= p) {
            if (p % j == 0) {
                break;
            }

            j += 2;
        }

        if (j * j > p) {
            return p;
        }

        p += 2;
    }
}

void lcg_test(LCG &lcg) {
    const int min = 1;
    const int max = 1000;
    const int numberOfValues = 1000000;

    int frequency[max + 1] = {};

    for (int i = 0; i < numberOfValues; i++) {
        unsigned long long number = lcg.random(min, max);
        frequency[number]++;
    }

    std::ofstream outfile("histogram.csv");

    if (!outfile) {
        std::cerr << "Unable to open histogram.csv" << std::endl;
        return;
    }

    outfile << "Number,Frequency\n";

    for (int i = min; i <= max; i++) {
        outfile << i << "," << frequency[i] << "\n";
    }

    outfile.close();

    std::cout << "Generated " << numberOfValues << " numbers" << std::endl;
    std::cout << "Results saved to histogram.csv file" << std::endl;
}

int main() {
    LCG lcg;

    // for (int n = 3; n <= 10; n++) {
    //     unsigned long long x = 1;
    //     for (int i = 1; i < n; i++) x *= 10;   // 10^(n-1)
    //     unsigned long long y = x * 10 - 1;     // 10^n - 1
    //
    //     auto start = std::chrono::high_resolution_clock::now();
    //     unsigned long long p = naive(lcg, x, y);
    //     auto end = std::chrono::high_resolution_clock::now();
    //
    //     std::chrono::duration<double, std::milli> ms = end - start;
    //
    //     std::cout << "n = " << n << ", praštevilo: " << p
    //               << ", čas: " << ms.count() << " ms" << std::endl;
    // }

    return 0;
}
