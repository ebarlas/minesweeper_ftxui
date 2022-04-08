#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include "game.h"

ftxui::Color map_color(minesweeper::Color color)
{
  if (color == minesweeper::Color::red) { return ftxui::Color::Red; }
  if (color == minesweeper::Color::blue) { return ftxui::Color::Blue; }
  if (color == minesweeper::Color::green) { return ftxui::Color::Green; }
  if (color == minesweeper::Color::dark_blue) { return ftxui::Color::DarkBlue; }
  if (color == minesweeper::Color::dark_red) { return ftxui::Color::DarkRed; }
  if (color == minesweeper::Color::sea_green) { return ftxui::Color::SeaGreen1; }
  if (color == minesweeper::Color::black) { return ftxui::Color::Black; }
  if (color == minesweeper::Color::light_gray) { return ftxui::Color::GrayLight; }
  if (color == minesweeper::Color::dark_gray) { return ftxui::Color::GrayDark; }
  return ftxui::Color::White;
}

ftxui::Canvas bitmap_to_canvas(const minesweeper::Bitmap &bitmap)
{
  auto canvas = ftxui::Canvas(bitmap.get_columns() * 2, bitmap.get_rows() * 4);
  for (int r = 0; r < bitmap.get_rows(); r++) {
    for (int c = 0; c < bitmap.get_columns(); c++) {
      auto pixel = bitmap.get(r, c);
      canvas.DrawText(c * 2, r * 4, std::string{ 1, pixel.value }, [&pixel](ftxui::Pixel &p) {
        p.foreground_color = map_color(pixel.foreground);
        p.background_color = map_color(pixel.background);
        p.bold = true;
      });
    }
  }
  return canvas;
}

int main()
{
  minesweeper::Game game{ 18, 30, 30, 20, 10, 1 };// NOLINT constant seed parameters for game

  using namespace ftxui;

  auto board_renderer = Renderer([&] { return canvas(bitmap_to_canvas(game.render_board())); });
  auto board_with_mouse = CatchEvent(board_renderer, [&](Event e) {
    if (e.is_mouse()) {
      auto &mouse = e.mouse();
      auto row = mouse.y - 3;// subtract top title bar height
      auto col = mouse.x - 1;// subtract left border width
      game.on_mouse_event(
        row, col, mouse.button == Mouse::Left, mouse.button == Mouse::Right, mouse.motion == Mouse::Motion::Released);
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