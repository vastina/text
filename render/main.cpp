#include "png.hpp"
#include "ttf.hpp"
#include "window.hpp"
#include "DrawBoard.hpp"

#include <filesystem>

using namespace std::chrono_literals;

struct typeSetter
{
  u32 left { 0 };
  u32 top { 0 };
  u32 width { 0 };
  u32 height { 0 };
  struct Config
  {
    u32 xgap { 7 };
    u32 ygap { 0 };
    i32 char_height { 32 * 48 };
    // ...
  } config;
  std::string content;
  struct pos
  {
    u32 x;
    u32 y;
  };
  bool cache_avaliable { false };
  std::vector<pos> poscache;
  vas::DrawBoard& b;
  vas::Text charConfig { "KAISG.ttf" };

  typeSetter( const std::string& text, vas::DrawBoard& board ) : content( text ), poscache( text.size() ), b { board }
  {
    for ( const auto ch : text ) {
      charConfig.AddChar( ch, 0, config.char_height );
    }
  }

  void setRect( int l, int t, int w, int h )
  {
    left = l;
    top = t;
    width = w;
    height = h;
  }

  void DrawContent( u32 start = 0u, u32 charnum = UINT32_MAX )
  {
    charnum = charnum > content.size() ? content.size() : charnum;
    u32 w_current { 0u };
    u32 y_offset { 0u };
    for ( u32 i = start; i < charnum; i++ ) {
      if ( w_current > width ) {
        w_current = 0;
        y_offset += charConfig.getMaxcharheight() + config.ygap;
      }
      const FT_Bitmap* bitmap { charConfig.LoadCharBitmap( content[i] ) };
      if( ! cache_avaliable )
        poscache[i] = { left + w_current, top + y_offset };
      b.DrawChar( bitmap->width,
                  bitmap->rows,
                  bitmap->buffer,
                  left + w_current,
                  top + y_offset + ( charConfig.getMaxcharheight() - bitmap->rows ) );
      w_current += bitmap->width + config.xgap;
    }
    cache_avaliable = true;
  }

  bool charinRect( u32 index, u32 x1, u32 y1, u32 x2, u32 y2 )
  { // left_top and right_bottom
    auto* bitmap = charConfig.LoadCharBitmap( content[index] );
    u32 left_top_x = min( x1, x2 );
    u32 left_top_y = min( y1, y2 );
    u32 right_bottom_x = max( x1, x2 );
    u32 right_bottom_y = max( y1, y2 );
    return ( left_top_x <= poscache.at( index ).x ) && ( left_top_y <= poscache.at( index ).y )
           && ( right_bottom_x >= poscache.at( index ).x + bitmap->width )
           && ( right_bottom_y >= poscache.at( index ).y + bitmap->rows );
    // return l <= left && left + bitmap->width <= l + w && t <= top && top + bitmap->rows <= t + h;
  }
};

int main( int argc, char* argv[] )
{
  constexpr u32 ww { 1000 };
  constexpr u32 hh { 800 };
  auto player { vas::img_player( argc > 1 ? argv[1] : argv[0], ww, hh ) };
  std::filesystem::create_directories( "./tmp" );
  const std::string filename { "tmp/window.png" };
  vas::DrawBoard b { filename, ww, hh };

  typeSetter ts { "abcdefghijklmnopqrstuvwxyz"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "1234567890"
                  "~`!@#$%^&*()_-+={}[]:;\"'<,>.?/|\\",
                  b };
  ts.setRect( 100, 100, ww - 200, hh - 200 );

  vas::mousehandle m { b };
  player.addhandle( SDL_MOUSEBUTTONDOWN, [&m]( const SDL_Event& e ) { m.DealDown( e ); } );
  player.addhandle( SDL_MOUSEBUTTONUP, [&m, &ts]( const SDL_Event& e ) {
    m.DealUp( e );
    std::string inRect {};
    for ( u32 i = 0; i < ts.content.size(); i++ ) {
      if ( ts.charinRect( i, m.xcur, m.ycur, m.xfirst, m.yfirst ) ) {
        inRect.push_back( ts.content[i] );
      }
    }
    std::cout << inRect << std::endl;
  } );
  player.addhandle( SDL_MOUSEMOTION, [&m]( const SDL_Event& e ) { m.DealMove( e ); } );

  const auto length = ts.content.size();
//  int chars = 0;
  while ( !player.ShouldQuit() ) {
    std::filesystem::remove( filename );

    ts.DrawContent( 0, length );
//    chars = ( chars + 1 ) % ( length + 1 );

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
