#ifndef _LIGHTSOFF_GAME_H_
#define _LIGHTSOFF_GAME_H_

#include "window.hpp"

#include "board.hpp"
using namespace std::chrono_literals;

namespace vas {
namespace lo {

constexpr static std::vector<Board::borad_t> start_puzzles()
{
  return { { off, off, off, off, off, off, off, on,  off, off, off, on, on,
             on,  off, off, off, on,  off, off, off, off, off, off, off },
           { off, off, off, on,  on,  off, off, off, off, on,  off, off, off,
             off, off, off, off, off, off, off, off, off, off, off, off },
           { on,  on,  off, on, on,  off, on,  off, on, off, off, off, off,
             off, off, off, on, off, on,  off, on,  on, off, on,  on } };
}

class Game
{
  Player window;
  DrawBoard base_board;

  Board game_board;
  Drawer drawer;

public:
  Game( u32 ww = 500, u32 hh = 500 )
    : window( "LightsOff", ww, hh )
    , base_board( window.CreateTexture(), ww, hh )
    , game_board( start_puzzles() )
    , drawer( base_board )
  {
    window.addEventhandle( SDL_MOUSEBUTTONDOWN, [this]( const SDL_Event& e ) { MouseDown( e ); } );
    window.addEventhandle( SDL_MOUSEBUTTONUP, [this]( const SDL_Event& e ) { MouseUp( e ); } );
  }
  ~Game() = default;
  void MainLoop()
  {
    while ( not window.ShouldQuit() ) {
      base_board.ClearBuffer();
      {
        window.HandleEvent();
        window.HandleState();
        drawer.DrawContent( game_board );
      }
      window.Render( base_board );
      freshGameState();
    }
  }

private:
  mousestate ms {};
  void MouseDown( const SDL_Event& e )
  {
    ms.leftdown = true;
    ms.xfirst = e.button.x;
    ms.yfirst = e.button.y;
    ms.xcur = ms.xfirst;
    ms.ycur = ms.yfirst;
  }
  void MouseUp( const SDL_Event& e )
  {
    auto type = e.button.button;
    if ( ms.leftdown && type == SDL_BUTTON_LEFT ) {
      ms.leftdown = false;
      ms.xcur = std::min( std::max( 0, e.button.x ), static_cast<int>( base_board.pic.width ) );
      ms.ycur = std::min( std::max( 0, e.button.y ), static_cast<int>( base_board.pic.height ) );
      if ( std::abs( ms.xcur - ms.xfirst ) < 15 && std::abs( ms.ycur - ms.yfirst ) < 15 ) {
        auto p { drawer.CalculatePos( ms.xcur, ms.ycur ) };
        game_board.Click( p );
        // if(game_board.Success())
        //   game_board.NextPuzzle();
      }
    }
  }
  void freshGameState()
  {
    if ( game_board.Success() ) {
      game_board.NextPuzzle();
    }
  }
};

}
}
#endif