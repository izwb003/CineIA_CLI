cmake_minimum_required (VERSION 3.10)
project(IABLib)
set(CMAKE_CXX_STANDARD 20)

include_directories(include src/lib)

# library

file(GLOB_RECURSE LIB_SRC_FILES CONFIGURE_DEPENDS src/lib/*.cpp)

if(NOT APPLE) 
  list(REMOVE_ITEM LIB_SRC_FILES ${PROJECT_SOURCE_DIR}/src/lib/coreutils/VectDSPMacAccelerate.cpp)
endif()

add_library(${PROJECT_NAME} ${LIB_SRC_FILES})

if(APPLE)
  find_library(ACCEL_UNIT Accelerate)
  if (NOT ACCEL_UNIT)
    message(FATAL_ERROR "MacOS Accelerate Framework not found")
  endif()
  target_link_libraries(${PROJECT_NAME} ${ACCEL_UNIT})
endif()

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

# sample app

file(GLOB_RECURSE APP_SRC_FILES CONFIGURE_DEPENDS src/apps/RendererExample/*.cpp)

add_executable(RenderExample ${APP_SRC_FILES})
target_link_libraries(RenderExample ${PROJECT_NAME} pthread)

# documentation

find_package(Doxygen)

if(DOXYGEN_FOUND)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_GENERATE_TREEVIEW YES)

  doxygen_add_docs(doxygen include src/lib)
endif()

# tests

# Allow parent project to disable building upstream tests (they fail on some toolchains
# and are not needed when IABLib is used as a third-party library).
option(IABLIB_BUILD_TESTS "Build iab-renderer unit and performance tests" OFF)

if(IABLIB_BUILD_TESTS)
  add_subdirectory(tests)
endif()
