// Native lift of `0xf1a0` — `SharedStr::SharedStr(const char*)` (E581).
//
// Body `0xf1a0..0xf237` (151 B, sha256 `954762ab…08946`), exact protocol:
//   size = strlen(src)                                  ; holder+0x08
//   alloc = (size + 4) & ~3                             ; f1be..f1cc
//   capacity = alloc - 1                                ; holder+0x10
//   data = 0x12820(alloc, tag 0, align 8, flags 0)      ; holder+0x00
//   memcpy(data, src, size); data[size] = 0
//   control = 0x17070(8) = 0x12820(8, tag 1, 8, 1)     ; holder+0x18
//   *control = 1 (xchg, atomic)
// Empty source: the existing `empty_string_f730()` static (data = 0x2dd9f78, the shared zero
// byte), size = capacity = control = 0.
//
// The 0x20 B holder is the existing `SharedStr`; its `aux` field is proven to be capacity (the
// 0xf3c0 resize compares it with the requested length). The release is the existing 73 B
// `shared_release_f590`, unchanged. The allocator bodies are hooks; a null allocator or a failed
// allocation returns false without a fake success. A null `src` is treated as empty (the original
// would fault).
//
// NOT modelled: `0xf3c0` resize/COW, `0x14c2f0` (the `****` rewrite + FNV hash), and the guest
// `operator new` bodies. G1 PARTIAL.
#pragma once

#include <cstddef>

#include "alloc_wrappers_17070.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSharedStrCtorF1a0Rva = 0xF1A0;
inline constexpr std::size_t kSharedStrCtorF1a0Size = 151;
inline constexpr std::uint64_t kSharedStrCtorF1a0EndRva = 0xF237;
inline constexpr std::uint64_t kSharedStrResizeF3c0Rva = 0xF3C0;
inline constexpr std::size_t kSharedStrResizeF3c0Size = 459;
inline constexpr std::uint64_t kSharedStrDataAllocRva = 0x12820;
inline constexpr std::uint64_t kSharedStrControlAllocRva = 0x17070;

struct SharedStrCtorHooks {
    // 0x12820: data uses tag 0 / align 8 / flags 0; the control block uses tag 1 / align 8 / flags 1.
    AllocDispatchContract alloc{};
};

// Builds `out` from `src`. Returns false when the allocator is missing or either allocation fails;
// on a control-block failure the data buffer is returned through `alloc.deallocate` when available.
[[nodiscard]] bool init_shared_str_f1a0(SharedStr& out, const char* src,
                                        const SharedStrCtorHooks& hooks) noexcept;

}  // namespace sotc
