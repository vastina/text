#include "text.hpp"
#include "window.hpp"

using namespace std::chrono_literals;
const static vas::BeforeStart bs {};
int main( int argc, char* argv[] )
{
  try {
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
    ts.setRect( 100, 100, ww - 200, hh - 350 );
    ts.background = { 0x70, 0x80, 0x90 };
    ts.LoadContent();
    // ts.calculateContent();

    vas::typeSetter load { "====================================", b };
    load.setRect( 100, hh - 200, ww - 200, 200 );
    load.config.ygap = 12;
    load.LoadContent();
    load.calculateContent();

    vas::typeSetter Title { "这是标题", b, {}, "simhei.ttf" };
    Title.setRect( 300, 20, ww - 400, 70 );
    Title.config.char_height *= 2;
    Title.background = { 0xff, 0xa0, 0x7a };
    Title.LoadContent();
    Title.calculateContent();

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
      if ( !str.empty() ) {
        std::cout << str << '\n';
        SDL_SetClipboardText( str.data() );
      }
    } );
    player.addhandle( SDL_MOUSEMOTION, [&m]( const SDL_Event& e ) { m.DealMove( e ); } );
    player.addStatehandle( [&m] { return !m.moved_last_frame && m.down; }, [&m] { m.DealDownState(); } );

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
