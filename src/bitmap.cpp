#include "bitmap.h"
namespace minesweeper {
Bitmap::Bitmap(int rows_, int columns_)
  : rows(rows_), columns(columns_), pixels(static_cast<unsigned long>(rows_ * columns_))
{}
int Bitmap::get_rows() const { return rows; }
int Bitmap::get_columns() const { return columns; }
void Bitmap::set(int row, int col, minesweeper::Color fg, minesweeper::Color bg, char val)
{
  auto &pixel = pixels[static_cast<unsigned int>(row * columns + col)];
  pixel.background = bg;
  pixel.foreground = fg;
  pixel.value = val;
}
Pixel Bitmap::get(int row, int col) const { return pixels[static_cast<unsigned int>(row * columns + col)]; }
}// namespace minesweeper