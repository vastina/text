#include "png.hpp"
#include "ttf.hpp"
#include "window.hpp"

#include <chrono>
#include <array>
#include <filesystem>

using namespace std::chrono_literals;

int main( int argc, char* argv[] )
{
  auto text { vastina::Text( "KAISG.ttf" ) };

  constexpr std::string_view name { "abcdefghijklmnopqrstuvwxyz"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "1234567890"
                                    "~`!@#$%^&*()_-+={}[]:;\"'<,>.?/|\\" };
  constexpr auto length = name.size();

  unsigned h = 0;
  for ( int i = 0; i < length; i++ ) {
    text.AddChar( name[i], 0, 32 * 64 );
    FT_Bitmap* bitmap { text.LoadChar( name[i] ) };
    h = h > bitmap->rows ? h : bitmap->rows;
  }

  // 创建窗口
  constexpr unsigned ww { 1000 };
  constexpr unsigned hh { 800 };
  auto player { vastina::img_player( "abc", ww, hh ) };

  unsigned r = 5;
  // auto start { std::chrono::system_clock::now() }; // frame control
  std::filesystem::create_directories( "./tmp" );
  const std::string filename { "tmp/window.png" };
  vastina::png::png p { filename, ww, hh };

  while ( !player.ShouldQuit() ) {
    std::filesystem::remove( filename );
    unsigned xoffset = 100; // ww / 10;
    unsigned yoffset = hh / 8;
    unsigned w_current = 0;
    for ( int i = 0; i < length; i++ ) {
      if ( w_current + xoffset > ww - xoffset ) {
        w_current = 0;
        yoffset += h;
      }
      FT_Bitmap* bitmap { text.LoadChar( name[i] ) };
      p.DrawChar( bitmap->width, bitmap->rows, bitmap->buffer, w_current + xoffset, yoffset + h - bitmap->rows );
      constexpr unsigned gap = 10;
      w_current += bitmap->width + gap;
    }
    r = ( r + 1 ) % hh;
    r = r == 0 ? 5 : r;
    unsigned rr1 = ( r - 1 ) * ( r - 1 ) - r * 2;
    unsigned rr2 = ( r - 1 ) * ( r - 1 ) + r * 2;
    p.FillWith(
      [rr1, rr2]( unsigned x, unsigned y ) {
        unsigned rr = x * x + y * y;
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
