cmake_minimum_required(VERSION 3.10)
project(CineIA_CLI VERSION 0.1.0)

# Include iab-renderer library
add_subdirectory(external/iab-renderer)
include_directories(external/iab-renderer/include)
include_directories(external/iab-renderer/src/lib)

# Include asdcplib library
add_subdirectory(external/asdcplib)
if(WIN32)
    add_definitions(/DKM_WIN32)
endif()
include_directories(external/asdcplib/src)

# Build CineIA library
add_library(CineIA cineia.h cineia.cpp)
target_link_libraries(CineIA PUBLIC IABLib)

# Build executable
add_executable(cineia main.cpp)
target_link_libraries(cineia PUBLIC CineIA libkumu libasdcp libas02)

# Pass version number
target_compile_definitions(cineia PRIVATE
    PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)