#ifndef MINESWEEPER_BOARD
#define MINESWEEPER_BOARD

#include "bitmap.h"
#include <array>
#include <vector>
#include <functional>

namespace minesweeper {

// Cell is the data model for a minesweeper board tile. It can represent all tile data states.
struct Cell
{
  int row;
  int col;
  bool mine;
  bool flagged;
  bool revealed;
  int adjacentMines;
};

// Board is a two-dimensional grid of cells. It can be rendered as a bitmap.
class Board
{
  const std::array<Color, 9> COLORS{ Color::black,
    Color::blue,
    Color::green,
    Color::red,
    Color::dark_blue,
    Color::dark_red,
    Color::sea_green,
    Color::black,
    Color::black };

  const int rows;
  const int columns;

  int mines;

  std::vector<Cell> cells;

  int hover_row = -1;
  int hover_col = -1;

  void reset();
  void for_each_adjacent(int row, int col, const std::function<void(Cell &cell)> &fn);
  Cell &at(int row, int col);
  [[nodiscard]] const Cell &at(int row, int col) const;
  void assign_mines();
  void assign_adjacent_mines();
  int count_adjacent_flags(int row, int col);
  void reveal_neighbors(int row, int col);
  void reveal(int row, int col);
  void render(Bitmap &bitmap, int row, int col) const;

public:
  explicit Board(int rows_, int columns_, int mines_);
  [[nodiscard]] Bitmap render() const;
  void on_left_click(int row, int col);
  void on_right_click(int row, int col);
  void on_key_up();
  void on_hover(int row, int col);
  void restore();
  void update(int mines_update);
  [[nodiscard]] int get_mines() const;
  [[nodiscard]] int get_rows() const;
  [[nodiscard]] int get_columns() const;
  [[nodiscard]] bool is_alive() const;
  [[nodiscard]] bool is_complete() const;
};
}// namespace minesweeper

#endif