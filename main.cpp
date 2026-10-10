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

bool naive_test(unsigned long long p) {
    if (p < 2 || p % 2 == 0) return false;
    if (p == 2 || p == 3) return true;

    unsigned long long j = 3;

    while (j * j <= p) {
        if (p % j == 0) {
            return false;
        }

        j += 2;
    }

    return true;
}

unsigned long long naive(LCG &lcg, unsigned long long x, unsigned long long y) {

    unsigned long long p = lcg.random(x,y);

    if (p % 2 == 0) {
        p++;
    }

    while (true) {
        if (naive_test(p)) {
            return p;
        }

        p += 2;

        if (p > y) {
            p = x + 1;
        }
    }
}

unsigned long long modular_exponentiation(unsigned long long a, unsigned long long b, unsigned long long n) {
    unsigned long long d = 1;
    int j = 0;
    unsigned long long temp = b;

    while (temp >>= 1) { //Set temp to itself shifted by one bit to the right 1011 -> 0101
        j++;
    }

    for (int i = j; i >= 0; i--) {
        d = ( d * d ) % n;
        if ((b >> i) & 1) { // ">>" shift b i bits to the right , "&" is 1 if lsb is 1
            d = (d * a) % n;
        }
    }

    return d;
}

bool miller_rabin_test(LCG &lcg, unsigned long long p, unsigned long long s) {
    if (p <= 3) return true;   //prime
    if (p % 2 == 0) return false;   //composite

    unsigned long long d = p - 1;
    unsigned long long k = 0;

    while (d % 2 == 0) {
        d /= 2;
        k++;
    }

    for (unsigned long long j = 1; j <= s; j++) {
        unsigned long long a = lcg.random(2, p - 2);
        unsigned long long x = modular_exponentiation(a, d, p);

        if (x == 1) continue;

        //če ∃i..
        for (unsigned long long i = 0; i < k; i++) {
            if (x == p - 1) break;
            x = (x * x) % p;
        }

        if (x != p - 1) return false;   //composite number
    }

    return true;   //probably prime number
}

