#ifndef MINESWEEPER_GAME
#define MINESWEEPER_GAME

#include "board.h"

#include "ftxui/component/screen_interactive.hpp"

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
  Game(int rows,
    int columns,
    int time_init,
    int time_increment,
    int mines_increment,
    int mines_init);// NOLINT adjacent int params
  [[nodiscard]] int get_round() const;
  [[nodiscard]] int get_time() const;
  [[nodiscard]] int get_mines() const;
  void on_mouse_event(int row, int col, ftxui::Mouse::Button button, ftxui::Mouse::Motion motion);
  void on_refresh_event();
  void on_new_game();
  void on_reset_game();
  [[nodiscard]] ftxui::Canvas render_board() const;
};
}// namespace minesweeper

#endif