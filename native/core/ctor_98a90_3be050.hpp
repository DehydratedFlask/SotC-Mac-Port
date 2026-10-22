// Heavy ctor `0x3be050` for the 0x98a90 object installed at app+0x3b58 (prologue tail).
//
// Full OBS body: 0x3be050..0x3be2c2 (627 B; sha f186db1c…). Next symbol at 0x3be2d0.
//
// This slice lifts the header OBS 0x3be050..0x3be0b6 (102 B) via tip AllocatorFront:
//   vxor; zero [obj+8] xmmword; alloc 0x48 nested sentinel;
//   nested: [p]=[p+8]=[p+0x10]=p; byte[p+0x18]=1; byte[p+0x19]=1;
//   store nested at [obj+8]; byte[obj+0x18]=0;
//   zero [obj+0x28] xmm, [obj+0x38]=0, [obj+0x55] xmm, [obj+0x48] xmm.
// Then hooks:
//   call 0x3be930(obj+0x68) — 826 B; NOT modelled
//   call 0x323d0(obj+0x98a88) — timestamp/global; NOT modelled (hook)
//   capacity ([obj+0x38]-[obj+0x28])>>3; if <=0xff call 0x3c2f70(obj+0x20, 0x100)
//   remainder from 0x3be0f8 (0x80 path / trees) — NOT modelled (hook)
//
// No leaf-vec; not idx 95; PR #5 cite-only.
#pragma once

#include "allocator_front.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kCtor98a90_3be050Rva = 0x3be050;
inline constexpr std::size_t kCtor98a90_3be050Size = 627;       // through first ret
inline constexpr std::size_t kCtor98a90_3be050HeaderSize = 102;  // to call 0x3be930
inline constexpr std::size_t kCtor98a90NestedBytes = 0x48;
inline constexpr std::size_t kCtor98a90TscOffset = 0x98a88;

struct Ctor98a90_3be050Hooks {
    void (*sub_3be930)(void* at_plus_68) = nullptr;              // null → no-op
    void (*store_tsc_323d0)(void* at_plus_98a88) = nullptr;      // null → no-op
    void (*ensure_3c2f70)(void* at_plus_20, std::uint32_t n) = nullptr;
    void (*remainder_from_3be0f8)(void* obj) = nullptr;          // null → no-op
};

struct Ctor98a90_3be050Result {
    bool ok = false;
    void* nested_48 = nullptr;
    bool ensure_called = false;
};

// Header + hooked callees. Requires obj sized >= kPrologue98a90Bytes (caller zeros).
[[nodiscard]] Ctor98a90_3be050Result init_98a90_3be050(AllocatorFront& alloc, void* obj,
                                                       const Ctor98a90_3be050Hooks& hooks);

}  // namespace sotc
