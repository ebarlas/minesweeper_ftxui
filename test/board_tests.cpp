#include "board.h"
#include <catch2/catch.hpp>

void check_default_render(const minesweeper::Board &board)
{
  auto bitmap = board.render();
  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < 2; c++) {
      auto pixel = bitmap.get(r, c);
      REQUIRE(pixel.foreground == minesweeper::Color::light_gray);
      REQUIRE(pixel.background == minesweeper::Color::light_gray);
    }
  }
}

void verify_one(const minesweeper::Pixel &pixel)
{
  REQUIRE(pixel.value == '1');
  REQUIRE(pixel.foreground == minesweeper::Color::blue);
  REQUIRE(pixel.background == minesweeper::Color::white);
}

std::pair<int, int> find_mine(minesweeper::Board &board)
{
  for (int r = 0; r < board.get_rows(); r++) {
    for (int c = 0; c < board.get_columns(); c++) {
      board.restore();
      board.on_left_click(r, c);
      if (!board.is_alive()) { return { r, c }; }
    }
  }
  return { -1, -1 };
}

TEST_CASE("Initial board state", "[board]")
{
  minesweeper::Board board{ 2, 2, 1 };
  REQUIRE(board.get_mines() == 1);
  REQUIRE(board.get_rows() == 2);
  REQUIRE(board.get_columns() == 2);
  REQUIRE(board.is_alive());
  REQUIRE_FALSE(board.is_complete());
  check_default_render(board);
}

TEST_CASE("Hover over cell", "[board]")
{
  minesweeper::Board board{ 2, 2, 1 };

  board.on_hover(0, 0);
  auto pixel = board.render().get(0, 0);
  REQUIRE(pixel.foreground == minesweeper::Color::light_gray);
  REQUIRE(pixel.background == minesweeper::Color::dark_gray);

  board.on_hover(-1, -1);
  pixel = board.render().get(0, 0);
  REQUIRE(pixel.foreground == minesweeper::Color::light_gray);
  REQUIRE(pixel.background == minesweeper::Color::light_gray);
}

TEST_CASE("Complete board", "[board]")
{
  minesweeper::Board board{ 2, 2, 0 };
  board.on_left_click(0, 0);
  REQUIRE(board.is_complete());
  auto pixel = board.render().get(0, 0);
  REQUIRE(pixel.foreground == minesweeper::Color::white);
  REQUIRE(pixel.background == minesweeper::Color::white);
}

TEST_CASE("Flag cell", "[board]")
{
  minesweeper::Board board{ 2, 2, 1 };
  board.on_right_click(0, 0);
  auto pixel = board.render().get(0, 0);
  REQUIRE(pixel.value == '*');
  REQUIRE(pixel.foreground == minesweeper::Color::red);
  REQUIRE(pixel.background == minesweeper::Color::light_gray);
}

TEST_CASE("Keystroke flag cell", "[board]")
{
  minesweeper::Board board{ 2, 2, 1 };
  board.on_hover(0, 0);
  board.on_key_up();
  auto pixel = board.render().get(0, 0);
  REQUIRE(pixel.value == '*');
  REQUIRE(pixel.foreground == minesweeper::Color::red);
  REQUIRE(pixel.background == minesweeper::Color::dark_gray);
}

TEST_CASE("Reveal mine", "[board]")
{
  minesweeper::Board board{ 2, 2, 4 };
  board.on_left_click(0, 0);
  REQUIRE_FALSE(board.is_complete());
  REQUIRE_FALSE(board.is_alive());

  auto pixel = board.render().get(0, 0);
  REQUIRE(pixel.foreground == minesweeper::Color::red);
  REQUIRE(pixel.background == minesweeper::Color::red);
}

TEST_CASE("Board update", "[board]")
{
  minesweeper::Board board{ 2, 2, 0 };
  REQUIRE(board.get_mines() == 0);
  board.update(4);
  REQUIRE(board.get_mines() == 4);
}

TEST_CASE("Restore board", "[board]")
{
  minesweeper::Board board{ 2, 2, 4 };
  board.on_left_click(0, 0);
  REQUIRE_FALSE(board.is_alive());
  board.restore();
  REQUIRE(board.is_alive());
}

TEST_CASE("Reveal neighbors left click", "[board]")
{
  minesweeper::Board board{ 2, 2, 1 };
  const auto [mine_row, mine_col] = find_mine(board);
  board.restore();
  board.on_right_click(mine_row, mine_col);
  auto click_row = (mine_row + 1) % 2;
  auto click_col = mine_col;
  board.on_left_click(click_row, click_col);
  verify_one(board.render().get(click_row, click_col));
  board.on_left_click(click_row, click_col);
  REQUIRE(board.is_alive());
  REQUIRE(board.is_complete());
}

TEST_CASE("Reveal neighbors right click", "[board]")
{
  minesweeper::Board board{ 2, 2, 1 };
  const auto [mine_row, mine_col] = find_mine(board);
  board.restore();
  board.on_right_click(mine_row, mine_col);
  auto click_row = (mine_row + 1) % 2;
  auto click_col = mine_col;
  board.on_left_click(click_row, click_col);
  verify_one(board.render().get(click_row, click_col));
  board.on_right_click(click_row, click_col);
  REQUIRE(board.is_alive());
  REQUIRE(board.is_complete());
}