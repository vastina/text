#ifndef _DRAW_BOARD_H_
#define _DRAW_BOARD_H_

#include "png.hpp"

namespace vas {

struct DrawBoard
{
  vas::png::png pic;
  std::vector<bool> drawable;
  DrawBoard( const std::string& filename, u32 width, u32 height )
    : pic { filename, width, height }, drawable( width * height, true )
  {}
  void DrawChar( const u32 width, const u32 height, const u8* buffer, const u32 xoffst, const u32 yoffst )
  {
    for ( u32 y = 0; y < height; y++ ) {
      for ( u32 x = 0; x < width; x++ ) {
        auto val { buffer[y * width + x] };
        //      if(val > 16)
        setIndex( x + xoffst, y + yoffst, { val, val, val } );
      }
    }
  }
  void setIndex( u32 x, u32 y, vas::png::RGB color )
  {
    if ( drawable[x + y * pic.width] ) {
      pic.setIndex( x, y, color );
      drawable[x + y * pic.width] = false;
    }
  }
  void setIndex_Coverable( u32 x, u32 y, vas::png::RGB color )
  {
    if ( drawable[x + y * pic.width] ) {
      pic.setIndex( x, y, color );
    }
  }
  void DraeRect( int x1, int y1, int x2, int y2, vas::png::RGB color )
  {
    const int dx = x1 > x2 ? -1 : 1;
    const int dy = y1 > y2 ? -1 : 1;
    for ( int i = x1; i != x2; i += dx ) {
      for ( int j = y1; j != y2; j += dy ) {
        setIndex( i, j, color );
      }
    }
  }
  void DraeRect_Coverable( int x1, int y1, int x2, int y2, vas::png::RGB color )
  {
    const int dx = x1 > x2 ? -1 : 1;
    const int dy = y1 > y2 ? -1 : 1;
    for ( int i = x1; i != x2; i += dx ) {
      for ( int j = y1; j != y2; j += dy ) {
        setIndex_Coverable( i, j, color );
      }
    }
  }
  void Drawrect_force( int x1, int y1, int x2, int y2, vas::png::RGB color )
  {
    const int dx = x1 > x2 ? -1 : 1;
    const int dy = y1 > y2 ? -1 : 1;
    for ( int i = x1; i != x2; i += dx ) {
      for ( int j = y1; j != y2; j += dy ) {
        pic.setIndex( i, j, color );
      }
    }
  }
  void Clear( int x1, int y1, int x2, int y2 ) { DraeRect( x1, y1, x2, y2, { 0, 0, 0 } ); }
  void Clear_Coverable( int x1, int y1, int x2, int y2 ) { DraeRect_Coverable( x1, y1, x2, y2, { 0, 0, 0 } ); }
  void Clear_force( int x1, int y1, int x2, int y2 ) { Drawrect_force( x1, y1, x2, y2, { 0, 0, 0 } ); }
};

}
#endif