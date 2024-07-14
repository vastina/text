#include "text.hpp"
#include "window.hpp"

int main( int argc, char* argv[] )
{
  constexpr u32 ww { 1000 };
  constexpr u32 hh { 800 };
  auto player { vas::Player( argc > 1 ? argv[1] : argv[0], ww, hh ) };
  vas::DrawBoard b { player.CreateTexture(), ww, hh };

  vas::typeSetter ts { "abcdefghijklmnopqrstuvwxyz"
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
    SDL_SetClipboardText( inRect.data() );
  } );
  player.addhandle( SDL_MOUSEMOTION, [&m]( const SDL_Event& e ) { m.DealMove( e ); } );

  while ( !player.ShouldQuit() ) {
    ts.DrawContent();
    player.HandleEvent();
    player.Render( b );

    player.Clear();
    std::fill( b.drawable.begin(), b.drawable.end(), true );
  }

  return 0;
}
