#ifndef _MAIN_H_
#define _MAIN_H_

#include "window.hpp"

struct mousehandle : vas::mousehandle
{
  vas::DrawBoard& b;

  mousehandle( vas::DrawBoard& B ) : b( B ) {}
  ~mousehandle() = default;

  void DealDown( const SDL_Event& e ) override
  {
    leftdown = true;
    xfirst = e.button.x;
    yfirst = e.button.y;
    xcur = xfirst;
    ycur = yfirst;
    // if ( down ) {
    //   // error, if down, why there is another down
    // } else {
    // }
  }
  void DealUp( const SDL_Event& e ) override
  {
    leftdown = false;
    xcur = std::max( std::min( 0, e.button.x ), static_cast<int>( b.pic.width ) );
    ycur = std::max( std::min( 0, e.button.y ), static_cast<int>( b.pic.height ) );
  }

  void DealMove( const SDL_Event& e ) override
  {
    if ( leftdown ) {
      if ( e.button.x < 0 || e.button.y < 0 || static_cast<u64>( e.button.x ) > b.pic.width
           || static_cast<u64>( e.button.y ) > b.pic.height ) {
        return;
      }
      b.Clear_Coverable( xfirst, yfirst, xcur, ycur );
      xcur = e.button.x;
      ycur = e.button.y;
      b.DrawRect_Coverable( xfirst, yfirst, xcur, ycur, { 0, 0, 0xff } );
    }
    moved_last_frame = true;
  }
  void DealDownState() override
  {
    b.DrawRect_Coverable( xfirst, yfirst, xcur, ycur, { 0, 0, 0xff } );
  }
  void freshState() override { moved_last_frame = false; }
};

#endif