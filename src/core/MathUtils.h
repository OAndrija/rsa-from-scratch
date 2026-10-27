//
// Created by andrija on 10/27/26.
//

#ifndef VAJA1_MATHUTILS_H
#define VAJA1_MATHUTILS_H

#include <vector>

unsigned long long min_from_bits(int bits);
unsigned long long max_from_bits(int bits);

long long mod(long long a, long long n);
int log2_down(unsigned long long n);
int log2_up(unsigned long long n);

void extended_euclid(long long a, long long b, long long& d, long long& x, long long& y);
std::vector<long long> modular_linear_equation_solver(long long a, long long b, long long n);
unsigned long long modular_exponentiation(unsigned long long a, unsigned long long b, unsigned long long n);

#endif //VAJA1_MATHUTILS_H
