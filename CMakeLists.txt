cmake_minimum_required(VERSION 3.10)
project(CineIA_CLI VERSION 0.1.0)

# Include iab-renderer library
add_subdirectory(external/iab-renderer)
set(IABLib_HEADER_PATH
    external/iab-renderer/include
    external/iab-renderer/src/lib
)

# Include asdcplib library
add_subdirectory(external/asdcplib)
if(WIN32)
    add_definitions(/DKM_WIN32)
endif()
set(ASDCP_HEADER_PATH
    external/asdcplib/src
)

# Build CineIA library
add_library(CineIA cineia.h cineia.cpp)
target_link_libraries(CineIA PUBLIC IABLib)
target_include_directories(CineIA PUBLIC ${IABLib_HEADER_PATH})

# Build executable
add_executable(cineia main.cpp)
target_link_libraries(cineia CineIA libkumu libasdcp libas02)
target_include_directories(cineia PRIVATE ${ASDCP_HEADER_PATH})

# Pass version number
target_compile_definitions(cineia PRIVATE
    PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)