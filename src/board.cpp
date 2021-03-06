#include <chrono>
#include <numeric>
#include <random>

#include "board.h"

namespace minesweeper {
void Board::reset()
{
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      auto &cell = at(row, col);
      cell.row = row;
      cell.col = col;
      cell.mine = false;
      cell.flagged = false;
      cell.revealed = false;
      cell.adjacentMines = 0;
    }
  }
  assign_mines();
  assign_adjacent_mines();
}

void Board::for_each_adjacent(int row, int col, const std::function<void(Cell &cell)> &fn)// NOLINT adj int parameters
{
  for (int r = row - 1; r <= row + 1; r++) {
    for (int c = col - 1; c <= col + 1; c++) {
      if (r >= 0 && r < rows && c >= 0 && c < columns) {
        if (c != col || r != row) { fn(at(r, c)); }
      }
    }
  }
}

Cell &Board::at(int row, int col) { return cells.at(static_cast<unsigned int>(row * columns + col)); }

[[nodiscard]] const Cell &Board::at(int row, int col) const
{
  return cells.at(static_cast<unsigned int>(row * columns + col));
}

void Board::assign_mines()
{
  auto now = std::chrono::system_clock::now();
  auto second_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
  std::mt19937 mt{ static_cast<unsigned int>(second_since_epoch) };
  std::uniform_int_distribution dist{ 0, rows * columns - 1 };// random values over closed (inclusive) range
  int remaining = mines;
  while (remaining > 0) {
    auto next = dist(mt);
    auto &cell = cells.at(static_cast<unsigned int>(next));
    if (!cell.mine) {
      cell.mine = true;
      remaining--;
    }
  }
}

void Board::assign_adjacent_mines()
{
  for (auto &target : cells) {
    for_each_adjacent(target.row, target.col, [&target](const Cell &adj) {
      if (adj.mine) { target.adjacentMines++; }
    });
  }
}

int Board::count_adjacent_flags(int row, int col)// NOLINT adjacent int parameters
{
  int count = 0;
  for_each_adjacent(row, col, [&count](const Cell &cell) {
    if (cell.flagged) { count++; }
  });
  return count;
}

void Board::reveal_neighbors(int row, int col)// NOLINT adjacent int parameters
{
  for_each_adjacent(row, col, [this](const Cell &cell) {
    if (!cell.flagged && !cell.revealed) { reveal(cell.row, cell.col); }
  });
}

void Board::reveal(int row, int col)// NOLINT adjacent int parameters
{
  auto &cell = at(row, col);
  cell.revealed = true;
  if (!cell.mine && cell.adjacentMines == 0) { reveal_neighbors(row, col); }
}

void Board::render(Bitmap &bitmap, int row, int col) const// NOLINT adjacent int parameters
{
  const auto &cell = at(row, col);
  auto is_sel = row == hover_row && col == hover_col;
  if (!cell.revealed && !cell.flagged) {
    bitmap.set(row, col, { Color::light_gray, is_sel ? Color::dark_gray : Color::light_gray, ' ' });
  } else if (!cell.revealed && cell.flagged) {
    bitmap.set(row, col, { Color::red, is_sel ? Color::dark_gray : Color::light_gray, '*' });
  } else if (cell.mine) {
    bitmap.set(row, col, { Color::red, is_sel ? Color::dark_gray : Color::red, ' ' });
  } else if (cell.adjacentMines == 0) {
    bitmap.set(row, col, { Color::white, is_sel ? Color::dark_gray : Color::white, ' ' });
  } else {
    auto color = COLORS.at(static_cast<unsigned int>(cell.adjacentMines));
    auto value = static_cast<char>(cell.adjacentMines + '0');// ASCII arithmetic!
    bitmap.set(row, col, { color, is_sel ? Color::dark_gray : Color::white, value });
  }
}

Board::Board(int rows_, int columns_, int mines_)// NOLINT adjacent int parameters
  : rows(rows_), columns(columns_), mines(mines_), cells(static_cast<std::vector<Cell>::size_type>(rows * columns))
{
  reset();
}

Bitmap Board::render() const
{
  auto bitmap = Bitmap(rows, columns);
  for (const auto &cell : cells) { render(bitmap, cell.row, cell.col); }
  return bitmap;
}

void Board::on_left_click(int row, int col)
{
  if (is_alive()) {
    const auto &cell = at(row, col);
    if (cell.revealed && cell.adjacentMines == count_adjacent_flags(row, col)) {
      reveal_neighbors(row, col);
    } else if (!cell.flagged) {
      reveal(row, col);
    }
  }
}

void Board::on_right_click(int row, int col)
{
  if (is_alive()) {
    auto &cell = at(row, col);
    if (cell.revealed && cell.adjacentMines == count_adjacent_flags(row, col)) {
      reveal_neighbors(row, col);
    } else if (!cell.revealed) {
      cell.flagged = !cell.flagged;
    }
  }
}

void Board::on_hover(int row, int col)// NOLINT adjacent int parameters
{
  hover_row = row;
  hover_col = col;
}

void Board::restore()
{
  for (auto &cell : cells) {
    cell.flagged = false;
    cell.revealed = false;
  }
}

void Board::update(int mines_update)
{
  mines = mines_update;
  reset();
}

int Board::get_mines() const { return mines; }

int Board::get_rows() const { return rows; }

int Board::get_columns() const { return columns; }

bool Board::is_alive() const
{
  auto fn = [](bool alive, const Cell &c) { return alive && !(c.revealed && c.mine); };
  return std::accumulate(cells.cbegin(), cells.cend(), true, fn);
}

bool Board::is_complete() const
{
  auto fn = [](int sum, const Cell &c) { return c.revealed ? sum + 1 : sum; };
  auto revealed = std::accumulate(cells.cbegin(), cells.cend(), 0, fn);
  return revealed == static_cast<int>(cells.size()) - mines;
}
void Board::on_key_up() { on_right_click(hover_row, hover_col); }
}// namespace minesweeper