#include "game.h"
#include <catch2/catch.hpp>

void check_default_render(const minesweeper::Bitmap &bitmap)
{
  for (int r = 0; r < bitmap.get_rows(); r++) {
    for (int c = 0; c < bitmap.get_columns(); c++) {
      auto pixel = bitmap.get(r, c);
      REQUIRE(pixel.foreground == minesweeper::Color::light_gray);
      REQUIRE(pixel.background == minesweeper::Color::light_gray);
    }
  }
}

TEST_CASE("Initial game state", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 5, 0, 5 };// NOLINT magic numbers
  REQUIRE(game.get_mines() == 0);
  REQUIRE(game.get_time() == 5);
  REQUIRE(game.get_round() == 1);
  check_default_render(game.render_board());
}

TEST_CASE("Advance rounds", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 5, 0, 0 };// NOLINT magic numbers
  game.on_mouse_event(0, 0, true, false, true);
  REQUIRE(game.get_round() == 2);
}

TEST_CASE("Time expired", "[game]")
{
  // time_init: 0, time_inc: 0, mines_init: 0, mines_inc: 0
  minesweeper::Game game{ 2, 2, 0, 0, 0, 0 };// NOLINT magic numbers

  // click to start timer and advance to round 2
  game.on_mouse_event(0, 0, true, false, true);
  REQUIRE(game.get_round() == 2);
  REQUIRE(game.get_time() == 0);// game is not actually over yet, since game-over is processed in refresh

  // refresh event ends game
  game.on_refresh_event();

  // subsequent click has no effect
  game.on_mouse_event(0, 0, true, false, true);
  REQUIRE(game.get_round() == 2);
  REQUIRE(game.get_time() == 0);

  game.on_new_game();
  REQUIRE(game.get_round() == 1);
}

TEST_CASE("Reset round", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 0, 4, 0 };// NOLINT magic numbers
  game.on_mouse_event(0, 0, true, false, true);
  REQUIRE(game.get_round() == 1);

  game.on_reset_game();
}

TEST_CASE("Flag mine", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 0, 4, 0 };// NOLINT magic numbers
  game.on_mouse_event(0, 0, false, true, true);
  auto canvas = game.render_board();
  auto pixel = canvas.get(0, 0);
  REQUIRE(pixel.value == '*');
  REQUIRE(pixel.foreground == minesweeper::Color::red);
  REQUIRE(pixel.background == minesweeper::Color::dark_gray);
}

TEST_CASE("Flag mine with keystroke", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 0, 4, 0 };// NOLINT magic numbers
  game.on_mouse_event(0, 0, false, false, true);
  game.on_key_up();
  auto pixel = game.render_board().get(0, 0);
  REQUIRE(pixel.value == '*');
  REQUIRE(pixel.foreground == minesweeper::Color::red);
  REQUIRE(pixel.background == minesweeper::Color::dark_gray);
}