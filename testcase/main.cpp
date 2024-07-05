#include "png.hpp"
#include "ttf.hpp"
#include "window.hpp"

#include <chrono>
#include <array>
#include <filesystem>

using namespace std::chrono_literals;

int main( int argc, char* argv[] )
{
  // todo, search for ttf in platforms
  constexpr std::string_view font_path { "KAISG.ttf" };
  auto text { vastina::Text( font_path.data() ) };

  constexpr std::string_view name { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                                    "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" };
  constexpr auto length = name.size();

  unsigned wa = 0;
  unsigned h = 0;
  for ( int i = 0; i < length; i++ ) {
    text.AddChar( name[i], 0, 32 * 64 );
    FT_Bitmap* bitmap { text.LoadChar( name[i] ) };
    wa += bitmap->width;
    h = h > bitmap->rows ? h : bitmap->rows;
  }

  // 创建窗口
  const unsigned ww = wa / 2;
  const unsigned hh = h * 12;
  auto player { vastina::img_player( ww, hh ) };

  unsigned r = 5;
  auto start { std::chrono::system_clock::now() }; // frame control
  const std::string filename { "./test/window.png" };
  vastina::png::png p { filename, ww, hh };
  std::filesystem::create_directories( "./test" );
  while ( !player.ShouldQuit() ) {
    std::filesystem::remove( filename );
    unsigned xoffset = 200; // ww / 10;
    unsigned yoffset = hh / 3;
    unsigned w_current = 0;
    for ( int i = 0; i < length; i++ ) {
      if ( w_current + xoffset > ww - xoffset ) {
        w_current = 0;
        yoffset += h;
      }
      FT_Bitmap* bitmap { text.LoadChar( name[i] ) };
      p.DrawChar( bitmap->width, bitmap->rows, bitmap->buffer, w_current + xoffset, yoffset );
      w_current += bitmap->width;
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

      auto now { std::chrono::system_clock::now() };
      while ( now - start < 5ms )
        now = std::chrono::system_clock::now();
      start = now;

      std::fill( p.data.begin(), p.data.end(), 0 );
    }
  }

  return 0;
}
