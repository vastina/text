#ifndef _FT_FREETYPE_CXX_H_
#define _FT_FREETYPE_CXX_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <unordered_map>
#include <stdexcept>

#include "util.hpp"

namespace vas {

class Text
{
private:
  FT_Library library;
  std::unordered_map<u64, FT_Face> faces;
  string font_path;
  u32 maxcharheight { 0u };
  u32 maxcharwidth { 0u };

public:
  static inline vector<uint32_t> utf8_to_utf32( const string& utf8_str )
  {
    vector<uint32_t> utf32_str;
    auto it = utf8_str.begin();
    while ( it != utf8_str.end() ) {
      utf32_str.push_back( utf8::next( it, utf8_str.end() ) );
    }
    return utf32_str;
  }
  static inline string utf32_to_utf8( const vector<uint32_t>& utf32_str )
  {
    string utf8_str;
    utf8::utf32to8( utf32_str.begin(), utf32_str.end(), back_inserter( utf8_str ) );
    return utf8_str;
  }

  Text( string ttfpath = "KAISG.ttf" )
  {
    if ( !std::filesystem::exists( ttfpath ) ) {
      font_path = SearchTTf();
    } else {
      font_path = std::move( ttfpath );
    }
    if ( FT_Init_FreeType( &library ) ) {
      std::cerr << "Could not initialize FreeType library" << '\n';
      throw std::runtime_error( "freetype" );
    }
  }
  ~Text()
  {
    for ( const auto& [k, v] : faces ) {
      FT_Done_Face( v );
    }
    FT_Done_FreeType( library );
  }
  void AddChar( u64 char_code, i64 char_width, i64 char_height )
  {
    if ( faces.contains( char_code ) )
      return;
    FT_Face face;
    if ( FT_New_Face( library, font_path.data(), 0, &face ) ) {
      std::cerr << "Could not open font" << '\n';
      FT_Done_FreeType( library );
      return;
    }
    FT_Set_Char_Size( face, char_width, char_height, dpiX, dpiY );
    if ( FT_Load_Char( face, char_code, FT_LOAD_RENDER ) ) {
      std::cerr << "Could not load character" << char_code << '\n';
      FT_Done_Face( face );
      FT_Done_FreeType( library );
      return;
    }
    maxcharheight
      = maxcharheight > face->glyph->bitmap.rows ? maxcharheight : face->glyph->bitmap.rows;
    maxcharwidth
      = maxcharwidth > face->glyph->bitmap.width ? maxcharwidth : face->glyph->bitmap.width;
    faces.insert( std::make_pair( char_code, face ) );
  }

  FT_Bitmap* LoadCharBitmap( u64 char_code ) const
  {
    return &( faces.at( char_code )->glyph->bitmap );
  }
  u32 getMaxcharheight() const { return maxcharheight; }
  u32 getMaxcharwidth() const { return maxcharwidth; }
};
}

#endif