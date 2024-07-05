#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace vastina {

using std::string;
using std::string_view;
using std::vector;
using u64 = std::uint64_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using u8 = unsigned char;

#define GSetter( member )                                                                                          \
  decltype( member )& get##member()                                                                                \
  {                                                                                                                \
    return member;                                                                                                 \
  }                                                                                                                \
  void set##member( const decltype( member )& _##member )                                                          \
  {                                                                                                                \
    (member) = _##member;                                                                                            \
  }

}

#endif