#ifndef VAJA1_BENCHMARK_H
#define VAJA1_BENCHMARK_H
#include "../core/LCG.h"

bool lcg_test(LCG &lcg);
bool prime_generation_timing_test(LCG lcg);
bool miller_rabin_s_timing_test(LCG lcg);
bool keygen_timing_test(LCG& lcg);
bool crypt_timing_test(LCG& lcg);


#endif //VAJA1_BENCHMARK_H
