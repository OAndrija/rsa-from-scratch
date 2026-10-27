cmake_minimum_required(VERSION 4.3)
project(vaja1)

set(CMAKE_CXX_STANDARD 20)

add_executable(vaja1 src/main.cpp
        src/core/BitReader.h
        src/core/BitWriter.h
        src/core/LCG.h
        src/core/MathUtils.h
        src/core/MathUtils.cpp
        src/core/Primes.cpp
        src/core/Primes.h
        src/core/RSA.cpp
        src/core/RSA.h
        src/benchmarks/Benchmark.cpp
        src/benchmarks/Benchmark.h)
