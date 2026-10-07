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

unsigned long long miller_rabin_test(LCG &lcg) { return 0; }

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

unsigned long long min_from_bits(int bits) {
    return 1ULL << (bits - 1);
}

unsigned long long max_from_bits(int bits) {
    return (1ULL << bits) - 1;
}

void print_menu() {
    std::cout << "GENERATOR PRASTEVIL" << std::endl;
    std::cout << "1. Generiranje prastevil - naivna metoda" << std::endl;
    std::cout << "2. Generiranje prastevil - Miller-Rabin" << std::endl;
    std::cout << "3. Test prastevilnosti - naivna metoda" << std::endl;
    std::cout << "4. Test prastevilnosti - Miller-Rabin" << std::endl;
    std::cout << "5. Test LCG generatorja" << std::endl;
    std::cout << "0. Izhod" << std::endl;
    std::cout << "\nIzbira: ";
}

int main() {
    LCG lcg;
    int choice;

    do {
        print_menu();
        std::cin >> choice;

        switch (choice) {
            case 1: {
                int bits;

                std::cout << "Izbrali ste generiranje prastevil z naivno metodo.\n\n";
                std::cout << "Vnesite stevilo bitov (2-32): ";
                std::cin >> bits;

                if (bits < 2 || bits > 32) {
                    std::cout << "Stevilo bitov omejeno med 2 in 32.\n" << std::endl;
                    break;
                }

                unsigned long long min = min_from_bits(bits);
                unsigned long long max = max_from_bits(bits);

                std::cout << "Obmocje stevil: " << min << " - " << max << std::endl;

                unsigned long long prime = naive(lcg, min, max);

                std::cout << "Najdeno prastevilo: " << prime << std::endl;

                break;
            }

            case 2:
                std::cout << "Izbrali ste generiranje prastevil z Miller-Rabinovo metodo." << std::endl;
                break;


            case 3:
                std::cout << "Izbrali ste generiranje prastevil z Miller-Rabinovo metodo." << std::endl;
                break;


            case 4:
                std::cout << "Izbrali ste generiranje prastevil z Miller-Rabinovo metodo." << std::endl;
                break;


            case 5:
                std::cout << "Izbrali ste generiranje prastevil z Miller-Rabinovo metodo." << std::endl;
                break;


            case 0:
                break;

            default:
                std::cout << "\nNapacna izbira. Opcija " << choice << " ne obstaja." << std::endl;
                std::cout << "Poskusite ponovo." << std::endl;
        }
    } while (choice != 0);

    return 0;


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
