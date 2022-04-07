#ifndef MINESWEEPER_GAME
#define MINESWEEPER_GAME

#include "board.h"

namespace minesweeper {

class Game
{
  enum class GameState { init, playing, ended };

  const int time_init;
  const int time_increment;
  const int mines_init;
  const int mines_increment;

  Board board;

  GameState state = GameState::init;
  int round = 1;
  int time;
  std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> start_time{};

  static std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> time_now();
  [[nodiscard]] std::chrono::seconds elapsed_time() const;

public:
  Game(int rows_, int cols_, int time_init_, int time_inc_, int mines_init_, int mines_inc_);// NOLINT adj int params
  [[nodiscard]] int get_round() const;
  [[nodiscard]] int get_time() const;
  [[nodiscard]] int get_mines() const;
  void on_mouse_event(int row, int col, bool left_click, bool right_click, bool mouse_up);
  void on_key_up();
  void on_refresh_event();
  void on_new_game();
  void on_reset_game();
  [[nodiscard]] Bitmap render_board() const;
};
}// namespace minesweeper

#endif