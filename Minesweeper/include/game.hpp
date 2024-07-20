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
  Game( const char* title = "Minesweeper",
        u32 rows = 16,
        u32 cols = 16,
        u32 mines = 40,
        u32 w = 1000,
        u32 h = 1000 )
    : window( title, w, h )
    , base_board( window.CreateTexture(), w, h )
    , game_board( rows, cols, mines )
    , drawer( base_board, rows, cols, w, h )
  // , mouse_handle( base_board )
  {
    window.addhandle( SDL_MOUSEBUTTONDOWN, [this]( const SDL_Event& e ) { MouseDown( e ); } );
    window.addhandle( SDL_MOUSEBUTTONUP, [this]( const SDL_Event& e ) { MouseUp( e ); } );
  }
  ~Game() = default;
  void MainLoop()
  {
    bool gaming { true };
    const auto play_again { [] {
      std::cout << "Play again? (y/n): ";
      std::string ans;
      std::cin >> ans;
      return ans == "y";
    } };
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
          if ( play_again() )
            reset();
          else
            break;
        }
      }
    }
  }

private:
  void reset()
  {
    window.addhandle( SDL_MOUSEBUTTONDOWN, [this]( const SDL_Event& e ) { MouseDown( e ); } );
    window.addhandle( SDL_MOUSEBUTTONUP, [this]( const SDL_Event& e ) { MouseUp( e ); } );
    base_board.ClearBuffer();
    game_board.reset();
    window.ChangeTitle( "Minesweeper" );
  }
  struct mouse_state
  {
    bool leftdown { false };
    bool rightdown { false };
    int xfirst { 0 };
    int xcur { 0 };
    int yfirst { 0 };
    int ycur { 0 };
  } ms {};
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

    bool not_moved { std::abs( ms.xcur - ms.xfirst ) < 10 && std::abs( ms.ycur - ms.yfirst ) < 10 };
    if ( not_moved && type == SDL_BUTTON_LEFT ) {
      game_board.click( drawer.CalculatePos( ms.xcur, ms.ycur ) );
    } else if ( not_moved && type == SDL_BUTTON_RIGHT ) {
      auto [x, y] = drawer.CalculatePos( ms.xcur, ms.ycur );
      if ( game_board.getvisible()[x][y] ) {
        return;
      }
      drawer.mark_as_mine( x, y );
    }
    // prevent crash, clear all handles
    if ( game_board.HitMine() ) {
      window.clearStatehandle();
      window.removehandle( SDL_MOUSEBUTTONDOWN );
      window.removehandle( SDL_MOUSEBUTTONUP );
      window.ChangeTitle( "Lose" );
    } else if ( game_board.Success() ) {
      window.clearStatehandle();
      window.removehandle( SDL_MOUSEBUTTONDOWN );
      window.removehandle( SDL_MOUSEBUTTONUP );
      for ( auto it = game_board.getvisible().begin(); it != game_board.getvisible().end(); it++ ) {
        std::fill( it->begin(), it->end(), true );
      }
      window.ChangeTitle( "Win" );
    }
  }
  void MouseMove( const SDL_Event& e )
  {
    if ( ms.leftdown ) {
      ms.xcur = std::min( std::max( 0, e.button.x ), static_cast<int>( base_board.pic.width ) );
      ms.ycur = std::min( std::max( 0, e.button.y ), static_cast<int>( base_board.pic.height ) );
    }
  }
};

}
}