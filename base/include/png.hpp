#ifndef _VAS_PNG_H_
#define _VAS_PNG_H_

#include "vasdef.hpp"

namespace vastina {
namespace png {

extern "C++" void write_uint32( std::ofstream& file, uint32_t value );
extern "C++" void write_chunk( std::ofstream& file, const string_view& type, const std::vector<uint8_t>& data );

struct pngConfig
{
  u8 bit_depth { 8 };
  GSetter( bit_depth ) u8 color_type { 2 };
  GSetter( color_type ) u8 compression_method { 0 };
  GSetter( compression_method ) u8 filter_method { 0 };
  GSetter( filter_method ) u8 interlace_method { 0 };
  GSetter( interlace_method )
};

struct RGB
{
  u8 r;
  u8 g;
  u8 b;
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
    for ( unsigned y = 0; y < height; y++ ) {
      for ( unsigned x = 0; x < width; x++ ) {
        if ( judge( x, y ) )
          setIndex( x, y, color );
      }
    }
  }

  GSetter( filename ) GSetter( width ) GSetter( height ) GSetter( config ) GSetter( data )
};

}; // namespace png
}; // namespace vastina

#endif