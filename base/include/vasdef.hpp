#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

using u64 = std::uint64_t;
using i64 = std::int64_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using u8 = unsigned char;

namespace vas {

using std::string;
using std::string_view;
using std::vector;

#define GSetter( member )                                                                          \
  decltype( member )& get##member()                                                                \
  {                                                                                                \
    return member;                                                                                 \
  }                                                                                                \
  void set##member( const decltype( member )& _##member )                                          \
  {                                                                                                \
    ( member ) = _##member;                                                                        \
  }

struct RGB
{
  u8 r;
  u8 g;
  u8 b;
};

struct Vec3f
{
  float x;
  float y;
  float z;
};

}

#endif