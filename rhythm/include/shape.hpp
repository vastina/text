#include "DrawBoard.hpp"

namespace vas {

struct shape
{
  ~shape() = default;
  virtual void Draw() = 0;
  virtual void Draw( DrawBoard& board ) = 0;
};

struct round : shape
{
  u32 centerX;
  u32 centerY;
  u32 radius;
  RGB color;

  round( u32 centerx, u32 centery, u32 r )
    : centerX( centerx )
    , centerY( centery )
    , radius( r )
  {}
  ~round() = default;
  virtual void Draw() override
  {
    // draw a round
  }
  virtual void Draw( DrawBoard& board ) override
  {
    const u32 r2 { radius * radius };
    for( u32 i = centerX - radius; i <= centerX + radius; i++ )
      for( u32 j = centerY - radius; j <= centerY + radius; j++ ) {
        const u32 dis = ( i - centerX ) * ( i - centerX ) + ( j - centerY ) * ( j - centerY );
        if( dis <= r2 )
          board.pic.setIndex( i, j, color );
      }
  }
};

}