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

Game::Game(int rows_, int cols_, int time_init_, int time_inc_, int mines_init_, int mines_inc_)// NOLINT adj int params
  : time_init(time_init_), time_increment(time_inc_), mines_init(mines_init_), mines_increment(mines_inc_),
    board(rows_, cols_, mines_init_), time(time_init)
{}

int Game::get_round() const { return round; }

int Game::get_time() const
{
  if (state == GameState::init) { return time_init; }
  if (state == GameState::ended) { return 0; }
  return time - static_cast<int>(elapsed_time().count());
}

int Game::get_mines() const { return board.get_mines(); }

void Game::on_mouse_event(int row, int col, bool left_click, bool right_click, bool mouse_up)
{
  board.on_hover(row, col);

  if (state != GameState::ended) {
    if (row >= 0 && row < board.get_rows() && col >= 0 && col < board.get_columns()) {
      if (mouse_up) {
        if (left_click) {
          if (state == GameState::init) {
            state = GameState::playing;
            start_time = time_now();
          }
          board.on_left_click(row, col);
        } else if (right_click) {
          board.on_right_click(row, col);
        }
      }
    }
  }

  if (state == GameState::playing && board.is_complete()) {
    board.update(std::min(board.get_rows() * board.get_columns(), board.get_mines() + mines_increment));
    round++;
    time += time_increment;
  }
}

void Game::on_refresh_event()
{
  if (state == GameState::playing && elapsed_time().count() >= time) {
    state = GameState::ended;
#if defined(__EMSCRIPTEN__)// use stderr channel to communicate score to JavaScript
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

Bitmap Game::render_board() const { return board.render(); }
void Game::on_key_up()
{
  if (state != GameState::ended) { board.on_key_up(); }
}
}// namespace minesweeper