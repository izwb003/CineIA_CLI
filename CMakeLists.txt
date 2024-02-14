cmake_minimum_required(VERSION 3.10)
project(CineIA_CLI VERSION 0.2.0)

# Init CMake
cmake_policy(SET CMP0048 NEW)
set(CMAKE_CXX_STANDARD 11)

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
elseif(UNIX)
    add_definitions(/DASDCP_PLATFORM=\"unix\")
endif()
set(ASDCP_HEADER_PATH
    external/asdcplib/src
)

# Include indicators library
set(INDICATORS_HEADER_PATH
    external/indicators/include
)

# Build executable
add_executable(cineia
    main.cpp
    cineia.h cineia.cpp
)
target_link_libraries(cineia
    IABLib
    libkumu libasdcp libas02
)
target_include_directories(cineia PRIVATE
    ${ASDCP_HEADER_PATH}
    ${IABLib_HEADER_PATH}
    ${INDICATORS_HEADER_PATH}
)

# Pass version number
target_compile_definitions(cineia PRIVATE
    PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Set install rules
install(TARGETS cineia RUNTIME DESTINATION bin)
