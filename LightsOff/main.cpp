#include "game.hpp"

int main( int argc, char* argv[] )
{
  try {
    vas::lo::Game g {};
    g.MainLoop();
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << std::endl;
    std::cerr.flush();
    std::clog.flush();
    std::cout.flush();
  }
  return 0;
}