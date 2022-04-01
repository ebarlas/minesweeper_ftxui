#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/screen/color.hpp"

int main()
{
  using namespace ftxui;

  const auto rows = 16;
  const auto columns = 30;

  auto board_renderer = Renderer([] {
    auto cvs = Canvas(columns * 2, rows * 4);
    for (int r = 0; r < rows; r++) {
      for (int c = 0; c < columns; c++) {
        cvs.DrawText(c * 2, r * 4, " ", [](Pixel &p) {
          p.background_color = Color::GrayLight;
          p.foreground_color = Color::GrayLight;
        });
      }
    }
    return canvas(std::move(cvs));
  });

  auto round_window = window(text("Round"), text("1"));
  auto time_window = window(text("Time"), text("180"));

  auto game_renderer = Renderer(board_renderer, [&] {
    return hbox({ board_renderer->Render(), separator(), vbox({ round_window, time_window }) }) | border;
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(game_renderer);
  return 0;
}