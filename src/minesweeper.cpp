#include <array>
#include <random>
#include <numeric>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/screen/color.hpp"

static const int ROWS = 18;
static const int COLUMNS = 30;

struct Cell
{
  int row;
  int col;
  bool mine;
  bool flagged;
  bool revealed;
  int adjacentMines;
};

class Board
{
  const std::array<ftxui::Color, 9> COLORS{ ftxui::Color::Black,
    ftxui::Color::Blue,
    ftxui::Color::Green,
    ftxui::Color::Red,
    ftxui::Color::DarkBlue,
    ftxui::Color::DarkRed,
    ftxui::Color::SeaGreen1,
    ftxui::Color::Black,
    ftxui::Color::Black };

  std::array<Cell, static_cast<size_t>(ROWS *COLUMNS)> cells{};

  int mines;

  int hover_row = -1;
  int hover_col = -1;

  void reset()
  {
    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLUMNS; c++) {
        auto &cell = at(r, c);
        cell.row = r;
        cell.col = c;
        cell.mine = false;
        cell.flagged = false;
        cell.revealed = false;
        cell.adjacentMines = 0;
      }
    }
    assign_mines();
    assign_adjacent_mines();
  }

  void for_each_adjacent(int r, int c, const std::function<void(Cell &cell)> &fn)
  {
    for (int ri = r - 1; ri <= r + 1; ri++) {
      for (int ci = c - 1; ci <= c + 1; ci++) {
        if (ri >= 0 && ri < ROWS && ci >= 0 && ci < COLUMNS) {
          if (ci != c || ri != r) { fn(at(ri, ci)); }
        }
      }
    }
  }

  Cell &at(int row, int col) { return cells.at(row * COLUMNS + col); }

  void assign_mines()
  {
    auto now = std::chrono::system_clock::now();
    auto second_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::mt19937 mt{ static_cast<unsigned int>(second_since_epoch) };
    std::uniform_int_distribution dist{ 0, ROWS * COLUMNS - 1 };// random values over closed (inclusive) range
    int remaining = mines;
    while (remaining > 0) {
      auto next = dist(mt);
      auto &cell = cells.at(next);
      if (!cell.mine) {
        cell.mine = true;
        remaining--;
      }
    }
  }

  void assign_adjacent_mines()
  {
    for (auto &target : cells) {
      for_each_adjacent(target.row, target.col, [&target](const Cell &adj) {
        if (adj.mine) { target.adjacentMines++; }
      });
    }
  }

  int count_adjacent_flags(int row, int col)
  {
    int count = 0;
    for_each_adjacent(row, col, [&count](const Cell &cell) {
      if (cell.flagged) { count++; }
    });
    return count;
  }

  void reveal_neighbors(int row, int col)
  {
    for_each_adjacent(row, col, [this](const Cell &cell) {
      if (!cell.flagged && !cell.revealed) { reveal(cell.row, cell.col); }
    });
  }

  void reveal(int row, int col)
  {
    auto &cell = at(row, col);
    cell.revealed = true;
    if (!cell.mine && cell.adjacentMines == 0) { reveal_neighbors(row, col); }
  }

  static void draw(ftxui::Canvas &canvas, int row, int col, const std::string &value, ftxui::Color fg, ftxui::Color bg)
  {
    canvas.DrawText(col * 2, row * 4, value, [fg, bg](ftxui::Pixel &p) {
      p.foreground_color = fg;
      p.background_color = bg;
      p.bold = true;
    });
  }

  void render(ftxui::Canvas &canvas, int row, int col)
  {
    using namespace ftxui;
    auto &cell = at(row, col);
    auto is_sel = row == hover_row && col == hover_col;
    if (!cell.revealed && !cell.flagged) {
      draw(canvas, row, col, " ", Color::GrayLight, is_sel ? Color::GrayDark : Color::GrayLight);
    } else if (!cell.revealed && cell.flagged) {
      draw(canvas, row, col, "*", Color::Red, is_sel ? Color::GrayDark : Color::GrayLight);
    } else if (cell.mine) {
      draw(canvas, row, col, " ", Color::Red, is_sel ? Color::GrayDark : Color::Red);
    } else if (cell.adjacentMines == 0) {
      draw(canvas, row, col, " ", Color::White, is_sel ? Color::GrayDark : Color::White);
    } else {
      draw(canvas,
        row,
        col,
        std::to_string(cell.adjacentMines),
        COLORS.at(cell.adjacentMines),
        is_sel ? Color::GrayDark : Color::White);
    }
  }

