#ifndef _MINESWEEPER_LAUNCH_H_
#define _MINESWEEPER_LAUNCH_H_

#include "game.hpp"

namespace vas {
namespace mw {

enum option_t : u8
{
  easy,
  medium,
  hard,
  exit
};
constexpr static array<string_view, 4> option_str { "easy", "medium", "hard", "exit" };

class menu
{
private:
  Player window;
  DrawBoard board;
  array<typeSetter*, 4> options;
  struct miniState
  {
    bool leftdown { false };
    i32 lastx { 0u };
    i32 lasty { 0u };
    option_t inRect { 0u };
    bool shouldStart { false };
  } state {};

public:
  menu( const char* title, u32 width = 200, u32 height = 360 )
    : window { title, width, height }, board { window.CreateTexture(), width, height }
  {
    for ( int i = 0; auto& op : options ) {
      op = new typeSetter( string( option_str[i] ), board );
      op->background = toRGB( Color::not_visible );
      op->setRect( width / 2 - 75, 50 + i++ * 70, 150, 50 );
      op->config.draw_start_x = 10;
      op->config.draw_start_y = 10;
      op->LoadContent();
    }
    initEvent();
  }
  ~menu()
  {
    for ( auto& i : options )
      delete i;
  }
  void initEvent()
  {
    window.addEventhandle( SDL_MOUSEMOTION, [this]( const SDL_Event& e ) {
      state.lastx = e.motion.x;
      state.lasty = e.motion.y;
    } );
    window.addStatehandle(
      []() { /* always check */
             return true;
      },
      [this]() {
        for ( int i = 0; i < options.size(); i++ ) {
          if ( options[i]->posInRect( state.lastx, state.lasty ) ) {
            options[i]->background = toRGB( Color::mark_as_mine );
          } else {
            options[i]->background = toRGB( Color::not_visible );
          }
        }
      } );
    window.addEventhandle( SDL_MOUSEBUTTONDOWN, [this]( const SDL_Event& e ) {
      state.leftdown = true;
      state.lastx = e.button.x;
      state.lasty = e.button.y;
      for ( int i = 0; auto& op : options ) {
        if ( op->posInRect( state.lastx, state.lasty ) ) {
          state.inRect = option_t( i );
          op->background = toRGB( Color::mark_as_mine );
          break;
        }
        i++;
      }
    } );
    window.addEventhandle( SDL_MOUSEBUTTONUP, [this]( const SDL_Event& e ) {
      state.shouldStart = state.leftdown && std::abs( state.lastx - e.button.x ) < 10
                          && std::abs( state.lasty - e.button.y ) < 10
                          && options[state.inRect]->posInRect( e.button.x, e.button.y );
    } );
  }
  option_t getOption()
  {
    while ( true ) {
      if ( window.ShouldQuit() )
        return option_t::exit;
      options[0]->b.ClearBuffer();
      {
        for ( auto& op : options )
          op->DrawContent();
        window.HandleEvent();
        window.HandleState();
      }
      window.Render( options[0]->b );
      if ( state.shouldStart ) {
        return state.inRect;
      }
    }
    return option_t::exit;
  }
};

class launcher
{
public:
  static inline void Start(const char* title = "Minesweeper")
  {
    auto option { u8( 1919810 % 114514 ) };
    while ( option != option_t::exit ) {
      {
        // for destructing menu, scope it
        menu m { title };
        option = m.getOption();
      }
      switch ( option ) {
        case option_t::easy: {
          Game g( 8, 8, 10, 320, 320, title );
          g.MainLoop();
          break;
        }
        case option_t::medium: {
          Game g( 16, 16, 40, 480, 480, title );
          g.MainLoop();
          break;
        }
        case option_t::hard: {
          Game g( 16, 30, 99, ScreenWidth / 2 - 50, ScreenHeight / 2 - 50, title );
          g.MainLoop();
          break;
        }
        default:
          break;
      }
    }
  }
};

}
}

#endif