#include "launch.hpp"

const static vas::BeforeStart bs {};
int main( int argc, char* argv[] )
{
  try {
    vas::mw::launcher::Start();
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << '\n';
    std::cerr.flush();
    std::clog.flush();
    std::cout.flush();
    return 1;
  }

  return 0;
}