cmake_minimum_required(VERSION 3.10)
project(CineIA_CLI VERSION 0.1.0)

# Include iab-renderer library
add_subdirectory(external/iab-renderer)
include_directories(external/iab-renderer/include)
include_directories(external/iab-renderer/src/lib)

# Build CineIA library
add_library(CineIA cineia.h cineia.cpp)

# Build test
add_executable(test test.h test.cpp)
target_link_libraries(test PUBLIC IABLib CineIA)