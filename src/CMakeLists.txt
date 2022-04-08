add_executable(minesweeper bitmap.cpp board.cpp game.cpp minesweeper.cpp)

target_link_libraries(minesweeper PRIVATE project_options project_warnings)

target_link_system_libraries(
        minesweeper
        PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component)

target_include_directories(minesweeper PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")