#ifndef _FT_FREETYPE_CXX_H_
#define _FT_FREETYPE_CXX_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <unordered_map>

#include "vasdef.hpp"
#include "util.hpp"
#include "DrawBoard.hpp"

namespace vas {

class Text
{
private:
  FT_Library library;
  std::unordered_map<u64, FT_Face> faces;
  std::string font_path;
  u32 maxcharheight { 0u };

public:
  static inline std::vector<uint32_t> utf8_to_utf32( const std::string& utf8_str )
  {
    std::vector<uint32_t> utf32_str;
    auto it = utf8_str.begin();
    while ( it != utf8_str.end() ) {
      utf32_str.push_back( utf8::next( it, utf8_str.end() ) );
    }
    return utf32_str;
  }
  static inline std::string utf32_to_utf8( const std::vector<uint32_t>& utf32_str )
  {
    std::string utf8_str;
    utf8::utf32to8( utf32_str.begin(), utf32_str.end(), back_inserter( utf8_str ) );
    return utf8_str;
  }

  Text( std::string ttfpath = "KAISG.ttf" )
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
    faces.insert( std::make_pair( char_code, face ) );
  }

  FT_Bitmap* LoadCharBitmap( u64 char_code ) const
  {
    return &( faces.at( char_code )->glyph->bitmap );
  }
  u32 getMaxcharheight() const { return maxcharheight; }
};

struct typeSetter
{
  u32 left { 0 };
  u32 top { 0 };
  u32 width { 0 };
  u32 height { 0 };
  struct Config
  {
    u32 xgap { 7 };
    u32 ygap { 0 };
    i32 char_width { 0 };
    i32 char_height { 32 * 48 };
    u32 draw_start_x { 0u };
    u32 draw_start_y { 0u };
    // ...
  } config {};
  RGB background { 0x22, 0x22, 0x22 };
  std::vector<u32> content;
  //  std::string content;
  bool cache_avaliable { false };
  std::vector<pos> poscache;
  DrawBoard& b;
  Text charConfig;

  // Deng.ttf, 等线, form C:\\Windows\\Fonts
  typeSetter( const std::string& text,
              vas::DrawBoard& board,
              typeSetter::Config Config = { 7, 0, 0, 32 * 48, 0, 0 },
              const std::string& FontPath = "Deng.ttf" )
    : config( Config )
    , content { Text::utf8_to_utf32( text ) }
    , b { board }
    , charConfig { FontPath }
  {}

  void setRect( int l, int t, int w, int h )
  {
    left = l;
    top = t;
    width = w;
    height = h;
    cache_avaliable = false;
  }
  // calculate height width and char_pos
  void calculateContent()
  {
    const auto length { content.size() };
    u32 w_current { 0u };
    u32 w_max { 0u };
    u32 y_offset { 0u };
    for ( u64 i = 0; i < length; i++ ) {
      if ( content[i] == '\n' ) {
        w_max = std::max( w_max, w_current );
        w_current = 0;
        y_offset += charConfig.getMaxcharheight() + config.ygap;
        continue;
      }
      if ( w_current > width ) {
        w_max = std::max( w_max, w_current );
        w_current = 0;
        y_offset += charConfig.getMaxcharheight() + config.ygap;
      }
      const FT_Bitmap* bitmap { charConfig.LoadCharBitmap( content[i] ) };
      poscache[i] = { left + w_current, top + y_offset };
      w_current += bitmap->width + config.xgap;
    }
    w_max = std::max( w_max, w_current );
    height = y_offset + charConfig.getMaxcharheight() + config.ygap;
    width = w_max;
    cache_avaliable = true;
  }

  void LoadContent()
  {
    for ( const auto ch : content ) {
      charConfig.AddChar( ch, config.char_width, config.char_height );
    }
    poscache.resize( content.size() );
  }
  template<typename func> // a concept need here
  void ChangeContent( const func& change )
  {
    change( content );
    cache_avaliable = false;
    LoadContent();
  }

  void DrawContent( u64 start = 0u, u64 charnum = UINT32_MAX )
  {
    FillBackgroud();

    charnum = charnum > content.size() ? content.size() : charnum;
    // float rgbmax =std::max({r,g,b});
    // Vec3f scale {rgbmax/background.r, rgbmax/background.g, rgbmax/background.b};
    if ( !cache_avaliable ) {
      u32 w_current { config.draw_start_x };
      u32 y_offset { config.draw_start_y };
      for ( u64 i = start; i < start + charnum; i++ ) {
        if ( content[i] == '\r' )
          continue;
        if ( content[i] == '\n' ) {
          w_current = config.draw_start_x;
          y_offset += charConfig.getMaxcharheight() + config.ygap;
          continue;
        }
        if ( w_current > width ) {
          w_current = config.draw_start_x;
          y_offset += charConfig.getMaxcharheight() + config.ygap;
        }
        const FT_Bitmap* bitmap { charConfig.LoadCharBitmap( content[i] ) };

        poscache[i] = { left + w_current, top + y_offset };
        b.DrawChar( bitmap->width,
                    bitmap->rows,
                    bitmap->buffer,
                    left + w_current,
                    top + y_offset + ( charConfig.getMaxcharheight() - bitmap->rows ),
                    background );
        w_current += bitmap->width + config.xgap;
      }
      if ( charnum == content.size() )
        cache_avaliable = true;
    } else {
      for ( u64 i = start; i < start + charnum; i++ ) {
        if ( content[i] == '\n' || content[i] == '\r' ) {
          continue;
        }
        const FT_Bitmap* bitmap { charConfig.LoadCharBitmap( content[i] ) };

        b.DrawChar( bitmap->width,
                    bitmap->rows,
                    bitmap->buffer,
                    poscache[i].x,
                    poscache[i].y + ( charConfig.getMaxcharheight() - bitmap->rows ),
                    background );
      }
    }
  }
  void FillBackgroud() const
  {
    for ( u32 y = top; y <= top + height; y++ ) {
      b.pic.setLine( y, left, left + width, background );
    }
  }

  bool charinRect( u32 index, u32 x1, u32 y1, u32 x2, u32 y2 ) const
  {
    if ( !cache_avaliable )
      return false;
    auto* bitmap = charConfig.LoadCharBitmap( content[index] );
    u32 left_top_x = std::min( x1, x2 );
    u32 left_top_y = std::min( y1, y2 );
    u32 right_bottom_x = std::max( x1, x2 );
    u32 right_bottom_y = std::max( y1, y2 );
    return ( left_top_x <= poscache.at( index ).x ) && ( left_top_y <= poscache.at( index ).y )
           && ( right_bottom_x >= poscache.at( index ).x + bitmap->width )
           && ( right_bottom_y >= poscache.at( index ).y + bitmap->rows );
  }
};

struct DomNode
{
  typeSetter* ts { nullptr };
  std::vector<DomNode*> children;

  static inline void DeleteTree( DomNode* root )
  {
    for ( auto& child : root->children ) {
      DeleteTree( child );
    }
    delete root->ts;
    delete root;
  }
};
class DomTree
{
private:
  DomNode* root { nullptr };

public:
  DomTree() : root { new DomNode { nullptr, {} } } {}
  ~DomTree()
  {
    if ( root )
      DomNode::DeleteTree( root );
  }

private:
  void LoadAll( DomNode* node )
  {
    if ( node->ts )
      node->ts->LoadContent();
    for ( const auto& child : node->children ) {
      LoadAll( child );
    }
  }

public:
  void LoadAll() { LoadAll( root ); }
};

};

#endif