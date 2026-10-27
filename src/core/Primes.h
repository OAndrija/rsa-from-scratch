#ifndef VAJA1_PRIMES_H
#define VAJA1_PRIMES_H
#include "LCG.h"
#include "MathUtils.h"

unsigned long long naive_generate(LCG &lcg, unsigned long long x, unsigned long long y);
bool naive_test(unsigned long long p);

bool miller_rabin_test(LCG &lcg, unsigned long long p, unsigned long long s);
unsigned long long miller_rabin_generate(LCG &lcg, unsigned long long min, unsigned long long max, unsigned long long s);

unsigned long long generate_prime(LCG& lcg, int bits, bool useMillerRabin, unsigned long long s);

#endif //VAJA1_PRIMES_H
