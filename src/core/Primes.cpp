#include "Primes.h"

// Naivni test prastevilnosti
// Za prastevilo p preverimo vsa potencialna delitelja manjsa od sqrt(p)
bool naive_test(unsigned long long p) {
    if (p == 2 || p == 3) return true;
    if (p < 2 || p % 2 == 0) return false;

    // Potencialni delitelj, zacnemo z 3
    unsigned long long j = 3;

    // Gremo od 3 do sqrt(p)
    while (j * j <= p) {
        if (p % j == 0) {
            return false;
        }

        // Preverimo samo liha stevila
        j += 2;
    }

    return true;
}

// Naivno generiranje prastevil
unsigned long long naive_generate(LCG &lcg, unsigned long long x, unsigned long long y) {

    unsigned long long p = lcg.random(x,y);

    // Prastevilo ne more biti sodo, pa povecjamo stevilo da postane liho
    if (p % 2 == 0) {
        p++;
    }

    while (true) {
        if (naive_test(p)) {
            return p;
        }

        // Naslednje liho stevilo
        p += 2;

        // Ce presezemo zgornjo mejo intervala, se vrnemo na zacetek meje, x + 1 ker je x vedno sod
        if (p > y) {
            p = x + 1;
        }
    }
}

// Miler rabinov test
// s je st. ponovitev testa, vecji parametar s vecja zanesljivost
bool miller_rabin_test(LCG &lcg, unsigned long long p, unsigned long long s) {
    if (p == 2 || p == 3) return true;
    if (p < 2 || p % 2 == 0) return false;

    unsigned long long d = p - 1;
    unsigned long long k = 0;

    while (d % 2 == 0) {
        d /= 2;
        k++;
    }

    // Test ponovimo s krat, vsakic z drugo a
    for (unsigned long long j = 1; j <= s; j++) {
        unsigned long long a = lcg.random(2, p - 2);
        unsigned long long x = modular_exponentiation(a, d, p); // Iscemo dokaz da je p sestavljeno

        if (x == 1) continue;

        // če ∃i: a^(d * 2^i)≡−1(mod p) potem je p verjetno praštevilo
        for (unsigned long long i = 0; i < k; i++) {
            if (x == p - 1) break;
            x = (x * x) % p;
        }

        if (x != p - 1) return false;   // Verjetno sestavljeno stevilo
    }

    return true;   // Verjetno prastevilo
}

// Generiranje prastevil z Miller-Rabinovim testom
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

// Generira prastevilo z bits biti in metodo
unsigned long long generate_prime(LCG& lcg, int bits, bool useMillerRabin, unsigned long long s) {
    unsigned long long min = min_from_bits(bits);
    unsigned long long max = max_from_bits(bits);

    if (useMillerRabin) {
        return miller_rabin_generate(lcg, min, max, s);
    }

    return naive_generate(lcg, min, max);
}
