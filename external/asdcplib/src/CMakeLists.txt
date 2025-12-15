option(USE_ASDCP_JXS "Build with JPEG XS support in AS-DCP?" OFF)
option(WITH_SSL "Build encryption support?" ON)
option(WITH_XML "Build XML support?" ON)

if (WITH_SSL)
    find_package(OpenSSL CONFIG REQUIRED)
endif ()
if (WITH_XML)
    find_package(XercesC CONFIG REQUIRED)
endif ()

set(UseRandomUUID OFF CACHE BOOL "")

if (WITH_SSL)
    set (HAVE_OPENSSL 1)
    message(STATUS "Building with encryption support")
    add_definitions(/DHAVE_OPENSSL=1)
else()
    message(STATUS "Building without encryption support")
endif()

if (WITH_XML)
    set (HAVE_XERCES_C 1)
    message(STATUS "Building with XML parse support")
    add_definitions(/DHAVE_XERCES_C=1)
else()
    message(STATUS "Building without XML parse support")
endif()

# This lib. doesn't export from dll with __declspec(dllexport). So this lib. must be built statically on Windows.
if(WIN32)
    if (BUILD_SHARED_LIBS)
        message(STATUS "Building shared libs is not supported for WIN32")
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared or static libs?" FORCE)
    endif()
else()
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared or static libs?")
endif()

# ----------helpers----------
# Helper to append a file to a list only if it exists
function(append_if_exists out_var)
    foreach(_f IN LISTS ARGN)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
            list(APPEND ${out_var} "${_f}")
        endif()
    endforeach()
    set(${out_var} "${${out_var}}" PARENT_SCOPE)
endfunction()

# ----------libkumu----------
# source (only .cpp files; headers are NOT added to sources)
set(kumu_src KM_fileio.cpp KM_log.cpp KM_util.cpp KM_tai.cpp KM_prng.cpp KM_xml.cpp)
# optional sources
append_if_exists(kumu_src KM_aes.cpp KM_sha1.cpp)

# ----------libasdcp----------
# source (code files only)
set(asdcp_src MPEG2_Parser.cpp MPEG.cpp JP2K_Codestream_Parser.cpp
        JP2K_Sequence_Parser.cpp JP2K.cpp PCM_Parser.cpp Wav.cpp
        KLV.cpp Dict.cpp MXFTypes.cpp MXF.cpp Index.cpp Metadata.cpp AS_DCP.cpp AS_DCP_MXF.cpp TimedText_Parser.cpp
        h__Reader.cpp h__Writer.cpp AS_DCP_MPEG2.cpp AS_DCP_JP2K.cpp
        AS_DCP_PCM.cpp AS_DCP_TimedText.cpp PCMParserList.cpp MDD.cpp
        AS_DCP_ATMOS.cpp AS_DCP_DCData.cpp DCData_ByteStream_Parser.cpp DCData_Sequence_Parser.cpp AtmosSyncChannel_Generator.cpp
        AtmosSyncChannel_Mixer.cpp PCMDataProviders.cpp SyncEncoder.c CRC16.c UUIDInformation.c)

# optional AES implementation (only if OpenSSL enabled AND file exists)
if (HAVE_OPENSSL)
    append_if_exists(asdcp_src AS_DCP_AES.cpp)
endif()

# optional JPEG XS sources
if (USE_ASDCP_JXS)
    append_if_exists(asdcp_src AS_DCP_JXS.cpp JXS_Codestream_Parser.cpp JXS_Sequence_Parser.cpp JXS.cpp)
endif()

# headers for deployment (filter only existing)
set(asdcp_deploy_header)
append_if_exists(asdcp_deploy_header AS_DCP.h AS_DCP_JXS.h PCMParserList.h AS_DCP_internal.h KM_error.h KM_fileio.h KM_util.h KM_memio.h KM_tai.h KM_platform.h KM_log.h KM_mutex.h)
if (WIN32)
    append_if_exists(asdcp_deploy_header dirent_win.h)
endif()

# ----------as02----------
# source (code files only)
set(as02_src h__02_Reader.cpp h__02_Writer.cpp AS_02_ISXD.cpp AS_02_JP2K.cpp
        AS_02_PCM.cpp ST2052_TextParser.cpp AS_02_TimedText.cpp AS_02_ACES.cpp ACES_Codestream_Parser.cpp ACES_Sequence_Parser.cpp ACES.cpp AS_02_IAB.cpp)
# optional JXS for AS-02
if (USE_ASDCP_JXS)
    append_if_exists(as02_src AS_02_JXS.cpp)
endif()

