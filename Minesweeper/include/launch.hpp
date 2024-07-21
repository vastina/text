#ifndef _MINESWEEPER_LAUNCH_H_
#define _MINESWEEPER_LAUNCH_H_

#include "window.hpp"
#include "game.hpp"

#include <array>
#include <string_view>

namespace vas {
namespace mw {

class menu
{
private:
  typeSetter* options[4];
  struct miniState
  {
    bool leftdown { false };
    i32 lastx { 0u };
    i32 lasty { 0u };
    u8 inRect { 0u };
    bool shouldStart { false };
  } state {};
  constexpr static std::array<std::string_view, 4> option_str { "easy", "medium", "hard", "exit" };

public:
  menu( DrawBoard& board )
  {
    const u32 width = board.pic.width;
    const u32 height = board.pic.height;
    for ( int i = 0; auto& op : options ) {
      op = new typeSetter( std::string( option_str[i++] ), board );
      op->background = toRGB( Color::not_visible );
      op->setRect( width / 2 - 75, height / 4 + i * 70, 150, 50 );
      op->config.draw_start_x = 10;
      op->config.draw_start_y = 10;
      op->LoadContent();
    }
  }
  ~menu()
  {
    for ( auto& i : options )
      delete i;
  }
  void initEvent( Player& window )
  {
    window.addhandle( SDL_MOUSEMOTION, [this]( const SDL_Event& e ) {
      state.lastx = e.motion.x;
      state.lasty = e.motion.y;
    } );
    window.addStatehandle(
      []() { /* always check */
             return true;
      },
      [this]() {
        for ( int i = 0; i < 4; i++ ) {
          if ( options[i]->posInRect( state.lastx, state.lasty ) ) {
            options[i]->background = toRGB( Color::mark_as_mine );
          } else {
            options[i]->background = toRGB( Color::not_visible );
          }
        }
      } );
    window.addhandle( SDL_MOUSEBUTTONDOWN, [this]( const SDL_Event& e ) {
      state.leftdown = true;
      state.lastx = e.button.x;
      state.lasty = e.button.y;
      for ( int i = 0; auto& op : options ) {
        if ( op->posInRect( state.lastx, state.lasty ) ) {
          state.inRect = i;
          op->background = toRGB( Color::mark_as_mine );
          break;
        }
        i++;
      }
    } );
    window.addhandle( SDL_MOUSEBUTTONUP, [this]( const SDL_Event& e ) {
      state.shouldStart = state.leftdown && std::abs( state.lastx - e.button.x ) < 10
                          && std::abs( state.lasty - e.button.y ) < 10
                          && options[state.inRect]->posInRect( e.button.x, e.button.y );
    } );
  }
  u8 getOption( Player& window )
  {
    while ( true ) {
      if ( window.ShouldQuit() )
        return 3;
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
    return 0xff;
  }
};

class launcher
{
public:
  static inline void Start()
  {
    constexpr u32 ww { 300u };
    constexpr u32 hh { 600u };
    while ( true ) {
      u8 option;
      {
        Player wd( "Minesweeper", ww, hh );
        DrawBoard b( wd.CreateTexture(), ww, hh );
        b.config.background = { 0x22, 0x22, 0x22 };
        menu m( b );
        m.initEvent( wd );
        option = m.getOption( wd );
      }
      {
        if ( option == 3 )
          break;
        if ( option == 0 ) {
          Game g( 8, 8, 10, 500, 500 );
          g.MainLoop();
        } else if ( option == 1 ) {
          Game g( 16, 16, 40, 900, 900 );
          g.MainLoop();
        } else if ( option == 2 ) {
          Game g {};
          g.MainLoop();
        }
      }
    }
  }
};

}
}

#endif