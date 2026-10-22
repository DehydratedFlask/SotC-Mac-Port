// Native recovery of four original constructors that only store name pointers and their hash:
//   0x669450 (1 record, cells 0x3012638),  0x842450 (3 records, 0x32936c8),
//   0x7dace0 (2 records, 0x325efb0),       0x6dde20 (3 records, 0x3240c68)
// (ELF sha256 d0723afb...5861d3). Each body is call-only straight-line code: for every record it
// stores the image address of a NUL-terminated name string at +0x00, calls the helper 0x52a990
// (sotc::fnv_cstr_52a990) on that string and stores the 32-bit result at +0x08. Records are 16 bytes
// apart; the 4 bytes at +0x0c are never written. There are no guards and no other state.
//
// The original string addresses are image addresses with no meaning in a native process and the
// strings are original game content, so they are NOT embedded: the caller supplies the name
// pointers, the cell stores the pointer actually given and the hash is computed from that string.
// Names are ours; the original has no symbols. What the records mean is not analysed.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

struct NameHashCell {
    const char* name;           // +0x00  original: address of the name string (8 bytes)
    std::uint32_t hash;         // +0x08  original: result of 0x52a990(name)
    std::uint32_t unwritten;    // +0x0c  never written by any of the four constructors
};
static_assert(sizeof(NameHashCell) == 16 && offsetof(NameHashCell, name) == 0 &&
              offsetof(NameHashCell, hash) == 8 && offsetof(NameHashCell, unwritten) == 12);

// Unguarded: each call rewrites every covered field (name, hash) and leaves `unwritten` alone.
void init_ctor_669450(std::array<NameHashCell, 1>& cells, const std::array<const char*, 1>& names) noexcept;
void init_ctor_842450(std::array<NameHashCell, 3>& cells, const std::array<const char*, 3>& names) noexcept;
void init_ctor_7dace0(std::array<NameHashCell, 2>& cells, const std::array<const char*, 2>& names) noexcept;
void init_ctor_6dde20(std::array<NameHashCell, 3>& cells, const std::array<const char*, 3>& names) noexcept;

}  // namespace sotc
