find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

add_library(catch_main OBJECT catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_link_libraries(catch_main PRIVATE project_options)

add_executable(board_tests board_tests.cpp ../src/bitmap.cpp ../src/board.cpp)
target_include_directories(board_tests PRIVATE ../src)
target_link_libraries(board_tests PRIVATE project_warnings project_options catch_main)

target_include_directories(board_tests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
        board_tests
        TEST_PREFIX
        "unittests."
        REPORTER
        xml
        OUTPUT_DIR
        .
        OUTPUT_PREFIX
        "unittests."
        OUTPUT_SUFFIX
        .xml)

add_executable(game_tests game_tests.cpp ../src/bitmap.cpp ../src/board.cpp ../src/game.cpp)
target_include_directories(game_tests PRIVATE ../src)
target_link_libraries(game_tests PRIVATE project_warnings project_options catch_main)

target_include_directories(game_tests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
        game_tests
        TEST_PREFIX
        "unittests."
        REPORTER
        xml
        OUTPUT_DIR
        .
        OUTPUT_PREFIX
        "unittests."
        OUTPUT_SUFFIX
        .xml)