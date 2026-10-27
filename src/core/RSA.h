#ifndef VAJA1_RSA_H
#define VAJA1_RSA_H

#include "LCG.h"

#include <fstream>

// Dva RSA kljuca, javni (e,n) in privatni (d,n)
struct RSAKeys {
    long long e, d, n;
};

RSAKeys generate_keys(LCG& lcg, int bits, bool useMillerRabin, unsigned long long s);

bool save_keys(const RSAKeys& keys);
bool load_key(const std::string& filename, long long& e, long long& n);

bool encrypt_file(const std::string& inputName, const std::string& outputName, long long e, long long n);
bool decrypt_file(const std::string& inputName, const std::string& outputName, long long d, long long n);

#endif //VAJA1_RSA_H
