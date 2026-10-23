// Native lift of `0x14c2f0` — ResourceId/HashedStr construction (E582).
//
// Body `0x14c2f0..0x14c396` (166 B, sha256 `548a3850…3bcd6b`):
//   key.str = SharedStr(literal)                 ; 0xf1a0
//   found = strstr(key.str.data, "****")
//   if (found) {
//     fourcc = movbe dword [data + size - 4]     ; byte-swapped last four bytes
//     tag    = 0x147e20(*0x2ee2d00, fourcc)      ; runtime platform-tag lookup
//     memcpy(found, tag, 4)                      ; patch the placeholder in place
//   }
//   key.hash = 0x161f80(&key.str, 0x811c9dc5)    ; FNV-1a-32 over the patched text
//
// The hash is therefore over the runtime-patched string, not the literal. `key+0x04..+0x07` is
// never written; the native model zeroes it. The platform lookup is a hook: a literal containing
// `****` without a lookup is a visible failure and allocates nothing.
//
// NOT modelled: `0x147e20`'s tag table, `0x2ee2d00`, and the guest allocator bodies. G1 PARTIAL.
#pragma once

#include <cstdint>

#include "hashed_string_14c3a0.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kResourceIdCtor14c2f0Rva = 0x14C2F0;
inline constexpr std::size_t kResourceIdCtor14c2f0Size = 166;
inline constexpr std::uint64_t kResourceIdCtor14c2f0EndRva = 0x14C396;
inline constexpr std::uint64_t kPlatformTagLookupRva = 0x147E20;

struct ResourceIdCtorHooks {
    SharedStrCtorHooks str{};
    // 0x147e20(*0x2ee2d00, fourcc) -> 4-byte platform tag. Null when the literal contains "****"
    // is a visible failure (the original would call the table).
    std::uint32_t (*platform_tag)(std::uint32_t fourcc) = nullptr;
};

// Builds `out` from `src`. The string is patched before hashing. Returns false without allocating
// when a "****" placeholder is present but `platform_tag` is null.
[[nodiscard]] bool resource_id_ctor_14c2f0(HashedStr& out, const char* src,
                                           const ResourceIdCtorHooks& hooks) noexcept;

}  // namespace sotc
