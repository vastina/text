#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <iostream>
#endif

namespace vas {
inline static void GetScreenDPI( int& dpiX, int& dpiY )
{
#ifdef _WIN32
  HDC screen = GetDC( NULL );
  dpiX = GetDeviceCaps( screen, LOGPIXELSX );
  dpiY = GetDeviceCaps( screen, LOGPIXELSY );
  ReleaseDC( NULL, screen );
#elif __APPLE__
  return;
#elif __linux__
  Display* display = XOpenDisplay( NULL );
  if ( display == NULL ) {
    std::cerr << "Cannot open display" << '\n';
    dpiX = dpiY = -1; // Return invalid DPI values
    return;
  }
  int screen_width = XDisplayWidth( display, DefaultScreen( display ) );
  int screen_height = XDisplayHeight( display, DefaultScreen( display ) );
  int screen_width_mm = XDisplayWidthMM( display, DefaultScreen( display ) );
  int screen_height_mm = XDisplayHeightMM( display, DefaultScreen( display ) );
  dpiX = (int)( 25.4 * screen_width / screen_width_mm );
  dpiY = (int)( 25.4 * screen_height / screen_height_mm );
  XCloseDisplay( display );
#else
  dpiX = dpiY = -1; // Unsupported platform
#endif
}

}
#include <filesystem>
#include <string>

namespace vas {
static inline std::string SearchTTf()
{
  std::string path {};
#ifdef _WIN32
  path = "C:\\Windows\\Fonts\\Deng.ttf";
#elif __linux__
  path = "/usr/share/fonts/truetype/dejavu/DejaVuMathTeXGyre.ttf";
#endif
  if ( !std::filesystem::exists( path ) )
    throw std::runtime_error( "no ttf found" );
  return path;
  // #ifdef _WIN32
  //   search_path = "C:\\Windows\\Fonts";
  // #elif __linux__
  //   search_path = "/usr/share/fonts/truetype";
  // #endif
  //   for ( const auto& entry : std::filesystem::directory_iterator( search_path ) ) {
  //     if( !entry.is_directory() ) {
  //       const auto path { entry.path().filename().string() };
  //       if( path.ends_with(".ttf") )
  //         return path;
  //     }
  //   }
  //   throw std::runtime_error( "no ttf found" );
}
}

#include <SDL.h>
// #include <SDL_image.h>
#include <iostream>

namespace vas {
static int dpiX;
static int dpiY;

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
  ~BeforeStart()
  {
    // IMG_Quit();
    SDL_Quit();
  }

private:
  static inline bool init()
  {
    GetScreenDPI( vas::dpiX, vas::dpiY );
    if ( vas::dpiX == -1 || vas::dpiY == -1 )
      return false;
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
      std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
      return false;
    }
    // if ( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ) {
    //   std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << '\n';
    //   return false;
    // }

    return true;
  }
};
}

#include "vasdef.hpp"
namespace vas {
static inline double calculateLuminance( RGB color )
{
  return 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
}
static inline double calculateContrastRatio( RGB color1, RGB color2 )
{
  double luminance1 = calculateLuminance( color1 );
  double luminance2 = calculateLuminance( color2 );
  if ( luminance1 < luminance2 ) {
    std::swap( luminance1, luminance2 );
  }
  return ( luminance1 + 0.05 ) / ( luminance2 + 0.05 );
}
static inline RGB getHighContrastColor( RGB backgroundColor )
{
  // nothing special here
  if ( calculateLuminance( backgroundColor ) > 128 ) {
    return { 0, 0, 0 };
  }
  return { 0xff, 0xff, 0xff };
}
}

#ifdef _WIN32
#include <utf8.h>
#elif __linux__
// there is another pakage called libutf8.h-dev, so use utf8cpp/utf8.h instead of utf8.h
#include <utf8cpp/utf8.h>
#endif

#ifdef _WIN32
  #undef min
  #undef max
  // who define this? MSVC or freetype or SDL2? fuck you
#endif

#endif