public:
  explicit Board(int mines) : mines(mines) { reset(); }

  [[nodiscard]] ftxui::Canvas render()
  {
    using namespace ftxui;
    auto cvs = Canvas(COLUMNS * 2, ROWS * 4);
    for (auto &cell : cells) { render(cvs, cell.row, cell.col); }
    return cvs;
  }

  void on_left_click(int row, int col)
  {
    if (is_alive()) {
      const auto &cell = at(row, col);
      if (cell.revealed && cell.adjacentMines == count_adjacent_flags(row, col)) {
        reveal_neighbors(row, col);
      } else if (!cell.flagged) {
        reveal(row, col);
      }
    }
  }

  void on_right_click(int row, int col)
  {
    if (is_alive()) {
      auto &cell = at(row, col);
      if (!cell.revealed) { cell.flagged = !cell.flagged; }
    }
  }

  void on_hover(int row, int col)// NOLINT adjacent int params
  {
    hover_row = row;
    hover_col = col;
  }

  void restore()
  {
    for (auto &cell : cells) {
      cell.flagged = false;
      cell.revealed = false;
    }
  }

  void update(int mines_update)
  {
    mines = mines_update;
    reset();
  }

  [[nodiscard]] int get_mines() const { return mines; }

  bool is_alive()
  {
    auto fn = [](bool alive, const Cell &c) { return alive && !(c.revealed && c.mine); };
    return std::accumulate(cells.cbegin(), cells.cend(), true, fn);
  }

  bool is_complete()
  {
    auto fn = [](int sum, const Cell &c) { return c.revealed ? sum + 1 : sum; };
    auto revealed = std::accumulate(cells.cbegin(), cells.cend(), 0, fn);
    return revealed == cells.size() - mines;
  }
};

enum class GameState { init, playing, ended };

class Game
{
  const int time_limit;
  const int mines_init;
  const int mines_increment;

  Board board;

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

  [[nodiscard]] std::string mines_text() const { return std::to_string(board.get_mines()); }

  void on_mouse_event(ftxui::Event e)
  {
    using namespace ftxui;

    int col = e.mouse().x - 1;
    int row = e.mouse().y - 1;

    board.on_hover(row, col);

    if (state != GameState::ended) {
      if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS) {
        if (e.mouse().motion == Mouse::Released) {
          if (e.mouse().button == Mouse::Button::Left) {
            if (state == GameState::init) {
              state = GameState::playing;
              start_time = time_now();
            }
            board.on_left_click(row, col);
          } else if (e.mouse().button == Mouse::Button::Right) {
            board.on_right_click(row, col);
          }
        }
      }
    }

    if (board.is_complete()) {
      board.update(board.get_mines() + mines_increment);
      round++;
    }
  }

  void on_refresh_event()
  {
    if (state == GameState::playing && elapsed_time() >= time_limit) { state = GameState::ended; }
  }

  void on_new_game()
  {
    state = GameState::init;
    round = 1;
    board.update(mines_init);
  }

  void on_reset_game()
  {
    if (state == GameState::playing) { board.restore(); }
  }

public:
  Game(int time_limit, int mines_increment, int mines_init)// NOLINT adjacent int params
    : time_limit(time_limit), mines_init(mines_init), mines_increment(mines_increment), board(mines_init)
  {}

  void run()
  {
    using namespace ftxui;

    auto board_renderer = Renderer([&] { return canvas(board.render()); });
    auto board_with_mouse = CatchEvent(board_renderer, [&](const Event &e) {
      if (e.is_mouse()) { on_mouse_event(e); }
      return false;
    });

    auto new_game_button = Button("New Game", [&] { on_new_game(); });
    auto reset_button = Button("Reset", [&] { on_reset_game(); });

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
                 window(text("Mines"), text(mines_text())),
                 buttons->Render() }) })
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
  }
};

int main()
{
  Game game{ 180, 5, 10 };// NOLINT constant seed parameters for game
  game.run();
  return 0;
}