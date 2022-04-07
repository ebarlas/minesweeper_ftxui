find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)

add_executable(minesweeper board.cpp game.cpp minesweeper.cpp)

target_link_system_libraries(
        minesweeper
        PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component)

target_include_directories(minesweeper PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")