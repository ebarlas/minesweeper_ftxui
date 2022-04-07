#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "game.h"

int main()
{
  minesweeper::Game game{ 18, 30, 60, 15, 5, 10 };// NOLINT constant seed parameters for game

  using namespace ftxui;

  auto board_renderer = Renderer([&] { return canvas(game.render_board()); });
  auto board_with_mouse = CatchEvent(board_renderer, [&](Event e) {
    if (e.is_mouse()) {
      auto &mouse = e.mouse();
      auto row = mouse.y - 3;// subtract top title bar height
      auto col = mouse.x - 1;// subtract left border width
      game.on_mouse_event(row, col, mouse.button, mouse.motion);
    }
    return false;
  });

  auto new_game_button = Button("New Game", [&] { game.on_new_game(); });
  auto reset_button = Button("Reset", [&] { game.on_reset_game(); });

  auto buttons = Container::Vertical({ new_game_button, reset_button });
  auto components = CatchEvent(Container::Horizontal({ board_with_mouse, buttons }), [&](const Event &e) {
    if (e.is_character()) { game.on_key_up(); }
    game.on_refresh_event();
    return false;
  });

  auto game_renderer = Renderer(components, [&] {
    return vbox({ center(text("Minesweeper Marathon")) | flex,
             separator(),
             hbox({ board_with_mouse->Render(),
               separator(),
               vbox({ window(text("Round"), text(std::to_string(game.get_round()))),
                 window(text("Time"), text(std::to_string(game.get_time()))),
                 window(text("Mines"), text(std::to_string(game.get_mines()))),
                 buttons->Render() }) }) })
           | border;
  });

  auto screen = ScreenInteractive::FitComponent();
  std::atomic<bool> refresh_ui_continue = true;
  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1.0s / 10.0);// NOLINT magic numbers
      screen.PostEvent(Event::Custom);
    }
  });
  screen.Loop(game_renderer);
  refresh_ui_continue = false;
  refresh_ui.join();

  return 0;
}