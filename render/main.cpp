#include "png.hpp"
#include "ttf.hpp"
#include "window.hpp"
#include "DrawBoard.hpp"

#include <filesystem>

using namespace std::chrono_literals;

int main( int argc, char* argv[] )
{
  auto text { vas::Text( "KAISG.ttf" ) };

  constexpr std::string_view content { "abcdefghijklmnopqrstuvwxyz"
                                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                       "1234567890"
                                       "~`!@#$%^&*()_-+={}[]:;\"'<,>.?/|\\" };
  constexpr auto length = content.size();

  u32 charheight = 0;
  for ( int i = 0; i < length; i++ ) {
    text.AddChar( content[i], 0, 32 * 64 );
    FT_Bitmap* bitmap { text.LoadCharBitmap( content[i] ) };
    charheight = charheight > bitmap->rows ? charheight : bitmap->rows;
  }

  // 创建窗口
  constexpr u32 ww { 1000 };
  constexpr u32 hh { 800 };
  auto player { vas::img_player( argc > 1 ? argv[1] : argv[0], ww, hh ) };

  std::filesystem::create_directories( "./tmp" );
  const std::string filename { "tmp/window.png" };
  vas::DrawBoard b { filename, ww, hh };

  vas::mousehandle m { b };
  player.addhandle( SDL_MOUSEBUTTONDOWN, [&m]( const SDL_Event& e ) { m.DealDown( e ); } );
  player.addhandle( SDL_MOUSEBUTTONUP, [&m]( const SDL_Event& e ) { m.DealUp( e ); } );
  player.addhandle( SDL_MOUSEMOTION, [&m]( const SDL_Event& e ) { m.DealMove( e ); } );

  int chars = 0;
  while ( !player.ShouldQuit() ) {
    std::filesystem::remove( filename );
    {
      constexpr u32 xoffset = 100; // ww / 10;
      u32 yoffset = hh / 8;
      u32 w_current = 0;
      chars = ( chars + 1 ) % ( length + 1 );
      for ( int i = 0; i < chars; i++ ) {
        if ( w_current + xoffset > ww - xoffset ) {
          w_current = 0;
          yoffset += charheight;
        }
        FT_Bitmap* bitmap { text.LoadCharBitmap( content[i] ) };
        b.DrawChar(
          bitmap->width, bitmap->rows, bitmap->buffer, w_current + xoffset, yoffset + charheight - bitmap->rows );
        constexpr u32 gap = 7;
        w_current += bitmap->width + gap;
      }
    }
    player.HandleEvent();
    b.pic.write();
    player.LoadImg( filename.data() );

    player.Clear();
    std::fill( b.drawable.begin(), b.drawable.end(), true );
  }

  std::filesystem::remove( filename );
  std::filesystem::remove( "./tmp" );
  return 0;
}