unsigned long long miller_rabin_generate(LCG &lcg, unsigned long long min, unsigned long long max, unsigned long long s) {
    unsigned long long p = lcg.random(min, max);

    if (p % 2 == 0) {
        p++;
    }

    while (!miller_rabin_test(lcg, p, s)) {
        p += 2;

        if (p > max) {
            p = min + 1;
        }
    }

    return p;
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

unsigned long long min_from_bits(int bits) {
    return 1ULL << (bits - 1);
}

unsigned long long max_from_bits(int bits) {
    return (1ULL << bits) - 1;
}

void prime_generation_timing_test(LCG lcg) {
    const int minBits = 4;
    const int maxBits = 32;
    const int runs = 100;
    const unsigned long long s = 10;

    std::ofstream outfile("timing.csv");

    if (!outfile) {
        std::cerr << "Unable to open timing.csv" << std::endl;
        return;
    }

    outfile << "Bits,Naive_ns,MillerRabin_ns\n";

    volatile unsigned long long sink = 0;

    for (int bits = minBits; bits <= maxBits; bits++) {
        unsigned long long min = min_from_bits(bits);
        unsigned long long max = max_from_bits(bits);

        double naiveTotal = 0;
        double mrTotal = 0;

        for (int i = 0; i < runs; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            sink = naive(lcg, min, max);
            auto end = std::chrono::high_resolution_clock::now();
            naiveTotal += std::chrono::duration<double, std::nano>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            sink = miller_rabin_generate(lcg, min, max, s);
            end = std::chrono::high_resolution_clock::now();
            mrTotal += std::chrono::duration<double, std::nano>(end - start).count();
        }

        outfile << bits << "," << naiveTotal / runs << "," << mrTotal / runs << "\n";

        std::cout << "Bits: " << bits << " done" << std::endl;
    }

    outfile.close();

    std::cout << "Results saved to timing.csv file" << std::endl;

}

void miller_rabin_s_timing_test(LCG lcg) {
    const int bits = 32;
    const int minS = 1;
    const int maxS = 20;
    const int runs = 1000;

    unsigned long long min = min_from_bits(bits);
    unsigned long long max = max_from_bits(bits);

    std::ofstream outfile("timing_s.csv");

    if (!outfile) {
        std::cerr << "Unable to open timing_s.csv" << std::endl;
        return;
    }

    outfile << "s,MillerRabin_ns\n";

    volatile unsigned long long sink = 0;

    for (int s = minS; s <= maxS; s++) {
        double total = 0;

        for (int i = 0; i < runs; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            sink = miller_rabin_generate(lcg, min, max, s);
            auto end = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration<double, std::nano>(end - start).count();
        }

        outfile << s << "," << total / runs << "\n";

        std::cout << "s: " << s << " done" << std::endl;
    }

    outfile.close();

    std::cout << "Results saved to timing_s.csv file" << std::endl;
}

void print_menu() {
    std::cout << "\t\tGENERATOR PRASTEVIL" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "1. Generiranje prastevil - naivna metoda" << std::endl;
    std::cout << "2. Generiranje prastevil - Miller-Rabin" << std::endl;
    std::cout << "3. Test prastevilnosti - naivna metoda" << std::endl;
    std::cout << "4. Test prastevilnosti - Miller-Rabin" << std::endl;
    std::cout << "5. Test LCG generatorja" << std::endl;
    std::cout << "6. Test casovne zahtevnosti generiranja prastevil" << std::endl;
    std::cout << "7. Test casovne zahtevnosti glede na parameter s" << std::endl;
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

                std::cout << "Vnesite stevilo bitov (2-32): ";
                std::cin >> bits;

                if (bits < 2 || bits > 32) {
                    std::cout << "Stevilo bitov omejeno med 2 in 32.\n" << std::endl;
                    break;
                }

                unsigned long long min = min_from_bits(bits);
                unsigned long long max = max_from_bits(bits);

                std::cout << "\nObmocje stevil: " << min << " - " << max << std::endl;

                unsigned long long prime = naive(lcg, min, max);

                std::cout << "Najdeno prastevilo: " << prime << "\n" << std::endl;

                break;
            }

            case 2: {
                int bits;
                unsigned long long s;

                std::cout << "Vnesite stevilo bitov (2-32): ";
                std::cin >> bits;

                if (bits < 2 || bits > 32) {
                    std::cout << "Stevilo bitov omejeno med 2 in 32.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite parameter s: ";
                std::cin >> s;

                if (s < 1 ) {
                    std::cout << "Parameter s mora biti vsaj 1.\n" << std::endl;
                    break;
                }

                unsigned long long min = min_from_bits(bits);
                unsigned long long max = max_from_bits(bits);

                std::cout << "\nObmocje stevil: " << min << " - " << max << std::endl;

                unsigned long long prime = miller_rabin_generate(lcg, min, max, s);

                std::cout << "Najdeno (verjetno) prastevilo: " << prime << "\n" << std::endl;

                break;
            }


            case 3: {
                unsigned long long number;

                std::cout << "Vnesite stevilo (do 32 bitov): ";
                std::cin >> number;

                if (number > max_from_bits(32)) {
                    std::cout << "Stevilo je vecje od 32 bitov.\n" << std::endl;
                    break;
                }

                if (naive_test(number)) {
                    std::cout << number << " je PRASTEVILO.\n" << std::endl;
                } else {
                    std::cout << number << " je SESTAVLJENO STEVILO.\n" << std::endl;
                }

                break;
            }


            case 4: {
                unsigned long long number;
                unsigned long long s;

                std::cout << "Vnesite stevilo (do 32 bitov): ";
                std::cin >> number;

                if (number > max_from_bits(32)) {
                    std::cout << "Stevilo je vecje od 32 bitov.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite parameter s: ";
                std::cin >> s;

                if (s < 1) {
                    std::cout << "Parameter s mora biti vsaj 1.\n" << std::endl;
                    break;
                }

                if (number < 2) {   //0 and 1 are not prime
                    std::cout << number << " je SESTAVLJENO STEVILO.\n" << std::endl;
                    break;
                }

                if (miller_rabin_test(lcg, number, s)) {
                    std::cout << number << " je VERJETNO PRASTEVILO.\n" << std::endl;
                } else {
                    std::cout << number << " je SESTAVLJENO STEVILO.\n" << std::endl;
                }

                break;
            }


            case 5: {
                lcg_test(lcg);
                std::cout << std::endl;
                break;
            }

            case 6: {
                prime_generation_timing_test(lcg);
                break;
            }

            case 7: {
                miller_rabin_s_timing_test(lcg);
                std::cout << std::endl;
                break;
            }

            case 0:
                break;

            default:
                std::cout << "\nNapacna izbira. Opcija " << choice << " ne obstaja." << std::endl;
                std::cout << "Poskusite ponovo.\n";
        }
    } while (choice != 0);

    return 0;
}
