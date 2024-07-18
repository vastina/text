#ifndef _DRAW_BOARD_H_
#define _DRAW_BOARD_H_

#include <SDL.h>
#include "vasdef.hpp"

namespace vas {

struct pixeler
{
  SDL_Texture* texture { nullptr };
  u32* pixels { nullptr };
  u32 width {};
  u32 height {};
  pixeler( SDL_Texture* Texture, u32 Width, u32 Height ) : texture( Texture ), width( Width ), height( Height )
  {
    int pitch;
    if ( SDL_LockTexture( texture, nullptr, (void**)&pixels, &pitch ) != 0 ) {
      // do something
    }
  }
  ~pixeler() { SDL_DestroyTexture( texture ); }
  void setIndex( u32 x, u32 y, RGB color ) const
  {
    if ( x > width || y > height )
      return;
    pixels[y * width + x] = ( color.r << 16 ) | ( color.g << 8 ) | color.b;
  }
  void FillBackground( RGB color ) const
  {
    std::fill( pixels, pixels + width * height, ( color.r << 16 ) | ( color.g << 8 ) | color.b );
  }
  void ClearBuffer() const { std::fill( pixels, pixels + width * height, 0 ); }
};

struct DrawBoard
{
  pixeler pic;
  std::vector<bool> drawable;
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
  void DrawChar( const u32 width, const u32 height, const u8* buffer, const u32 xoffst, const u32 yoffst )
  {
    for ( u32 y = 0; y < height; y++ ) {
      for ( u32 x = 0; x < width; x++ ) {
        auto val { buffer[y * width + x] };
        if ( val > 0 )
          setIndex( x + xoffst, y + yoffst, { val, val, val } );
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
  void Clear_Coverable( int x1, int y1, int x2, int y2 ) { DrawRect_Coverable( x1, y1, x2, y2, config.background ); }
  void Clear_force( int x1, int y1, int x2, int y2 ) const { Drawrect_force( x1, y1, x2, y2, config.background ); }
};

}
#endif