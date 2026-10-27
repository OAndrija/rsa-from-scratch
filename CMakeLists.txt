cmake_minimum_required(VERSION 4.3)
project(vaja1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)
qt_standard_project_setup()   # turns on AUTOMOC / AUTOUIC for you

# Logic, no UI
add_library(vaja1_core STATIC
        src/core/MathUtils.cpp
        src/core/Primes.cpp
        src/core/RSA.cpp
        src/benchmarks/Benchmark.cpp
)
target_include_directories(vaja1_core PUBLIC src)


# Qt GU
qt_add_executable(vaja1
        src/gui/main.cpp
        src/gui/MainWindow.h
        src/gui/MainWindow.cpp
        src/gui/MainWindow.h
        src/gui/MainWindow.cpp
)
target_link_libraries(vaja1 PRIVATE vaja1_core Qt6::Widgets Qt6::Concurrent)