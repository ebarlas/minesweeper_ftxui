#ifndef MINESWEEPER_BOARD
#define MINESWEEPER_BOARD

#include <array>
#include <vector>

#include "ftxui/dom/canvas.hpp"
#include "ftxui/screen/color.hpp"

namespace minesweeper {
struct Cell
{
  int row;
  int col;
  bool mine;
  bool flagged;
  bool revealed;
  int adjacentMines;
};

class Board
{
  const std::array<ftxui::Color, 9> COLORS{ ftxui::Color::Black,
    ftxui::Color::Blue,
    ftxui::Color::Green,
    ftxui::Color::Red,
    ftxui::Color::DarkBlue,
    ftxui::Color::DarkRed,
    ftxui::Color::SeaGreen1,
    ftxui::Color::Black,
    ftxui::Color::Black };

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
  static void draw(ftxui::Canvas &canvas, int row, int col, const std::string &value, ftxui::Color fg, ftxui::Color bg);
  void render(ftxui::Canvas &canvas, int row, int col) const;

public:
  explicit Board(int rows_, int columns_, int mines_);
  [[nodiscard]] ftxui::Canvas render() const;
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