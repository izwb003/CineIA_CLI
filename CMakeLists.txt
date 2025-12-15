cmake_minimum_required(VERSION 3.10)
project(CineIA_CLI VERSION 0.2.1 LANGUAGES CXX C)


# Глобально просим собирать статические библиотеки
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
# Static CRT for MSVC
if(MSVC)
    # Set default VCPKG triplet for static linking
    set(VCPKG_DEFAULT_TRIPLET x64-windows-static)

    # Set runtime library to static for MSVC
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:>")

    # Additional static CRT setup (you can remove this if you stick to the above)
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()

    # Additional linker flags for static linking
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

    # Set optimization for Release build
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2")

    # Strip debugging information
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG:NONE")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")

    # Disable CRT warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Set optimization to -O3 for Release build (non-MSVC)
if(NOT MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -fvisibility=hidden -fstack-protector-strong -Wunreachable-code -Wno-attributes -Werror=return-type")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -fvisibility=hidden -fstack-protector-strong -Wunreachable-code -Wno-attributes -Werror=return-type -pedantic")
endif()


# Init CMake
cmake_policy(SET CMP0048 NEW)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/indicators)

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
target_include_directories(cineia PUBLIC external/indicators/include)
target_link_libraries(cineia
        IABLib
        libkumu libasdcp libas02
)

# Системные библиотеки для статической линковки
if(UNIX)
    # Принудительная статическая линковка под Linux/MinGW
    target_link_options(cineia PRIVATE -s -static -static-libgcc -static-libstdc++)
elseif(WIN32)
    # Библиотеки WinAPI, необходимые asdcplib/kumu при статической линковке
    target_link_libraries(cineia
            indicators::indicators
            ws2_32
            iphlpapi
            advapi32
    )
    # Статическая CRT для MSVC
    if(MSVC)
        target_compile_options(cineia PRIVATE
                 $<$<CONFIG:Release>:/MT>
        )
    endif()
endif()

target_include_directories(cineia PRIVATE
        ${ASDCP_HEADER_PATH}
        ${IABLib_HEADER_PATH}
)

# Pass version numberope
target_compile_definitions(cineia PRIVATE
        PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Set install rules
install(TARGETS cineia RUNTIME DESTINATION bin)