#include "text.hpp"

#include <random>
#include <functional>

namespace vas {
namespace mw {

class Board
{
private:
  u32 rows;
  u32 cols;
  u32 mines;
  std::vector<std::vector<char>> board;
  std::vector<std::vector<bool>> visible;

  bool hit_mine { false };
  u32 units_left;

public:
  Board( u32 r = 8, u32 c = 8, u32 m = 10 )
    : rows( r ), cols( c ), mines( m ), units_left( r * c - m )
  {
    visible.resize( rows );
    for ( auto& row : visible ) {
      row.resize( cols, false );
    }
    board.resize( rows );
    for ( auto& row : board ) {
      row.resize( cols, 0 );
    }
    GenerateBoard();
  }
  GSetter( board );
  GSetter( visible ) u32 getcols() const { return cols; }

  void reset()
  {
    for ( auto it = visible.begin(); it != visible.end(); it++ ) {
      std::fill( it->begin(), it->end(), false );
    }
    for ( auto it = board.begin(); it != board.end(); it++ ) {
      std::fill( it->begin(), it->end(), 0 );
    }
    GenerateBoard();
    hit_mine = false;
    units_left = rows * cols - mines;
  }

private:
  void GenerateBoard()
  {
    GenerateMines();
    GenerateNum();
  }
  void GenerateMines()
  {
    std::mt19937 gen { std::random_device {}() };
    std::uniform_int_distribution<u32> dist( 0, rows * cols - 1 );
    auto rd { std::bind( dist, gen ) };

    for ( u32 i { 0u }; i < mines; i++ ) {
      do {
        auto row { rd() % rows };
        auto col { rd() % cols };
        if ( board[row][col] < '9' ) {
          board[row][col] = '9';
          break;
        }
      } while ( true );
    }
  }
  u8 getMinesNum( u32 row, u32 col )
  {
    u8 num { 0 };
    for ( int i = -1; i <= 1; i++ ) {
      for ( int j = -1; j <= 1; j++ ) {
        if ( i == 0 && j == 0 )
          continue;
        if ( row + i < 0 || row + i >= rows )
          continue;
        if ( col + j < 0 || col + j >= cols )
          continue;
        if ( board[row + i][col + j] == '9' )
          num++;
      }
    }
    return num;
  }
  void GenerateNum()
  {
    for ( u32 i { 0u }; i < rows; i++ ) {
      for ( u32 j { 0u }; j < cols; j++ ) {
        if ( board[i][j] >= '9' )
          continue;
        board[i][j] = getMinesNum( i, j ) + '0';
      }
    }
  }

public:
  void click( pos p )
  {
    auto [x, y] { p };
    if ( x < 0 || x >= rows || y < 0 || y >= cols )
      return;
    if ( visible[x][y] )
      return;
    visible[x][y] = true;
    // while HitMine() is judged before Success(), the following line is unnecessary
    // if(board[x][y] <= '9')
    units_left--;
    if ( board[x][y] == '0' ) {
      click( { x - 1, y - 1 } );
      click( { x - 1, y } );
      click( { x - 1, y + 1 } );
      click( { x, y - 1 } );
      click( { x, y + 1 } );
      click( { x + 1, y - 1 } );
      click( { x + 1, y } );
      click( { x + 1, y + 1 } );
    } else if ( board[x][y] == '9' ) {
      hit_mine = true;
    }
  }
  bool HitMine() const { return hit_mine; }
  bool Success() const { return units_left == 0; }
};

enum Color : u32
{
  not_visible = 0x708090,
  visible = 0x66cdaa,
  mark_as_mine = 0xcc2020,
  hit_mine = 0xff1010
};

static inline RGB toRGB( Color color )
{
  u32 c = color;
  return { u8( ( c >> 16 ) & 0xff ), u8( ( c >> 8 ) & 0xff ), u8( c & 0xff ) };
}

class Drawer
{
  u32 rows;
  u32 cols;
  std::vector<std::vector<typeSetter*>> drawer;

  u32 width;
  u32 height;
  u32 gap { 5 };

public:
  Drawer( DrawBoard& b, u32 r, u32 c, u32 window_width, u32 window_height )
    : rows( r ), cols( c ), width( window_width ), height( window_height )
  {
    drawer.resize( rows );
    for ( auto& row : drawer )
      row.resize( cols );
    const u32 unit_width { width / cols };
    const u32 unit_height { height / rows };
    for ( u32 i { 0u }; auto& row : drawer ) {
      for ( u32 j { 0u }; auto& ts : row ) {
        ts = new typeSetter( "", b );
        ts->config.draw_start_x = unit_width / 3;
        ts->config.draw_start_y = unit_height / 3;
        ts->setRect(
          i * unit_width + gap, j * unit_height + gap, unit_width - gap, unit_height - gap );
        j++;
      }
      i++;
    }
  }
  ~Drawer()
  {
    for ( auto& row : drawer ) {
      for ( auto& ts : row ) {
        delete ts;
      }
    }
  }
  void DrawContent( const Board& b )
  {
    const auto& board { b.getboard() };
    const auto& visible { b.getvisible() };
    for ( u32 i { 0u }; i < rows; i++ ) {
      for ( u32 j { 0u }; j < cols; j++ ) {
        if ( visible[i][j] ) {
          if ( board[i][j] >= '9' )
            drawer[i][j]->background = toRGB( Color::hit_mine );
          else
            drawer[i][j]->background = toRGB( Color::visible );
        } else {
          drawer[i][j]->background = toRGB( Color::not_visible );
        }
        drawer[i][j]->ChangeContent( [&]( auto& content ) {
          content = Text::utf8_to_utf32( { visible[i][j] ? board[i][j] : ' ' } );
        } );
        drawer[i][j]->DrawContent();
      }
    }
  }
  pos CalculatePos( u32 x, u32 y ) const
  {
    const u32 unit_width { width / cols };
    const u32 unit_height { height / rows };
    // if( x < 0 || x >= width || y < 0 || y >= height ) return {0, 0};
    return { x / unit_width, y / unit_height };
  }
  void mark_as_mine( u32 i, u32 j )
  {
    if ( i >= rows || j >= cols )
      return;
    if ( drawer[i][j]->background != toRGB( Color::mark_as_mine ) )
      drawer[i][j]->background = toRGB( Color::mark_as_mine );
    else
      drawer[i][j]->background = toRGB( Color::not_visible );
  }
};

}
}