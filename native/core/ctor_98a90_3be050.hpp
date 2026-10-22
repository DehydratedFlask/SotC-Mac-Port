// Heavy ctor `0x3be050` for the 0x98a90 object installed at app+0x3b58 (prologue tail).
//
// Full OBS body: 0x3be050..0x3be2c2 (627 B; sha f186db1c…). Next symbol at 0x3be2d0.
//
// Lifted (tip AllocatorFront):
//   Header OBS 0x3be050..0x3be0b6 (102 B): nested 0x48 sentinel + field zeros.
//   Ensure gate: ([+0x38]-[+0x28])>>3; if <=0xff hook 0x3c2f70(obj+0x20, 0x100).
//   Buffer grow OBS 0x3be0f8..0x3be1cb: if ([+0x58]-[+0x48])>>5 <= 3, alloc 0x80,
//     migrate 0x20-stride elems (elem dtor hook 0xf590), delete_ old, install
//     begin/end/cap at +0x48/+0x50/+0x58 (cap = begin+0x80).
//
// Hooks (bodies NOT modelled):
//   0x3be930(obj+0x68); 0x323d0(obj+0x98a88); 0x3c2f70; 0xf590;
//   list walk from 0x3be1ce (globals / 0xf8f500 / 0x3c0840).
//
// No leaf-vec; not idx 95; PR #5 cite-only.
#pragma once

#include "allocator_front.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kCtor98a90_3be050Rva = 0x3be050;
inline constexpr std::size_t kCtor98a90_3be050Size = 627;
inline constexpr std::size_t kCtor98a90_3be050HeaderSize = 102;
inline constexpr std::size_t kCtor98a90_3be050BufGrowSize = 212;  // 0x3be0f8..0x3be1cb incl.
inline constexpr std::size_t kCtor98a90NestedBytes = 0x48;
inline constexpr std::size_t kCtor98a90BufBytes = 0x80;
inline constexpr std::size_t kCtor98a90ElemBytes = 0x20;
inline constexpr std::size_t kCtor98a90TscOffset = 0x98a88;
inline constexpr std::size_t kCtor98a90VecBegin = 0x48;
inline constexpr std::size_t kCtor98a90VecEnd = 0x50;
inline constexpr std::size_t kCtor98a90VecCap = 0x58;

struct Ctor98a90_3be050Hooks {
    void (*sub_3be930)(void* at_plus_68) = nullptr;
    void (*store_tsc_323d0)(void* at_plus_98a88) = nullptr;
    void (*ensure_3c2f70)(void* at_plus_20, std::uint32_t n) = nullptr;
    void (*elem_dtor_f590)(void* elem) = nullptr;           // migrate path
    void (*list_walk_from_3be1ce)(void* obj) = nullptr;     // null → no-op
};

struct Ctor98a90_3be050Result {
    bool ok = false;
    void* nested_48 = nullptr;
    bool ensure_called = false;
    bool buf_grown = false;
    void* buf_80 = nullptr;
};

[[nodiscard]] Ctor98a90_3be050Result init_98a90_3be050(AllocatorFront& alloc, void* obj,
                                                       const Ctor98a90_3be050Hooks& hooks);

}  // namespace sotc
