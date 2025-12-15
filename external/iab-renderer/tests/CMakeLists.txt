include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(test_data)

file( GLOB_RECURSE UNIT_TESTS Unit_Tests/*.cpp )

include(GoogleTest)

foreach( UNIT_TEST_PATH ${UNIT_TESTS} )
	get_filename_component(UNIT_TEST_NAME ${UNIT_TEST_PATH} NAME_WE)
  add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_PATH} )
  target_link_libraries( ${UNIT_TEST_NAME} ${PROJECT_NAME} GTest::gtest_main)
  gtest_discover_tests( ${UNIT_TEST_NAME})
endforeach( UNIT_TEST_PATH ${UNIT_TESTS} )