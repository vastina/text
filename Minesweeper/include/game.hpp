#include "window.hpp"

#include "board.hpp"

namespace vas { namespace mw {

class Game
{
private:
  Player window;
  DrawBoard base_board;

  Board game_board;
  Drawer drawer;
public:
  Game( const char* title = "Minesweeper", u32 rows = 8, u32 cols = 8, u32 mines = 10, u32 w = 1000, u32 h = 1000 )
    : window( title, w, h )
    , base_board( window.CreateTexture(), w, h )
    , game_board( rows, cols, mines )
    , drawer( base_board, rows, cols, w, h )
    // , mouse_handle( base_board )
  {
    window.addhandle(SDL_MOUSEBUTTONDOWN, [this](const SDL_Event& e){MouseDown(e);});
    window.addhandle(SDL_MOUSEBUTTONUP, [this](const SDL_Event& e){MouseUp(e);});
  }
  ~Game() = default;
  void MainLoop()
  {
    while ( !window.ShouldQuit() ) {
      base_board.ClearBuffer();
      window.HandleEvent();
      window.HandleState();
      drawer.DrawContent(game_board);
      window.Render(base_board);
    }
  }
private:
  struct mouse_state {
    bool down { false };
    int xfirst { 0 };
    int xcur { 0 };
    int yfirst { 0 };
    int ycur { 0 };
  } ms {};
  void MouseDown(const SDL_Event& e) {
    ms.down = true;
    ms.xfirst = e.button.x;
    ms.yfirst = e.button.y;
    ms.xcur = ms.xfirst;
    ms.ycur = ms.yfirst;
  }
  void MouseUp(const SDL_Event& e) {
    if( !ms.down ) return;
    ms.down = false;
    ms.xcur = std::min(std::max(0, e.button.x), static_cast<int>(base_board.pic.width));
    ms.ycur = std::min(std::max(0, e.button.y), static_cast<int>(base_board.pic.height));

    if ( ms.xfirst == ms.xcur && ms.yfirst == ms.ycur ) {
      game_board.click(drawer.CalculatePos(ms.xcur, ms.ycur));
    }
  }
  void MouseMove(const SDL_Event& e) {
    if( ms.down ) {
      ms.xcur = std::min(std::max(0, e.button.x), static_cast<int>(base_board.pic.width));
      ms.ycur = std::min(std::max(0, e.button.y), static_cast<int>(base_board.pic.height));
    }
  }
};

}}