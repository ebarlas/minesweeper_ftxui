#include "game.h"
#include <catch2/catch.hpp>

void check_default_render(const ftxui::Canvas &canvas)
{
  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < 2; c++) {
      auto pixel = canvas.GetPixel(c, r);
      REQUIRE(pixel.foreground_color == ftxui::Color::GrayLight);
      REQUIRE(pixel.background_color == ftxui::Color::GrayLight);
    }
  }
}

TEST_CASE("Initial game state", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 5, 5, 0 };// NOLINT magic numbers
  REQUIRE(game.get_mines() == 0);
  REQUIRE(game.get_time() == 5);
  REQUIRE(game.get_round() == 1);
  check_default_render(game.render_board());
}

TEST_CASE("Advance rounds", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 5, 0, 0 };// NOLINT magic numbers
  game.on_mouse_event(0, 0, ftxui::Mouse::Button::Left, ftxui::Mouse::Motion::Released);
  REQUIRE(game.get_round() == 2);
}

TEST_CASE("Time expired", "[game]")
{
  // time_init: 0, time_inc: 0, mines_init: 0, mines_inc: 0
  minesweeper::Game game{ 2, 2, 0, 0, 0, 0 };// NOLINT magic numbers

  // click to start timer and advance to round 2
  game.on_mouse_event(0, 0, ftxui::Mouse::Button::Left, ftxui::Mouse::Motion::Released);
  REQUIRE(game.get_round() == 2);
  REQUIRE(game.get_time() == 0); // game is not actually over yet, since game-over is processed in refresh

  // refresh event ends game
  game.on_refresh_event();

  // subsequent click has no effect
  game.on_mouse_event(0, 0, ftxui::Mouse::Button::Left, ftxui::Mouse::Motion::Released);
  REQUIRE(game.get_round() == 2);
  REQUIRE(game.get_time() == 0);

  game.on_new_game();
  REQUIRE(game.get_round() == 1);
}

TEST_CASE("Reset round", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 0, 0, 4 };// NOLINT magic numbers
  game.on_mouse_event(0, 0, ftxui::Mouse::Button::Left, ftxui::Mouse::Motion::Released);
  REQUIRE(game.get_round() == 1);

  game.on_reset_game();
}

TEST_CASE("Flag mine", "[game]")
{
  minesweeper::Game game{ 2, 2, 5, 0, 0, 4 };// NOLINT magic numbers
  game.on_mouse_event(0, 0, ftxui::Mouse::Button::Right, ftxui::Mouse::Motion::Released);
}