#ifndef _DRAW_BOARD_H_
#define _DRAW_BOARD_H_

#include "util.hpp"

namespace vas {

struct pixeler
{
  SDL_Texture* texture { nullptr };
  u32* pixels { nullptr };
  u32 width {};
  u32 height {};
  pixeler( SDL_Texture* Texture, u32 Width, u32 Height )
    : texture( Texture ), width( Width ), height( Height )
  {
    int pitch;
    if ( SDL_LockTexture( texture, nullptr, (void**)&pixels, &pitch ) != 0 ) {
      // do something
    }
  }
  ~pixeler() { SDL_DestroyTexture( texture ); }
  void setIndex( u32 x, u32 y, RGB color ) const
  {
    if ( x >= width || y >= height )
      return;
    pixels[y * width + x] = RGBtoU32( color );
  }
  void setLine( pos from, pos to, RGB color ) const
  {
    int dx = static_cast<int>( to.x - from.x );
    int dy = static_cast<int>( to.y - from.y );
    int stepX { 1 };
    int stepY { 1 };

    if ( dx < 0 ) {
      stepX = -1;
      dx = -dx;
    }
    if ( dy < 0 ) {
      stepY = -1;
      dy = -dy;
    }
    int d2x = 2 * dx;
    int d2y = 2 * dy;
    int d2y_minus_d2x = d2y - d2x;
    int sx = from.x;
    int sy = from.y;

    if ( dy <= dx ) {
      int flag = d2y - dx;
      for ( int i = 0; i <= dx; i++ ) {
        setIndex( sx, sy, color );
        sx += stepX;
        if ( flag <= 0 ) {
          flag += d2y;
        } else {
          sy += stepY;
          flag += d2y_minus_d2x;
        }
      }
    } else {
      int flag = d2x - dy;
      for ( int i = 0; i <= dy; i++ ) {
        setIndex( sx, sy, color );
        sy += stepY;
        if ( flag <= 0 ) {
          flag += d2x;
        } else {
          sx += stepX;
          flag -= d2y_minus_d2x;
        }
      }
    }
  }
  void setLine( u32 y, u32 x_start, u32 x_end, RGB color ) const
  {
    auto pos { y * width };
    std::fill( pixels + pos + x_start, pixels + pos + x_end, RGBtoU32( color ) );
  }
  void FillBackground( RGB color ) const
  {
    std::fill( pixels, pixels + width * height, RGBtoU32( color ) );
  }
  void ClearBuffer() const { std::fill( pixels, pixels + width * height, 0 ); }
};

struct DrawBoard
{
  pixeler pic;
  vector<bool> drawable;
  struct Config
  {
    RGB background { 0x22, 0x22, 0x22 };
  } config {};
  DrawBoard( SDL_Texture* texture, u32 width, u32 height )
    : pic { texture, width, height }, drawable( width * height, true )
  {}
  void ClearBuffer()
  {
    std::fill( drawable.begin(), drawable.end(), true );
    // pic.ClearBuffer();
    FillBackground();
  }
  void FillBackground() const { pic.FillBackground( config.background ); }
  void DrawChar( const u32 width,
                 const u32 height,
                 const u8* buffer,
                 const u32 xoffst,
                 const u32 yoffst,
                 const RGB BackgoundColor )
  {
    RGB char_color { getHighContrastColor( BackgoundColor ) };
    // u8 ava_rgb = ( BackgoundColor.r + BackgoundColor.g + BackgoundColor.b ) / 3;
    // u8 rdiff = char_color.r - BackgoundColor.r;
    // u8 gdiff = char_color.g - BackgoundColor.g;
    // u8 bdiff = char_color.b - BackgoundColor.b;
    for ( u32 y = 0; y < height; y++ ) {
      for ( u32 x = 0; x < width; x++ ) {
        // if ( buffer[y * width + x] > ava_rgb ) {
        //   setIndex( x + xoffst, y + yoffst, char_color );
        // } else {
        //   setIndex( x + xoffst, y + yoffst, BackgoundColor );
        // }
        float ratio = buffer[y * width + x] / 255.f;
        // RGB c = { static_cast<u8>( BackgoundColor.r + rdiff * ratio ),
        //           static_cast<u8>( BackgoundColor.g + gdiff * ratio ),
        //           static_cast<u8>( BackgoundColor.b + bdiff * ratio ) };
        RGB c = { static_cast<u8>( BackgoundColor.r * ( 1 - ratio ) + char_color.r * ratio ),
                  static_cast<u8>( BackgoundColor.g * ( 1 - ratio ) + char_color.g * ratio ),
                  static_cast<u8>( BackgoundColor.b * ( 1 - ratio ) + char_color.b * ratio ) };
        setIndex( x + xoffst, y + yoffst, c );
      }
    }
  }
  void setIndex( u32 x, u32 y, RGB color )
  {
    if ( drawable[x + y * pic.width] ) {
      pic.setIndex( x, y, color );
      drawable[x + y * pic.width] = false;
    }
  }
  void setIndex_Coverable( u32 x, u32 y, RGB color )
  {
    if ( drawable[x + y * pic.width] ) {
      pic.setIndex( x, y, color );
    }
  }
  void setIndex_force( u32 x, u32 y, RGB color ) const { pic.setIndex( x, y, color ); }
  void DrawRect( int x1, int y1, int x2, int y2, RGB color )
  {
    const int dx = x1 > x2 ? -1 : 1;
    const int dy = y1 > y2 ? -1 : 1;
    for ( int i = x1; i != x2; i += dx ) {
      for ( int j = y1; j != y2; j += dy ) {
        setIndex( i, j, color );
      }
    }
  }
  void DrawRect_Coverable( int x1, int y1, int x2, int y2, RGB color )
  {
    const int dx = x1 > x2 ? -1 : 1;
    const int dy = y1 > y2 ? -1 : 1;
    for ( int i = x1; i != x2; i += dx ) {
      for ( int j = y1; j != y2; j += dy ) {
        setIndex_Coverable( i, j, color );
      }
    }
  }
  void Drawrect_force( int x1, int y1, int x2, int y2, RGB color ) const
  {
    const int dx = x1 > x2 ? -1 : 1;
    const int dy = y1 > y2 ? -1 : 1;
    for ( int i = x1; i != x2; i += dx ) {
      for ( int j = y1; j != y2; j += dy ) {
        setIndex_force( i, j, color );
      }
    }
  }
  void Clear( int x1, int y1, int x2, int y2 ) { DrawRect( x1, y1, x2, y2, config.background ); }
  void Clear_Coverable( int x1, int y1, int x2, int y2 )
  {
    DrawRect_Coverable( x1, y1, x2, y2, config.background );
  }
  void Clear_force( int x1, int y1, int x2, int y2 ) const
  {
    Drawrect_force( x1, y1, x2, y2, config.background );
  }
};

}
#endif