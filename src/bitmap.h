#ifndef MINESWEEPER_BITMAP
#define MINESWEEPER_BITMAP

#include <vector>

namespace minesweeper {
enum class Color { red, blue, green, dark_blue, dark_red, sea_green, black, light_gray, dark_gray, white };

struct Pixel
{
  Color foreground;
  Color background;
  char value;
};

class Bitmap
{

  const int rows;
  const int columns;
  std::vector<Pixel> pixels;

public:
  Bitmap(int rows_, int columns_);
  [[nodiscard]] int get_rows() const;
  [[nodiscard]] int get_columns() const;
  void set(int row, int col, Color fg, Color bg, char val);
  [[nodiscard]] Pixel get(int row, int col) const;
};
}// namespace minesweeper

#endif