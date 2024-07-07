#include "png.hpp"
#include "ttf.hpp"
#include "window.hpp"

#include <chrono>
#include <array>
#include <filesystem>

using namespace std::chrono_literals;

struct DrawBoard
{
  vas::png::png pic;
  std::vector<bool> drawable;
  DrawBoard( const std::string& filename, u32 width, u32 height )
    : pic { filename, width, height }, drawable( width * height )
  {}
};

struct mousehandle
{
  struct downhandle
  {
    bool first { true };
    u32 x[0u];
    u32 y { 0u };
  } d;
  struct poshandle
  {

  } p;
  // ...
};

int main( int argc, char* argv[] )
{
  auto text { vas::Text( "KAISG.ttf" ) };

  constexpr std::string_view name { "abcdefghijklmnopqrstuvwxyz"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "1234567890"
                                    "~`!@#$%^&*()_-+={}[]:;\"'<,>.?/|\\" };
  constexpr auto length = name.size();

  u32 h = 0;
  for ( int i = 0; i < length; i++ ) {
    text.AddChar( name[i], 0, 32 * 64 );
    FT_Bitmap* bitmap { text.LoadChar( name[i] ) };
    h = h > bitmap->rows ? h : bitmap->rows;
  }

  // 创建窗口
  constexpr u32 ww { 1000 };
  constexpr u32 hh { 800 };
  auto player { vas::img_player( "abc", ww, hh ) };

  u32 r = 5;
  // auto start { std::chrono::system_clock::now() }; // frame control
  std::filesystem::create_directories( "./tmp" );
  const std::string filename { "tmp/window.png" };
  vas::png::png p { filename, ww, hh };
  DrawBoard b { filename, ww, hh };

  // player.addhandle(SDL_MOUSEBUTTONDOWN, [&b,
  //   xfirst {0u}, yfirst{0u}, firstdwon {true}]
  //     (const SDL_Event& e) mutable {
  //     if( firstdwon ) {
  //       firstdwon = false;
  //       xfirst = e.button.x;
  //       yfirst = e.button.y;
  //     } else {

  //     }
  // });
  while ( !player.ShouldQuit() ) {
    std::filesystem::remove( filename );
    u32 xoffset = 100; // ww / 10;
    u32 yoffset = hh / 8;
    u32 w_current = 0;
    for ( int i = 0; i < length; i++ ) {
      if ( w_current + xoffset > ww - xoffset ) {
        w_current = 0;
        yoffset += h;
      }
      FT_Bitmap* bitmap { text.LoadChar( name[i] ) };
      p.DrawChar( bitmap->width, bitmap->rows, bitmap->buffer, w_current + xoffset, yoffset + h - bitmap->rows );
      constexpr u32 gap = 10;
      w_current += bitmap->width + gap;
    }
    r = ( r + 1 ) % hh;
    r = r == 0 ? 5 : r;
    u32 rr1 = ( r - 1 ) * ( r - 1 ) - r * 2;
    u32 rr2 = ( r - 1 ) * ( r - 1 ) + r * 2;
    p.FillWith(
      [rr1, rr2]( u32 x, u32 y ) {
        u32 rr = x * x + y * y;
        return ( rr > rr1 && rr < rr2 );
      },
      { 0, 0, 0xff } );
    p.write();

    {
      player.LoadImg( filename.data() );
      player.HandleEvent();
      player.Clear();

      // auto now { std::chrono::system_clock::now() };
      // while ( now - start < 5ms )
      //   now = std::chrono::system_clock::now();
      // start = now;

      // std::fill( p.data.begin(), p.data.end(), 0 );
    }
  }

  std::filesystem::remove( filename );
  std::filesystem::remove( "./tmp" );
  return 0;
}
