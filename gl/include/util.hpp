#ifndef _VAS_GL_UTILITY_H_
#define _VAS_GL_UTILITY_H_

#include "base.hpp"

namespace vas {
namespace gl {

class BeforeStart
{
public:
  BeforeStart()
  {
    if ( !init() ) {
      std::cerr.flush();
      std::exit( 1 );
    }
  }
  ~BeforeStart() { SDL_Quit(); }

private:
  static inline bool init()
  {
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
      std::cerr << "Failed to initialize SDL" << std::endl;
      return false;
    }
    if ( 0 != SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 ) )
      return false;
    if ( 0 != SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 ) )
      return false;
    if ( 0 != SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE ) )
      return false;

    stbi_set_flip_vertically_on_load( true );
    SDL_ShowCursor( SDL_DISABLE );
    SDL_SetRelativeMouseMode( SDL_TRUE );
    return true;
  }
};
const static ::vas::gl::BeforeStart bs {};

} // namespace gl
} // namespace vas

constexpr float PI = 3.14159265358979323846f;

#endif // _VAS_GL_UTILITY_H_