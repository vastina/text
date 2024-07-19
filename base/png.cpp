#include "png.hpp"

#include <fstream>
#include <zlib.h>

namespace vas {
namespace png {

namespace {
inline u32 calculate_crc32( const std::vector<uint8_t>& data )
{
  return crc32(
    0L, reinterpret_cast<const Bytef*>( data.data() ), static_cast<u32>( data.size() ) );
}
}

void write_uint32( std::ofstream& file, uint32_t value )
{
  file.put( ( value >> 24 ) & 0xFF );
  file.put( ( value >> 16 ) & 0xFF );
  file.put( ( value >> 8 ) & 0xFF );
  file.put( value & 0xFF );
}

void write_chunk( std::ofstream& file, const string_view& type, const std::vector<uint8_t>& data )
{
  write_uint32( file, static_cast<u32>( data.size() ) );
  file.write( type.data(), 4 );
  file.write( reinterpret_cast<const char*>( data.data() ), data.size() );

  std::vector<uint8_t> crc_data( type.begin(), type.end() );
  crc_data.insert( crc_data.end(), data.begin(), data.end() );

  uint32_t crc = calculate_crc32( crc_data );
  write_uint32( file, crc );
}

void png::resize( u32 h, u32 w, u8 scale = 3 )
{
  height = h;
  width = w;
  data.reserve( h * w * scale );
}

void png::write()
{
  {
    for ( u32 y = 0; y < height; y++ )
      data[y * width * 3 + y] = config.filter_method;
  }

  if ( !filename.ends_with( ".png" ) )
    filename.append( ".png" );

  std::ofstream fs( filename, std::ios::binary | std::ios::app );
  {
    constexpr u8 png_signature[8] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
    fs.write( reinterpret_cast<const char*>( png_signature ), 8 );
  }
  {
    std::vector<uint8_t> ihdr( 13 );
    ihdr[0] = ( width >> 24 ) & 0xFF;
    ihdr[1] = ( width >> 16 ) & 0xFF;
    ihdr[2] = ( width >> 8 ) & 0xFF;
    ihdr[3] = width & 0xFF;
    ihdr[4] = ( height >> 24 ) & 0xFF;
    ihdr[5] = ( height >> 16 ) & 0xFF;
    ihdr[6] = ( height >> 8 ) & 0xFF;
    ihdr[7] = height & 0xFF;
    ihdr[8] = config.bit_depth;
    ihdr[9] = config.color_type;
    ihdr[10] = config.compression_method;
    ihdr[11] = config.filter_method;
    ihdr[12] = config.interlace_method;
    write_chunk( fs, "IHDR", ihdr );
  }
  {
    // todo, config
    std::vector<uint8_t> compressed_data( ::compressBound( static_cast<uLongf>( data.size() ) )
                                          + 4 );
    {
      constexpr char idat_signature[4] = { 'I', 'D', 'A', 'T' };
      compressed_data[0] = idat_signature[0];
      compressed_data[1] = idat_signature[1];
      compressed_data[2] = idat_signature[2];
      compressed_data[3] = idat_signature[3];
    }
    unsigned long compressed_size = static_cast<unsigned long>( compressed_data.size() - 4 );
    ::compress( compressed_data.data() + 4,
                &compressed_size,
                data.data(),
                static_cast<uLongf>( data.size() ) );
    compressed_data.resize( compressed_size + 4 );

    write_uint32( fs, compressed_size );
    fs.write( reinterpret_cast<const char*>( compressed_data.data() ), compressed_size + 4 );
    uint32_t crc = calculate_crc32( compressed_data );
    write_uint32( fs, crc );
  }
  {
    write_chunk( fs, "IEND", {} );
  }
  fs.close();
}

void png::DrawChar( const u32 width,
                    const u32 height,
                    const u8* buffer,
                    const u32 xoffst,
                    const u32 yoffst )
{
  for ( u32 y = 0; y < height; y++ ) {
    for ( u32 x = 0; x < width; x++ ) {
      auto val { buffer[y * width + x] };
      //      if(val > 16)
      setIndex( x + xoffst, y + yoffst, { val, val, val } );
    }
  }
}

}; // namespace png
}; // namespace vas