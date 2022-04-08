# Minesweeper Marathon

[![ci](https://github.com/ebarlas/minesweeper_ftxui/actions/workflows/ci.yml/badge.svg)](https://github.com/ebarlas/minesweeper_ftxui/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/ebarlas/minesweeper_ftxui/branch/main/graph/badge.svg)](https://codecov.io/gh/ebarlas/minesweeper_ftxui)
[![CodeQL](https://github.com/ebarlas/minesweeper_ftxui/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/ebarlas/minesweeper_ftxui/actions/workflows/codeql-analysis.yml)

## Overview
Minesweeper Marathon is an adaptation of [Minesweeper](https://en.wikipedia.org/wiki/Microsoft_Minesweeper)
with a timer and rounds of increasing difficulty.

This repository was initially generated from the [C++ Best Practices GitHub template](https://github.com/cpp-best-practices/ftxui_template) 
for participation in the [April 2022 C++ Best Practices Game Jam](https://github.com/cpp-best-practices/game_jam/tree/main/Jam1_April_2022).

# Resources

### Source Code

#### Files

* [bitmap.h](src/bitmap.h), [bitmap.cpp](src/bitmap.cpp) - `Bitmap` class for modeling grid of pixels
* [board.h](src/board.h), [board.cpp](src/board.cpp) - `Board` class for modeling Minesweeper board
* [game.h](src/game.h), [game.cpp](src/game.cpp) - `Game` class for modeling Minesweeper Marathon game
* [minesweeper.cpp](src/minesweeper.cpp) - `main` function for launching a game in an FTXUI layout

#### Initialize
```
cmake -S . -B ./build
```

#### Build
```
make VERBOSE=1
```

#### Test
```
ctest -C Debug
```

### Emscripten and WebAssembly

The [Emscripten](https://emscripten.org/) toolchain emits WebAssembly suitable for inclusion in web pages.

Fortunately, [FTXUI](https://github.com/ArthurSonzogni/FTXUI/) supports Emscripten and even includes a range of examples compiled as Wasm. 

#### Files

* [CMakeLists.txt](emscripten/CMakeLists.txt) - CMake file tailored for Emscripten compilation
* [index.html](emscripten/index.html) - Webpage with minesweeper Wasm module for in-browser game play
* [highscores.html](emscripten/highscores.html) - Webpage with high score board
* [run_webassembly.py](emscripten/run_webassembly.py) - Python script for starting a local web server that includes necessary response headers

#### Initialize
```
emcmake cmake -S . -B ./build
```

#### Build
```
emmake make VERBOSE=1
```

#### Serve
```
python run_webassembly.py
```

### High Score Server

The Minesweeper Marathon high score service is a AWS Lambda Function written in Python.

The endpoints provided are `GET /scores` and `POST /scores`.

The AWS Lambda Function is attached to a CloudFront distribution.

#### Files:

* [lambda_function.py](server/lambda_function.py) - AWS Lambda Function Python script that implements simple high score REST API backed by DynamoDB