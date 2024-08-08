#include "window.hpp"
// #include "text.hpp"
#include "shape.hpp"

// SDL_main cause an AddressSanitizer check fail
// asan_thread.cpp:403 "((ptr[0] == kCurrentStackFrameMagic)) != (0)" (0x0, 0x0)
#undef main
int main( int argc, char* argv[] )
{
  try {
    constexpr u32 ww { 1000 };
    constexpr u32 hh { 1200 };
    auto player { vas::Player( argc > 1 ? argv[1] : argv[0], ww, hh ) };
    vas::DrawBoard b { player.CreateTexture(), ww, hh };

    std::array<vas::round*, 4> rounds;
    std::array<int, 4> steps { 1, 2, 3, 4 };
    constexpr auto genColor {
      []() -> glm::vec4 { return { rand() % 256, rand() % 256, rand() % 256, 1.f }; } };
    constexpr u32 radius { 100u };
    constexpr u32 unitWidth { ww / rounds.size() };
    constexpr u32 startHeight { radius };
    srand( time( nullptr ) );
    for ( u32 i = 0; i < rounds.size(); i++ ) {
      rounds[i] = new vas::round( unitWidth / 2 + unitWidth * i, startHeight, radius );
      rounds[i]->color = genColor();
    }

    player.addEventhandle( SDL_KEYDOWN, [&player]( const SDL_Event& ) { player.Quit(); } );

    while ( !player.ShouldQuit() ) {
      b.ClearBuffer();

      for ( int i = 0; auto& r : rounds ) {
        r->Draw( b );
        r->centerY += steps[i];
        if ( r->centerY > hh - radius || r->centerY < radius ) {
          steps[i] = -steps[i];
          r->centerY += steps[i];
        }
        i++;
      }

      player.HandleEvent();
      player.HandleState();
      player.Render( b );
      SDL_Delay( 1u );
    }
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << '\n';
    std::cerr.flush();
    std::clog.flush();
    std::cout.flush();
    return 1;
  }
}
