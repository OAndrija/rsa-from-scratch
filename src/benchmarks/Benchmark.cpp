#include "Benchmark.h"
#include "../core/MathUtils.h"
#include "../core/Primes.h"
#include "../core/RSA.h"

#include <fstream>
#include <chrono>

bool lcg_test(LCG &lcg) {
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
        return false;
    }

    outfile << "Number,Frequency\n";

    for (int i = min; i <= max; i++) {
        outfile << i << "," << frequency[i] << "\n";
    }

    return true;
}

bool prime_generation_timing_test(LCG lcg) {
    const int minBits = 4;
    const int maxBits = 32;
    const int runs = 100;
    const unsigned long long s = 10;

    std::ofstream outfile("timing.csv");

    if (!outfile) {
        return false;
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
            sink = naive_generate(lcg, min, max);
            auto end = std::chrono::high_resolution_clock::now();
            naiveTotal += std::chrono::duration<double, std::nano>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            sink = miller_rabin_generate(lcg, min, max, s);
            end = std::chrono::high_resolution_clock::now();
            mrTotal += std::chrono::duration<double, std::nano>(end - start).count();
        }

        outfile << bits << "," << naiveTotal / runs << "," << mrTotal / runs << "\n";
    }

    return true;
}

bool miller_rabin_s_timing_test(LCG lcg) {
    const int bits = 32;
    const int minS = 1;
    const int maxS = 20;
    const int runs = 1000;

    unsigned long long min = min_from_bits(bits);
    unsigned long long max = max_from_bits(bits);

    std::ofstream outfile("timing_s.csv");

    if (!outfile) {
        return false;
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
    }

    return true;
}

// Cas generiranja RSA kljucev glede na stevilo bitov p in q
bool keygen_timing_test(LCG& lcg) {
    const int minBits = 3;
    const int maxBits = 15;
    const int runs = 2000;
    const unsigned long long s = 10;

    std::ofstream outfile("timing_keygen.csv");
    if (!outfile) {
        return false;
    }

    outfile << "Bits_pq,Bits_n,Naive_ns,MillerRabin_ns\n";

    volatile long long sink = 0;

    for (int bits = minBits; bits <= maxBits; bits++) {
        if (bits == 4) continue;
        // Naivna metoda
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < runs; i++) {
            RSAKeys keys = generate_keys(lcg, bits, false, s);
            sink = keys.n;
        }
        auto end = std::chrono::high_resolution_clock::now();
        double naiveAvg = std::chrono::duration<double, std::nano>(end - start).count() / runs;

        // Miller-Rabin
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < runs; i++) {
            RSAKeys keys = generate_keys(lcg, bits, true, s);
            sink = keys.n;
        }
        end = std::chrono::high_resolution_clock::now();
        double mrAvg = std::chrono::duration<double, std::nano>(end - start).count() / runs;

        outfile << bits << "," << 2 * bits << "," << naiveAvg << "," << mrAvg << "\n";
    }

    return true;
}

// Cas sifriranja in desifriranja stevila glede na stevilo bitov n
// Pri vsakem n izvedemo veliko ponovitev, vsakic z drugim M
bool crypt_timing_test(LCG& lcg) {
    const int minBits = 3;
    const int maxBits = 15;
    const int runs = 20000;

    std::ofstream outfile("timing_crypt.csv");
    if (!outfile) {
        return false;
    }

    outfile << "Bits_pq,Bits_n,Encrypt_ns,Decrypt_ns\n";

    volatile unsigned long long sink = 0;

    for (int bits = minBits; bits <= maxBits; bits++) {
        if (bits == 4) continue;
        RSAKeys keys = generate_keys(lcg, bits, true, 10);

        // Vnaprej pripravimo nakljucna sporocila M iz [1, n-1]
        std::vector<unsigned long long> messages(runs), ciphers(runs);
        for (int i = 0; i < runs; i++) {
            messages[i] = lcg.random(1, keys.n - 1);
        }

        // Sifriranje
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < runs; i++) {
            ciphers[i] = modular_exponentiation(messages[i], keys.e, keys.n);
            sink = ciphers[i];
        }
        auto end = std::chrono::high_resolution_clock::now();
        double encAvg = std::chrono::duration<double, std::nano>(end - start).count() / runs;

        // Desifriranje
        int errors = 0;
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < runs; i++) {
            sink = modular_exponentiation(ciphers[i], keys.d, keys.n);
        }
        end = std::chrono::high_resolution_clock::now();
        double decAvg = std::chrono::duration<double, std::nano>(end - start).count() / runs;

        // Preverimo pravilnost (izven merjenja casa)
        for (int i = 0; i < runs; i++) {
            if (modular_exponentiation(ciphers[i], keys.d, keys.n) != messages[i]) errors++;
        }

        outfile << bits << "," << 2 * bits << "," << encAvg << "," << decAvg << "\n";
    }

    return true;
}