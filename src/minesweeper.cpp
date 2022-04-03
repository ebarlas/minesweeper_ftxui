#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/screen/color.hpp"

enum class GameState { init, playing, ended };

class Game
{
  const int rows = 16;
  const int columns = 30;
  const int time_limit = 180;

  int hover_row = -1;
  int hover_col = -1;

  GameState state = GameState::init;
  int round = 1;
  std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> start_time{};

  [[nodiscard]] std::string round_text() const { return std::to_string(round); }

  static auto time_now()
  {
    return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
  }

  [[nodiscard]] auto elapsed_time() const
  {
    auto now = time_now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
    return elapsed.count();
  }

  [[nodiscard]] std::string time_text() const
  {
    if (state == GameState::init) { return std::to_string(time_limit); }
    if (state == GameState::ended) { return "0"; }
    return std::to_string(time_limit - elapsed_time());
  }

  void on_mouse_event(ftxui::Event e)
  {
    using namespace ftxui;

    int col = e.mouse().x - 1;
    int row = e.mouse().y - 1;

    if (e.mouse().button == Mouse::Button::Left && e.mouse().motion == Mouse::Motion::Released) {
      if (row >= 0 && row < rows && col >= 0 && col < columns) {
        if (state == GameState::init) {
          state = GameState::playing;
          start_time = time_now();
        }
      }
    }

    hover_col = col;
    hover_row = row;
  }

  void on_refresh_event()
  {
    if (state == GameState::playing && elapsed_time() >= time_limit) { state = GameState::ended; }
  }

  void on_new_game() { state = GameState::init; }

  [[nodiscard]] ftxui::Canvas render_board() const
  {
    using namespace ftxui;
    auto cvs = Canvas(columns * 2, rows * 4);
    for (int r = 0; r < rows; r++) {
      for (int c = 0; c < columns; c++) {
        int x = c * 2;
        int y = r * 4;
        if (r == hover_row && c == hover_col) {
          cvs.DrawText(x, y, " ", [](Pixel &p) {
            p.background_color = Color::GrayDark;
            p.foreground_color = Color::GrayDark;
          });
        } else {
          cvs.DrawText(x, y, " ", [](Pixel &p) {
            p.background_color = Color::GrayLight;
            p.foreground_color = Color::GrayLight;
          });
        }
      }
    }
    return cvs;
  }

public:
  Game(int rows, int columns, int time_limit)// NOLINT adjacent params of same type not problematic in practice
    : rows(rows), columns(columns), time_limit(time_limit)
  {}

  void run()
  {
    using namespace ftxui;

    auto board_renderer = Renderer([&] { return canvas(render_board()); });
    auto board_with_mouse = CatchEvent(board_renderer, [&](const Event &e) {
      if (e.is_mouse()) { on_mouse_event(e); }
      return false;
    });

    auto round_window = window(text("Round"), text("1"));
    auto time_window = window(text("Time"), text("180"));

    auto new_game_button = Button("New Game", [&] { on_new_game(); });
    auto reset_button = Button("Reset", [&] {});

    auto buttons = Container::Vertical({ new_game_button, reset_button });
    auto components = CatchEvent(Container::Horizontal({ board_with_mouse, buttons }), [&](const Event &) {
      on_refresh_event();
      return false;
    });

    auto game_renderer = Renderer(components, [&] {
      return hbox({ board_with_mouse->Render(),
               separator(),
               vbox({ window(text("Round"), text(round_text())),
                 window(text("Time"), text(time_text())),
                 buttons->Render() }) })
             | border;
    });

    auto screen = ScreenInteractive::FitComponent();
    std::atomic<bool> refresh_ui_continue = true;
    std::thread refresh_ui([&] {
      while (refresh_ui_continue) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1.0s / 10.0);// NOLINT magic numbers
        screen.PostEvent(ftxui::Event::Custom);
      }
    });
    screen.Loop(game_renderer);
    refresh_ui_continue = false;
    refresh_ui.join();
  }
};

int main()
{
  Game game{ 16, 30, 5 };// NOLINT constant seed parameters for game
  game.run();
  return 0;
}