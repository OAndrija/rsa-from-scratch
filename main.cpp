#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>

#include "BitReader.h"
#include "BitWriter.h"
#include "LCG.h"

// Dva RSA kljuca, javni (e,n) in privatni (d,n)
struct RSAKeys {
    long long e, d, n;
};

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
unsigned long long naive(LCG &lcg, unsigned long long x, unsigned long long y) {

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

// a^b mod n
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

// Generira prastevilo z bits biti in metodo
unsigned long long generate_prime(LCG& lcg, int bits, bool useMillerRabin, unsigned long long s) {
    unsigned long long min = min_from_bits(bits);
    unsigned long long max = max_from_bits(bits);

    if (useMillerRabin) {
        return miller_rabin_generate(lcg, min, max, s);
    }

    return naive(lcg, min, max);
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

// Generiranje RSA kljucev
RSAKeys generate_keys(LCG& lcg, int bits, bool useMillerRabin, unsigned long long s) {
    // 1. Izberemo dva razlicna prastevila p in q z enakim stevilom bitov, torej priblizno enaki veliksoti
    unsigned long long p = generate_prime(lcg, bits, useMillerRabin, s);
    unsigned long long q;
    do {
        q = generate_prime(lcg, bits, useMillerRabin, s);
    } while (q == p);

    // 2. Izracunamo n = p * q in Eulerjevo/totientno funkcijo phi
    long long n = static_cast<long long>(p * q);
    long long fi = static_cast<long long>((p - 1) * (q - 1));

    // 3. Nakljucno manjse liho stevilo e, tako da velja 1 < e < fi in gcd (e, fi) = 1
    long long e, gcd, x, y;
    do {
        e = static_cast<long long>(lcg.random(3, fi - 1));
        extended_euclid(e, fi, gcd, x, y);
    } while (e % 2 == 0 || gcd != 1);

    // Step 4: Get multiplicative inverse of e mod phi => e * d = 1 (mod phi)
    // 4. Izracunamo skriti eksopnent d kot multiplikativni inverz e po modulu fi
    std::vector<long long> solutions = modular_linear_equation_solver(e, 1, fi);
    long long d = solutions[0]; // gcd je 1, torej ena resitev

    // 5. Javni kljuc P = (e, n) in privatni kljuc S = (d, n)
    RSAKeys keys;
    keys.e = e;
    keys.d = d;
    keys.n = n;

    return keys;
}

bool save_keys(const RSAKeys& keys) {
    std::ofstream public_key("pubkey.txt");
    std::ofstream private_key("privkey.txt");

    if (!public_key || !private_key) {
        return false;
    }

    public_key << keys.e << " " << keys.n << "\n";
    private_key << keys.d << " " << keys.n << "\n";

    return true;
}

bool load_key(const std::string& filename, long long& e, long long& n) {
    std::ifstream input(filename);

    if (!input) {
        return false;
    }

    input >> e >> n;

    return !input.fail();
}

// Sifriranje datotek z javnim kljucem (e, n)
// Datoteke beremo po log2_down(n) bitov
// Sifriran blok C je lahko najvec n-1, zato ga zapisemo z log2_up(n) biti, en bit vec kot M
bool encrypt_file(const std::string& inputName, const std::string& outputName, long long e, long long n) {
    std::ifstream input(inputName, std::ios::binary);
    std::ofstream output(outputName, std::ios::binary);

    if (!input || !output) {
        return false;
    }

    // Dolocimo velikost vhodne datoteke v bajtih
    input.seekg(0, std::ios::end);
    unsigned long long fileSize = input.tellg();
    input.seekg(0, std::ios::beg);

    // Dolocimo stevilo bitov za vhodne in sifrirane bloke
    int inBits = log2_down(n);
    int outBits = log2_up(n);

    BitReader reader(input);
    BitWriter writer(output);

    // V glavo datoteke zapisemo velikost izvorne datoteke, za pravilno desifriranje
    writer.write_bits(fileSize, 64);

    // Izracunamo stevilo blokov, ki jih moramo zasifrirati
    unsigned long long totalBits = fileSize * 8;
    unsigned long long blocks = totalBits / inBits;

    if (totalBits % inBits != 0) {
        blocks++;
    }

    for (unsigned long long i = 0; i < blocks; i++) {
        // Preberemo blok bitov in ga sifriramo
        unsigned long long M = reader.read_bits(inBits);
        unsigned long long C = modular_exponentiation(M, e, n);
        writer.write_bits(C, outBits);
    }

    writer.flush();

    return true;
}

// Desifriranje datotek z privatnim kljucem (d, n)
// Beremo bloke po log2_up(n) bitov,
// vsakega desifriramo in zapisemo nazaj z log2_down(n) biti
bool decrypt_file(const std::string& inputName, const std::string& outputName, long long d, long long n) {
    std::ifstream input(inputName, std::ios::binary);
    std::ofstream output(outputName, std::ios::binary);

    if (!input || !output) {
        return false;
    }

    // Dolocimo stevilo bitov v sifriranem in izvirnem bloku
    int inBits = log2_down(n);
    int outBits = log2_up(n);

    BitReader reader(input);
    BitWriter writer(output);

    // Iz glave izvorne datoteke preberemo velikost v bajtih
    unsigned long long fileSize = reader.read_bits(64);
    unsigned long long bitsLeft = fileSize * 8;

    while (bitsLeft > 0) {
        // Preberemo sifriran blok in ga desifriramo
        unsigned long long C = reader.read_bits(outBits);
        unsigned long long M = modular_exponentiation(C, d, n);

        int bitsToWrite = inBits;

        // Za zadnji blok preberemo prvih bitsLeft bitov, preostali so padded nicle ki smo ih dodali pri sifriranju
        if (bitsLeft < static_cast<unsigned long long>(inBits)) {
            bitsToWrite = static_cast<int>(bitsLeft);
        }

        writer.write_bits(M >> (inBits - bitsToWrite), bitsToWrite); // Odreze nicle na koncu bloka
        bitsLeft -= bitsToWrite;
    }

    writer.flush();

    return true;
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

// Cas generiranja RSA kljucev glede na stevilo bitov p in q
void keygen_timing_test(LCG& lcg) {
    const int minBits = 3;
    const int maxBits = 15;
    const int runs = 2000;
    const unsigned long long s = 10;

    std::ofstream outfile("timing_keygen.csv");
    if (!outfile) {
        std::cerr << "Unable to open timing_keygen.csv" << std::endl;
        return;
    }

    outfile << "Bits_pq,Bits_n,Naive_ns,MillerRabin_ns\n";

    volatile long long sink = 0;

    for (int bits = minBits; bits <= maxBits; bits++) {
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
        std::cout << "Bits p,q: " << bits << " done" << std::endl;
    }

    outfile.close();
    std::cout << "Results saved to timing_keygen.csv file" << std::endl;
}

// Cas sifriranja in desifriranja stevila glede na stevilo bitov n
// Pri vsakem n izvedemo veliko ponovitev, vsakic z drugim M
void crypt_timing_test(LCG& lcg) {
    const int minBits = 3;
    const int maxBits = 15;
    const int runs = 20000;

    std::ofstream outfile("timing_crypt.csv");
    if (!outfile) {
        std::cerr << "Unable to open timing_crypt.csv" << std::endl;
        return;
    }

    outfile << "Bits_pq,Bits_n,Encrypt_ns,Decrypt_ns\n";

    volatile unsigned long long sink = 0;

    for (int bits = minBits; bits <= maxBits; bits++) {
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
        std::cout << "Bits p,q: " << bits << " done, errors: " << errors << std::endl;
    }

    outfile.close();
    std::cout << "Results saved to timing_crypt.csv file" << std::endl;
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
    std::cout << "8. Generiranje RSA kljucev" << std::endl;
    std::cout << "9. Sifriranje stevila" << std::endl;
    std::cout << "10. Desifriranje stevila" << std::endl;
    std::cout << "11. Sifriranje datotek" << std::endl;
    std::cout << "12. Desifriranje datotek" << std::endl;
    std::cout << "13. Test casovne zahtevnosti generiranja kljucev" << std::endl;
    std::cout << "14. Test casovne zahtevnosti sifriranja in desifriranja" << std::endl;
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

            case 8: {
                int bits, method;

                std::cout << "Vnesite stevilo bitov(3-15): ";
                std::cin >> bits;

                if (bits < 3 || bits > 15) {
                    std::cout << "Obmocje bitov od 3 do 15.\n" << std::endl;
                    break;
                }

                std::cout << "Metoda (1 = naiva, 2 = Miller-Rabin: ";
                std::cin >> method;

                if (method != 1 && method != 2) {
                    std::cout << "Napacna izbira metode.\n" << std::endl;
                    break;
                }

                bool useMillerRabin = (method == 2);

                RSAKeys keys = generate_keys(lcg, bits, useMillerRabin, 10);

                std::cout << "Javni kljuc: " << keys.e << ", " << keys.n << std::endl;
                std::cout << "Privatni kljuc: " << keys.d << ", " << keys.n << std::endl;

                if (save_keys(keys)) {
                    std::cout << "Kljuci vspesno shranjeni v pubkey.txt in privkey.txt.\n" << std::endl;
                } else {
                    std::cout << "Napaka pri shranjevanju datotek.\n" << std::endl;
                }

                break;
            }

            case 9: {
                long long e, n , M;

                if (!load_key("pubkey.txt", e, n)) {
                    std::cout << "Najprej generirajte kljuce.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite sporocilo M: ";
                std::cin >> M;

                if (M < 0 || M >= n) {
                    std::cout << "M mora biti manjse od n.\n" << std::endl;
                    break;
                }

                long long C = modular_exponentiation(M, e, n);

                std::cout << "C = " << C << std::endl << std::endl;

                break;
            }

            case 10: {
                long long d, n, C;

                if (!load_key("privkey.txt", d, n)) {
                    std::cout << "Najprej generirajte kljuc.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite sifrirano sporocilo C: ";
                std::cin >> C;

                if (C < 0 || C >= n) {
                    std::cout << "C mora biti manjse od n.\n" << std::endl;
                    break;
                }

                long long M = modular_exponentiation(C, d, n);

                std::cout << "M = " << M << std::endl << std::endl;

                break;
            }

            case 11: {
                long long e, n;
                std::string inputName;

                if (!load_key("pubkey.txt", e, n)) {
                    std::cout << "Najprej generirajte kljuc.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite ime datoteke za sifriranje: ";
                std::cin >> inputName;

                if (encrypt_file(inputName, "enc.bin", e, n)) {
                    std::cout << "Datoteka sifrirana v enc.bin.\n" << std::endl;
                } else {
                    std::cout << "Napaka pri odpiranju datotek.\n" << std::endl;
                }

                break;
            }

            case 12: {
                long long d, n;

                if (!load_key("privkey.txt", d, n)) {
                    std::cout << "Najprej generirajte kljuc.\n" << std::endl;
                    break;
                }

                if (decrypt_file("enc.bin", "msg.bin", d, n)) {
                    std::cout << "Datoteka enc.bin desifrirana v msg.bin.\n" << std::endl;
                } else {
                    std::cout << "Napaka pri odpiranju datoteka.\n" << std::endl;
                }

                break;
            }

            case 13: {
                keygen_timing_test(lcg);
                std::cout << std::endl;
                break;
            }

            case 14: {
                crypt_timing_test(lcg);
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
