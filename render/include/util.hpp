#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#error "no support on apple"
#else
#include <X11/Xlib.h>
#endif
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
    std::cerr << "Cannot open display" << std::endl;
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

#include <filesystem>
#include <string>

static inline std::string SearchTTf()
{

#ifdef _WIN32
  return "C:\\Windows\\Fonts\\arial.ttf";
#elif __linux__
  return "/usr/share/fonts/truetype/dejavu/DejaVuMathTeXGyre.ttf";
#endif
  //   std::string search_path {};
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

#endif