# headers for deployment (filter only existing)
set(as02_deploy_header)
append_if_exists(as02_deploy_header AS_02.h AS_02_JXS.h Metadata.h MXF.h MXFTypes.h KLV.h MDD.h AS_02_ACES.h ACES.h AS_02_IAB.h AS_02_internal.h AS_02_PHDR.h)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(/DPACKAGE_VERSION=\"${VERSION_STRING}\")
if(WIN32)
    add_definitions(/DKM_WIN32 /D_CONSOLE /DASDCP_PLATFORM=\"win32\" /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
elseif(UNIX)
    add_definitions(/DASDCP_PLATFORM=\"unix\")
endif(WIN32)

if(UseRandomUUID)
    add_definitions(/DCONFIG_RANDOM_UUID=1)
endif(UseRandomUUID)

set(CMAKE_DEBUG_POSTFIX _d) # Append "_d" if debug lib.

add_library(libkumu STATIC ${kumu_src})
if (HAVE_OPENSSL)
    target_link_libraries(libkumu general OpenSSL::SSL OpenSSL::Crypto XercesC::XercesC)
endif()
set_target_properties(libkumu PROPERTIES PREFIX "" VERSION ${VERSION_STRING} SOVERSION ${VERSION_MAJOR})

add_library(libasdcp STATIC ${asdcp_src})
target_link_libraries(libasdcp general libkumu)
set_target_properties(libasdcp PROPERTIES PREFIX "" VERSION ${VERSION_STRING} SOVERSION ${VERSION_MAJOR})

add_library(libas02 STATIC ${as02_src})
target_link_libraries(libas02 general libasdcp)
set_target_properties(libas02 PROPERTIES PREFIX "" VERSION ${VERSION_STRING} SOVERSION ${VERSION_MAJOR})

add_executable(blackwave "blackwave.cpp")
target_link_libraries(blackwave general libasdcp)
if(WIN32)
    target_link_libraries(blackwave general Advapi32.lib)
endif(WIN32)

add_executable(wavesplit "wavesplit.cpp")
target_link_libraries(wavesplit general libasdcp)
if(WIN32)
    target_link_libraries(wavesplit general Advapi32.lib)
endif(WIN32)

add_executable(kmuuidgen "kmuuidgen.cpp")
target_link_libraries(kmuuidgen general libkumu OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(kmuuidgen general Advapi32.lib)
endif(WIN32)

add_executable(kmrandgen "kmrandgen.cpp")
target_link_libraries(kmrandgen general libkumu OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(kmrandgen general Advapi32.lib)
endif(WIN32)

add_executable(kmfilegen "kmfilegen.cpp")
target_link_libraries(kmfilegen general libkumu OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(kmfilegen general Advapi32.lib)
endif(WIN32)

add_executable(klvwalk "klvwalk.cpp")
target_link_libraries(klvwalk general libasdcp OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(klvwalk general Advapi32.lib)
endif(WIN32)

add_executable(asdcp-test "asdcp-test.cpp")
target_link_libraries(asdcp-test general libasdcp OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(asdcp-test general Advapi32.lib)
endif(WIN32)

add_executable(asdcp-wrap "asdcp-wrap.cpp")
target_link_libraries(asdcp-wrap general libasdcp OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(asdcp-wrap general Advapi32.lib)
endif(WIN32)

add_executable(asdcp-unwrap "asdcp-unwrap.cpp")
target_link_libraries(asdcp-unwrap general libasdcp OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(asdcp-unwrap general Advapi32.lib)
endif(WIN32)

add_executable(asdcp-info "asdcp-info.cpp")
target_link_libraries(asdcp-info general libasdcp OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(asdcp-info general Advapi32.lib)
endif(WIN32)

add_executable(asdcp-util "asdcp-util.cpp")
target_link_libraries(asdcp-util general libasdcp OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(asdcp-util general Advapi32.lib)
endif(WIN32)

add_executable(j2c-test "j2c-test.cpp")
target_link_libraries(j2c-test general libasdcp OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(j2c-test general Advapi32.lib)
endif(WIN32)

add_executable(as-02-wrap "as-02-wrap.cpp")
target_link_libraries(as-02-wrap general libas02 OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(as-02-wrap general Advapi32.lib)
endif(WIN32)

if (USE_ASDCP_JXS)
    add_executable(as-02-wrap-jxs "as-02-wrap-jxs.cpp")
    target_link_libraries(as-02-wrap-jxs general libas02 OpenSSL::SSL OpenSSL::Crypto)
    if(WIN32)
        target_link_libraries(as-02-wrap-jxs general Advapi32.lib)
    endif(WIN32)
endif ()

add_executable(as-02-unwrap "as-02-unwrap.cpp")
target_link_libraries(as-02-unwrap general libas02 OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(as-02-unwrap general Advapi32.lib)
endif(WIN32)

add_executable(as-02-info "as-02-info.cpp")
target_link_libraries(as-02-info general libas02 OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(as-02-info general Advapi32.lib)
endif(WIN32)

# add the install target
set (install_includes)
if (HAVE_OPENSSL)
    list(APPEND install_includes OpenSSL::SSL OpenSSL::Crypto)
endif()

install(TARGETS libkumu libasdcp libas02 EXPORT asdcplibtargets RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib INCLUDES DESTINATION "${install_includes}")

set(install_targets blackwave wavesplit klvwalk asdcp-test asdcp-wrap asdcp-unwrap asdcp-info asdcp-util j2c-test as-02-wrap as-02-unwrap as-02-info kmfilegen kmuuidgen kmrandgen)

if (USE_ASDCP_JXS)
    list(APPEND install_targets as-02-wrap-jxs)
endif ()

install(TARGETS ${install_targets} RUNTIME DESTINATION bin)
install(FILES ${as02_deploy_header} ${asdcp_deploy_header} DESTINATION include)
install(EXPORT asdcplibtargets DESTINATION targets)
