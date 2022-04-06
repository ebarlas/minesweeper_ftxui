#include "game.h"
#include <iostream>

namespace minesweeper {
std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> Game::time_now()
{
  return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
}

std::chrono::seconds Game::elapsed_time() const
{
  auto now = time_now();
  return std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
}

Game::Game(int rows,
  int columns,// NOLINT adjacent int parameters
  int time_init,
  int time_increment,
  int mines_increment,
  int mines_init)
  : time_init(time_init), time_increment(time_increment), mines_init(mines_init), mines_increment(mines_increment),
    board(rows, columns, mines_init), time(time_init)
{}

int Game::get_round() const { return round; }

int Game::get_time() const
{
  if (state == GameState::init) { return time_init; }
  if (state == GameState::ended) { return 0; }
  return time - static_cast<int>(elapsed_time().count());
}

int Game::get_mines() const { return board.get_mines(); }

void Game::on_mouse_event(int row, int col, ftxui::Mouse::Button button, ftxui::Mouse::Motion motion)
{
  using namespace ftxui;

  board.on_hover(row, col);

  if (state != GameState::ended) {
    if (row >= 0 && row < board.get_rows() && col >= 0 && col < board.get_columns()) {
      if (motion == Mouse::Released) {
        if (button == Mouse::Button::Left) {
          if (state == GameState::init) {
            state = GameState::playing;
            start_time = time_now();
          }
          board.on_left_click(row, col);
        } else if (button == Mouse::Button::Right) {
          board.on_right_click(row, col);
        }
      }
    }
  }

  if (board.is_complete()) {
    board.update(board.get_mines() + mines_increment);
    round++;
    time += time_increment;
  }
}

void Game::on_refresh_event()
{
  if (state == GameState::playing && elapsed_time().count() >= time) {
    state = GameState::ended;
#if defined(__EMSCRIPTEN__)//use stderr channel to communicate score to JavaScript
    std::cerr << std::to_string(round) << std::endl;
#endif
  }
}

void Game::on_new_game()
{
  state = GameState::init;
  round = 1;
  time = time_init;
  board.update(mines_init);
}

void Game::on_reset_game()
{
  if (state == GameState::playing) { board.restore(); }
}

ftxui::Canvas Game::render_board() const { return board.render(); }
}// namespace minesweeper