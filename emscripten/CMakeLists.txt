cmake_minimum_required(VERSION 3.20)
project(minesweeper)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v2.0.0
        )

FetchContent_GetProperties(ftxui)
if (NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

if (EMSCRIPTEN)
    string(APPEND CMAKE_CXX_FLAGS " -O2")
    string(APPEND CMAKE_CXX_FLAGS " -s USE_PTHREADS")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASYNCIFY")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -s PROXY_TO_PTHREAD")
    target_link_options(component PUBLIC "SHELL: -s TOTAL_MEMORY=33554432")
endif()

add_executable(minesweeper ../src/minesweeper.cpp ../src/bitmap.cpp ../src/board.cpp ../src/game.cpp)

target_link_libraries(minesweeper
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
        )

foreach(file
        "index.html"
        "run_webassembly.py")
    configure_file(${file} ${file})
endforeach(file)