#include "text.hpp"
#include "window.hpp"

const static BeforeStart bs {};
int main( int argc, char* argv[] )
{
  constexpr u32 ww { 1000 };
  constexpr u32 hh { 800 };
  auto player { vas::Player( argc > 1 ? argv[1] : argv[0], ww, hh ) };
  vas::DrawBoard b { player.CreateTexture(), ww, hh };

  vas::typeSetter ts { "abcdefghijklmnopqrstuvwxyz\n"
                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
                       "1234567890\n"
                       "~`!@#$%^&*()_-+={}[]:;\"'<,>.?/|\\\n"
                       "你好，世界！",
                       b };
  ts.setRect( 100, 100, ww - 200, hh - 200 );
  vas::typeSetter load { "====================================", b };
  load.setRect( 100, hh - 200, ww - 200, 200 );
  load.config.ygap = 7;

  vas::mousehandle m { b };
  player.addhandle( SDL_MOUSEBUTTONDOWN, [&m]( const SDL_Event& e ) { m.DealDown( e ); } );
  player.addhandle( SDL_MOUSEBUTTONUP, [&m, &ts, &load]( const SDL_Event& e ) {
    m.DealUp( e );
    std::vector<u32> inRect {};
    u32 length = static_cast<u32>( ts.content.size() );
    for ( u32 i = 0; i < length; i++ )
      if ( ts.charinRect( i, m.xcur, m.ycur, m.xfirst, m.yfirst ) )
        inRect.push_back( ts.content[i] );
    length = static_cast<u32>( load.content.size() );
    for ( u32 i = 0; i < length; i++ )
      if ( load.charinRect( i, m.xcur, m.ycur, m.xfirst, m.yfirst ) )
        inRect.push_back( load.content[i] );

    auto str = vas::Text::utf32_to_utf8( inRect );
    std::cout << str << std::endl;
    if ( !str.empty() )
      SDL_SetClipboardText( str.data() );
  } );
  player.addhandle( SDL_MOUSEMOTION, [&m]( const SDL_Event& e ) { m.DealMove( e ); } );

  u32 chars = 0;
  while ( !player.ShouldQuit() ) {
    load.DrawContent( 0, chars );
    chars = ( chars + 1 ) % ( load.content.size() + 1 );
    ts.DrawContent();

    player.HandleEvent();
    player.Render( b );

    b.ClearBuffer();
  }

  return 0;
}
