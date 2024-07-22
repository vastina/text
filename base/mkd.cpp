#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "mkd.hpp"

namespace vas {
namespace mkd {

Parser::Parser( const string& filepath ) : tokens { 0 }
{
  if ( !std::filesystem::exists( filepath ) )
    throw std::runtime_error( "File not found" );
}

}
}