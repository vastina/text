#ifndef _MINESWEEPER_GAME_H_
#define _MINESWEEPER_GAME_H_

#include "window.hpp"

#include "board.hpp"
using namespace std::chrono_literals;

namespace vas {
namespace mw {

class Game
{
private:
  Player window;
  DrawBoard base_board;

  Board game_board;
  Drawer drawer;

public:
  Game( u32 rows = 16,
        u32 cols = 30,
        u32 mines = 99,
        u32 w = 1200,
        u32 h = 600,
        const char* title = "Minesweeper" )
    : window( title, w, h )
    , base_board( window.CreateTexture(), w, h )
    , game_board( rows, cols, mines )
    , drawer( base_board, game_board )
  // , mouse_handle( base_board )
  {
    window.addEventhandle( SDL_MOUSEBUTTONDOWN, [this]( const SDL_Event& e ) { MouseDown( e ); } );
    window.addEventhandle( SDL_MOUSEBUTTONUP, [this]( const SDL_Event& e ) { MouseUp( e ); } );
  }
  ~Game() = default;
  void MainLoop()
  {
    bool gaming { true };
    // const auto play_again { [] {
    //   std::cout << "Play again? (y/n): ";
    //   string ans;
    //   std::cin >> ans;
    //   return ans == "y";
    // } };
    // auto start { std::chrono::system_clock::now() };
    // u32 frames { 0u };
    while ( !window.ShouldQuit() ) {
      gaming = !( game_board.HitMine() || game_board.Success() );
      base_board.ClearBuffer();
      {
        window.HandleEvent();
        window.HandleState();
        drawer.DrawContent( game_board );
      }
      window.Render( base_board );
      // {
      //   auto end { std::chrono::system_clock::now() };
      //   if ( end - start >= 1s ) {
      //     std::cout << "FPS: " << frames << '\n';
      //     frames = 0;
      //     start = end;
      //   }
      //   frames++;
      // }
      {
        if ( not gaming ) {
          // if ( play_again() )
          //   reset();
          // else
          // break;
          // prevent crash, clear all handles
          window.clearStatehandle();
          window.ClearEventhandle();
        }
      }
    }
  }

private:
  void reset()
  {
    window.addEventhandle( SDL_MOUSEBUTTONDOWN, [this]( const SDL_Event& e ) { MouseDown( e ); } );
    window.addEventhandle( SDL_MOUSEBUTTONUP, [this]( const SDL_Event& e ) { MouseUp( e ); } );
    base_board.ClearBuffer();
    game_board.reset();
    drawer.reset();
    window.ChangeTitle( "Minesweeper" );
  }
  mousestate ms {};
  void MouseDown( const SDL_Event& e )
  {
    // if(e.button.button == SDL_BUTTON_RIGHT) {
    //   ms.rightdown = true;
    // } else if(e.button.button == SDL_BUTTON_LEFT) {
    //   ms.leftdown = true;
    // } else {
    //   return;
    // }
    ms.xfirst = e.button.x;
    ms.yfirst = e.button.y;
    ms.xcur = ms.xfirst;
    ms.ycur = ms.yfirst;
  }
  void MouseUp( const SDL_Event& e )
  {
    auto type = e.button.button;
    // if(type == SDL_BUTTON_RIGHT) {
    //   ms.rightdown = false;
    // } else if(type == SDL_BUTTON_LEFT) {
    //   ms.leftdown = false;
    // } else {
    //   return;
    // }
    ms.xcur = std::min( std::max( 0, e.button.x ), static_cast<int>( base_board.pic.width ) );
    ms.ycur = std::min( std::max( 0, e.button.y ), static_cast<int>( base_board.pic.height ) );
    if ( std::abs( ms.xcur - ms.xfirst ) < 10 && std::abs( ms.ycur - ms.yfirst ) < 10 ) {
      auto p { drawer.CalculatePos( ms.xcur, ms.ycur ) };
      if ( type == SDL_BUTTON_LEFT ) {
        game_board.click( p );
      } else if ( type == SDL_BUTTON_RIGHT ) {
        if ( game_board.getvisible()[p.x][p.y] )
          return;
        drawer.mark_as_mine( p.x, p.y );
      }
      if ( game_board.HitMine() ) {
        window.ChangeTitle( "Lose" );
      } else if ( game_board.Success() ) {
        auto& visible { game_board.getvisible() };
        for ( auto it = visible.begin(); it != visible.end(); it++ ) {
          std::fill( it->begin(), it->end(), true );
        }
        window.ChangeTitle( "Win" );
      }
    }
  }
};

}
}

#endif