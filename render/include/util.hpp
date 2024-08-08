#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <iostream>
#endif

namespace vas {
static int ScreenWidth;
static int ScreenHeight;
static int dpiX;
static int dpiY;
inline static void GetScreenSize()
{
#ifdef _WIN32
  ScreenWidth = GetSystemMetrics( SM_CXSCREEN );
  ScreenHeight = GetSystemMetrics( SM_CYSCREEN );
#elif __linux__
  Display* display = XOpenDisplay( nullptr );
  if ( display == nullptr ) {
    std::cerr << "Cannot open display" << '\n';
    ScreenWidth = ScreenHeight = -1; // Return invalid screen size
    return;
  }
  vas::ScreenWidth = XDisplayWidth( display, DefaultScreen( display ) );
  vas::ScreenHeight = XDisplayHeight( display, DefaultScreen( display ) );
  XCloseDisplay( display );
#endif
}
inline static void GetScreenDPI()
{
#ifdef _WIN32
  HDC screen = GetDC( nullptr );
  vas::dpiX = GetDeviceCaps( screen, LOGPIXELSX );
  vas::dpiY = GetDeviceCaps( screen, LOGPIXELSY );
  ReleaseDC( nullptr, screen );
#elif __linux__
  Display* display = XOpenDisplay( nullptr );
  if ( display == nullptr ) {
    std::cerr << "Cannot open display" << '\n';
    dpiX = dpiY = -1; // Return invalid DPI values
    return;
  }
  int screen_width_mm = XDisplayWidthMM( display, DefaultScreen( display ) );
  int screen_height_mm = XDisplayHeightMM( display, DefaultScreen( display ) );
  dpiX = (int)( 25.4 * vas::ScreenWidth / screen_width_mm );
  dpiY = (int)( 25.4 * vas::ScreenHeight / screen_height_mm );
  XCloseDisplay( display );
#else
  dpiX = dpiY = -1; // Unsupported platform
#endif
}

}
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "vasdef.hpp"
namespace vas {
static inline string SearchTTf()
{
  string path {};
#ifdef _WIN32
  path = "C:\\Windows\\Fonts\\Deng.ttf";
#elif __linux__
  path = "/usr/share/fonts/truetype/dejavu/DejaVuMathTeXGyre.ttf";
  path = "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc";
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
static inline string Readfile( const string& path )
{
  const auto length { std::filesystem::file_size( path ) };
  std::ifstream file { path, std::ios::binary };
  string content( length, 0 );
  file.read( content.data(), length );
  file.close();
  return content;
}
}

/*
  init things before window start
*/
#include <SDL.h>
// #include <SDL_image.h>
#include <iostream>

namespace vas {
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
    GetScreenSize();
    if ( ScreenWidth <= 0 || ScreenHeight <= 0 || ScreenWidth > 20000 || ScreenHeight > 20000 ) {
      std::cerr << "get screen size failed" << '\n';
      return false;
    }
    GetScreenDPI();
    if ( vas::dpiX == -1 || vas::dpiY == -1 ) {
      std::cerr << "get screen dpi failed" << '\n';
      return false;
    }
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
const static vas::BeforeStart bs {};
}

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
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
#undef min
#undef max
#undef RGB
// fuck you, ms
#elif __LINUX__
// fuck you, X11/X.h
#undef Success

#endif

namespace vas {
// low bits
static inline RGB toRGB( u32 color )
{
  return { u8( ( color >> 16 ) & 0xff ), u8( ( color >> 8 ) & 0xff ), u8( color & 0xff ) };
}
static inline u32 RGBtoU32( RGB color )
{
  return ( color.r << 16 ) | ( color.g << 8 ) | color.b;
}
}

#define print_once( description, data )                                                            \
  {                                                                                                \
    static bool fortest { true };                                                                  \
    if ( fortest ) {                                                                               \
      fortest = false;                                                                             \
      std::cout << ( description ) << ' ' << ( data ) << '\n';                                     \
    }                                                                                              \
  }

#endif