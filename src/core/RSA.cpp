#include "RSA.h"

#include "BitReader.h"
#include "BitWriter.h"
#include "Primes.h"

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
