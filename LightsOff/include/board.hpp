#ifndef _LIGHTSOFF_BOARD_H_
#define _LIGHTSOFF_BOARD_H_

#include <random>
#include <functional>

#include "text.hpp"

namespace vas {
namespace lo {

enum state : u8
{
  off,
  on
};

enum Color : u32
{
  off_color = 0x000000,
  on_color = 0x3030dd,
};

constexpr static u32 rows { 5u };
constexpr static u32 cols { 5u };

class Board
{
public:
  using borad_t = array<array<state, cols>, rows>;

private:
  borad_t board;
  u32 puzzleIndex { 0u };
  std::vector<borad_t> puzzles;
  u32 ClickTimeThisPuzzle { 0u };

public:
  GSetter( board );
  u32 getClickTimeThisPuzzle() const { return ClickTimeThisPuzzle; }
  Board( int puzzleNum ) : puzzles( GeneratePuzzles( puzzleNum ) ) { board = puzzles[0]; }
  Board( const std::vector<borad_t>& p ) : puzzles( p )
  {
    if ( p.empty() ) {
      puzzles = GeneratePuzzles();
    }
    board = puzzles[0];
  }
  static inline void Click( borad_t& board, pos p )
  {
    const auto ChangeState { [&board]( u32 x, u32 y ) {
      if ( x >= cols || y >= rows ) {
        return;
      }
      board[y][x] = board[y][x] == state::on ? state::off : state::on;
    } };
    const auto x = p.x;
    const auto y = p.y;
    ChangeState( x, y );
    ChangeState( x, y - 1 );
    ChangeState( x, y + 1 );
    ChangeState( x - 1, y );
    ChangeState( x + 1, y );
  }
  void Click( pos p )
  {
    ClickTimeThisPuzzle++;
    Click( board, p );
  }
  static inline std::vector<borad_t> GeneratePuzzles( int n = 10 )
  {
    static std::mt19937 gen { std::random_device {}() };
    static std::uniform_int_distribution<int> dist( 0, 4 );
    static auto rd { std::bind( dist, gen ) };

    std::vector<borad_t> puzzles;
    for ( int i = 0; i < n; i++ ) {
      borad_t puzzle {};
      int step = rd() * 5 + 1;
      for ( int k = 0; k < step; k++ ) {
        int x = rd();
        int y = rd();
        Click( puzzle, { x, y } );
      }
      puzzles.push_back( puzzle );
    }
    return puzzles;
  }
  void NextPuzzle()
  {
    if ( ( ++puzzleIndex ) >= puzzles.size() ) {
      puzzleIndex = 0;
      puzzles = GeneratePuzzles();
    }
    board = puzzles[puzzleIndex];
  }
  bool Success()
  {
    for ( int i = 0; i < rows; i++ ) {
      for ( int j = 0; j < cols; j++ ) {
        if ( board[i][j] == state::on ) {
          return false;
        }
      }
    }
    ClickTimeThisPuzzle = 0;
    return true;
  }
};

class Drawer
{
  array<array<typeSetter*, cols>, rows> drawer;
  typeSetter* tryTimes;
  u32 width;
  u32 height;
  u32 gap { 5 };

  constexpr static u32 top_height { 50u };

public:
  Drawer( DrawBoard& b ) : width( b.pic.width ), height( b.pic.height )
  {
    tryTimes = new typeSetter { "", b };
    tryTimes->setRect( 10, 10, 70, 30 );
    tryTimes->config.draw_start_x += 5;
    tryTimes->config.draw_start_y += 5;
    const u32 unit_width { width / cols };
    const u32 unit_height { ( height - top_height ) / rows };
    for ( int i = 0; i < rows; i++ ) {
      for ( int j = 0; j < cols; j++ ) {
        drawer[i][j] = new typeSetter { "", b };
        drawer[i][j]->setRect( j * unit_width + gap,
                               top_height + i * unit_height + gap,
                               unit_width - gap,
                               unit_height - gap );
      }
    }
  }
  ~Drawer()
  {
    for ( int i = 0; i < rows; i++ ) {
      for ( int j = 0; j < cols; j++ ) {
        delete drawer[i][j];
      }
    }
  }
  pos CalculatePos( u32 x, u32 y ) const
  {
    return { x / ( width / cols ), ( y - top_height ) / ( ( height - top_height ) / rows ) };
  }
  void DrawContent( const Board& b )
  {
    tryTimes->ChangeContent( [&b]( std::vector<u32>& s ) {
      s = Text::utf8_to_utf32( std::to_string( b.getClickTimeThisPuzzle() ) );
    } );
    tryTimes->DrawContent();
    const auto& board { b.getboard() };
    for ( int i = 0; i < rows; i++ ) {
      for ( int j = 0; j < cols; j++ ) {
        drawer[i][j]->background
          = board[i][j] == state::on ? toRGB( on_color ) : toRGB( off_color );
        drawer[i][j]->DrawContent();
      }
    }
  }
};

}
}

#endif