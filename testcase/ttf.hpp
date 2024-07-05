#ifndef _FT_FREETYPE_CXX_H_
#define _FT_FREETYPE_CXX_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <unordered_map>

#include "vasdef.hpp"
#include "util.hpp"

namespace vastina {

static int dpiX;
static int dpiY;

class Text
{
private:
  FT_Library library;
  std::unordered_map<u32, FT_Face> faces;
  std::string font_path;

public:
  Text( std::string ttfpath = "KAISG.ttf" )
  {
    if ( !std::filesystem::exists( font_path ) ) {
      font_path = SearchTTf();
    } else {
      font_path = std::move( ttfpath );
    }
    if ( FT_Init_FreeType( &library ) ) {
      std::cerr << "Could not initialize FreeType library" << std::endl;
      throw std::runtime_error( "freetype" );
    }
    GetScreenDPI( dpiX, dpiY );
  }
  ~Text()
  {
    for ( const auto& [k, v] : faces ) {
      FT_Done_Face( v );
    }
    FT_Done_FreeType( library );
  }
  void AddChar( u32 char_code, i64 char_width, i64 char_height )
  {
    FT_Face face;
    if ( FT_New_Face( library, font_path.data(), 0, &face ) ) {
      std::cerr << "Could not open font" << std::endl;
      FT_Done_FreeType( library );
      return;
    }
    FT_Set_Char_Size( face, char_width, char_height, dpiX, dpiY );
    if ( FT_Load_Char( face, char_code, FT_LOAD_RENDER ) ) {
      std::cerr << "Could not load character" << char_code << std::endl;
      FT_Done_Face( face );
      FT_Done_FreeType( library );
      return;
    }
    faces.insert( std::make_pair( char_code, face ) );
  }

  FT_Bitmap* LoadChar( u32 char_code ) { return &( faces.at( char_code )->glyph->bitmap ); }
};

};

#endif