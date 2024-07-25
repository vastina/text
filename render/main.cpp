#include "text.hpp"

#include "main.hpp"

using namespace std::chrono_literals;
int main( int argc, char* argv[] )
{
  try {
    constexpr u32 ww { 1000 };
    constexpr u32 hh { 800 };
    auto player { vas::Player( argc > 1 ? argv[1] : argv[0], ww, hh ) };
    vas::DrawBoard b { player.CreateTexture(), ww, hh };

    vas::typeSetter ts {
      vas::Readfile( "test/content.txt" ), b, {}, "C:/Windows/Fonts/consola.ttf" };
    ts.setRect( 5, 100, ww - 10, hh - 250 );
    ts.background = { 0x70, 0x80, 0x90 };
    ts.config.char_height -= ts.config.char_height * 3 / 5;
    ts.LoadContent();
    ts.calculateContent();

    vas::typeSetter load {
      "========================================================================================",
      b };
    load.setRect( 100, hh - 100, 1919810, 114514 );
    load.config.draw_start_y = 7;
    load.config.ygap = 12;
    load.background = { 0x66, 0xcd, 0xaa };
    load.LoadContent();
    load.calculateContent( true, true );

    vas::typeSetter Title { "这是标题", b, {} };
    Title.setRect( 300, 20, ww - 400, 70 );
    Title.config.char_height *= 2;
    Title.background = { 0x8a, 0x2b, 0xe2 };
    Title.LoadContent();
    Title.calculateContent( true, true );

    mousehandle m { b };
    player.addEventhandle( SDL_MOUSEBUTTONDOWN, [&m]( const SDL_Event& e ) { m.DealDown( e ); } );
    player.addEventhandle( SDL_MOUSEBUTTONUP, [&]( const SDL_Event& e ) {
      m.DealUp( e );
      std::vector<u32> inRect {};
      const auto clip { [&]( const vas::typeSetter& t ) {
        const u32 length = static_cast<u32>( t.content.size() );
        for ( u32 i = 0; i < length; i++ )
          if ( t.charinRect( i, m.xcur, m.ycur, m.xfirst, m.yfirst ) )
            inRect.push_back( t.content[i] );
      } };
      clip( ts );
      clip( load );
      clip( Title );

      auto str = vas::Text::utf32_to_utf8( inRect );
      if ( !str.empty() ) {
        std::cout << str << '\n';
        SDL_SetClipboardText( str.data() );
      }
    } );
    player.addEventhandle( SDL_MOUSEMOTION, [&m]( const SDL_Event& e ) { m.DealMove( e ); } );
    player.addStatehandle( [&m] { return !m.moved_last_frame && m.leftdown; },
                           [&m] { m.DealDownState(); } );

    u32 chars = 0;
    u32 count = 0;
    u32 frame = 0;
    auto start { std::chrono::high_resolution_clock::now() };
    while ( !player.ShouldQuit() ) {
      b.ClearBuffer();

      load.DrawContent( 0, chars );
      chars = ( chars + ( count = ( count + 1 ) % 64 ) / 63 ) % ( load.content.size() + 1 );

      ts.DrawContent();
      Title.DrawContent();

      player.HandleEvent();
      player.HandleState();
      player.Render( b );
      m.freshState();
      {
        auto end { std::chrono::high_resolution_clock::now() };
        if ( end - start >= 1s ) {
          std::cout << "FPS: " << frame << '\n';
          frame = 0;
          start = end;
        }
        frame++;
      }
    }
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << '\n';
    std::cerr.flush();
    std::clog.flush();
    std::cout.flush();
    return 1;
  }

  return 0;
}
