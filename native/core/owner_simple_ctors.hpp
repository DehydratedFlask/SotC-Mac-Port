// Native lifts of the three trivial `0x2569d0` constructor bodies (E580).
//
// Recovered from E570 §1.4/§1.5:
//   - `0xa1cc0` (1 B, `c3`): a no-op constructor (row 7's 1-byte object).
//   - `0xfafb0` (92 B): row 6's 0x82028-byte object. Sets the vptr at +0x00, zeroes +0x08..+0x2007,
//     stores +0x2008 = obj+0x2028, +0x2010 = obj+0x82028, +0x2018 = obj+0x82028, then zeroes
//     +0x2028..+0x82027. The original leaves +0x2020..+0x2027 untouched; the native model zeroes
//     them for determinism (the object is freshly allocated, so the bytes are unobservable).
//   - `0x26f820` (57 B): row 16's 0x4028-byte object. Calls the base ctor `0x3eba0`, sets the vptr
//     at +0x00, zeroes +0x18, then tail-calls the sub-object ctor `0x3b310` at obj+0x4020.
//
// The vptr is a host value: the original stores `0x1597ca8` / `0x159c9d8`, which a native object
// must not use as a callable address. The base/sub ctors are hooks; a missing hook is a visible
// failure. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerA1cc0Rva = 0xA1CC0;
inline constexpr std::size_t kOwnerA1cc0Size = 1;
inline constexpr std::uint64_t kOwnerFafb0Rva = 0xFAFB0;
inline constexpr std::size_t kOwnerFafb0Size = 92;
inline constexpr std::size_t kOwnerFafb0ObjectSize = 0x82028;
inline constexpr std::size_t kOwnerFafb0FirstFill = 0x2000;
inline constexpr std::size_t kOwnerFafb0SecondFill = 0x80000;
inline constexpr std::uint64_t kOwner26f820Rva = 0x26F820;
inline constexpr std::size_t kOwner26f820Size = 57;
inline constexpr std::size_t kOwner26f820ObjectSize = 0x4028;
inline constexpr std::size_t kOwner26f820SubOffset = 0x4020;

// Row 7: the recovered body is a bare `ret`.
void init_owner_a1cc0(void* object) noexcept;

// Row 6: exact field/zero-fill layout. `vptr` is a host pointer, not the guest address.
void init_owner_fafb0(std::byte* object, void* vptr) noexcept;

struct Owner26f820Hooks {
    // 0x3eba0(this). Null or false => visible failure.
    bool (*base_ctor)(void* object) = nullptr;
    // 0x3b310(this+0x4020). Null or false => visible failure.
    bool (*sub_ctor)(void* object) = nullptr;
};

// Row 16: base ctor, vptr, +0x18 = 0, then the sub-object ctor at +0x4020.
[[nodiscard]] bool init_owner_26f820(std::byte* object, void* vptr,
                                     const Owner26f820Hooks& hooks) noexcept;

}  // namespace sotc
