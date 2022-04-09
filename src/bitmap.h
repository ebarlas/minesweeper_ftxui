#ifndef MINESWEEPER_BITMAP
#define MINESWEEPER_BITMAP

#include <vector>

namespace minesweeper {

// These colors reflect the entire minesweeper palette.
enum class Color { red, blue, green, dark_blue, dark_red, sea_green, black, light_gray, dark_gray, white };

// A pixel is sufficient to render a minesweeper board tile.
struct Pixel
{
  Color foreground;
  Color background;
  char value;
};

// A bitmap is a two-dimensional grid of pixels.
class Bitmap
{

  const int rows;
  const int columns;
  std::vector<Pixel> pixels;

public:
  Bitmap(int rows_, int columns_);
  [[nodiscard]] int get_rows() const;
  [[nodiscard]] int get_columns() const;
  void set(int row, int col, Pixel pixel);
  [[nodiscard]] Pixel get(int row, int col) const;
};
}// namespace minesweeper

#endif