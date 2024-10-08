#ifndef _VAS_PNG_H_
#define _VAS_PNG_H_

#include "vasdef.hpp"

namespace vas {
namespace png {

struct pngConfig
{
  u8 bit_depth { 8 };
  u8 color_type { 2 };
  u8 compression_method { 0 };
  u8 filter_method { 0 };
  u8 interlace_method { 0 };
};

struct png
{
  string filename;
  pngConfig config {};
  u32 width;
  u32 height;
  vector<u8> data;

public:
  png( const string& path, u32 w, u32 h, pngConfig cfig = {} )
    : filename( path ), config( cfig ), width( w ), height( h ), data( w * h * 3 + h )
  {}
  void resize( u32, u32, u8 );
  void GenRandomPNG( bool );
  void write();
  void read( const string_view& path );
  inline void setIndex( u32 x, u32 y, RGB rgb )
  {
    u32 index = ( y * width + x ) * 3 + y;
    data[index + 1] = rgb.r;
    data[index + 2] = rgb.g;
    data[index + 3] = rgb.b;
  }
  void DrawChar( u32, u32, const u8*, u32, u32 );
  template<typename func>
  void FillWith( func judge, RGB color )
  {
    for ( u32 y = 0; y < height; y++ ) {
      for ( u32 x = 0; x < width; x++ ) {
        if ( judge( x, y ) )
          setIndex( x, y, color );
      }
    }
  }

  GSetter( filename ) GSetter( width ) GSetter( height ) GSetter( config ) GSetter( data )
};

}; // namespace png
}; // namespace vas

#endif