// Native recovery of original helper RVA 0x161f80 (ELF sha256 d0723afb...5861d3), task CPU-01.
// 38-byte leaf: no calls, no stack use, no globals; it only reads the {ptr, len} pair passed in rdi and
// the bytes it points at.
//
//   rdi -> { const uint8_t* ptr @+0, int64 len @+8 },  esi = seed,  result in eax
//     if (len <= 0 /* signed */) return seed;
//     for each of the len bytes (zero-extended: movzx) : h = (h ^ byte) * 0x01000193 (32-bit)
//
// Same FNV-1a family as the lifted 0x161fb0 (`fnv_cstr_161fb0`, NUL-terminated, SIGN-extended bytes,
// null -> 0) and equal to sotc::fnv1a32 for the same bytes. Names are ours; what the hash keys is not
// analysed here. The original loop ends on an unsigned `ptr < ptr + len` compare; a span whose end
// wraps the address space is not modelled (it cannot be a valid object).
#pragma once
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint32_t kFnvOffsetBasis161f80 = 0x811c9dc5u;
inline constexpr std::uint32_t kFnvPrime161f80 = 0x01000193u;

// The 16-byte {ptr, len} argument block of 0x161f80 (rdi points at it).
struct ByteSpan {
    const std::uint8_t* data;   // +0x00
    std::int64_t size;          // +0x08 (signed; <= 0 means "no bytes")
};
static_assert(sizeof(ByteSpan) == 16 && offsetof(ByteSpan, data) == 0 && offsetof(ByteSpan, size) == 8);

// Original RVA 0x161f80.
[[nodiscard]] std::uint32_t fnv_span_161f80(const ByteSpan& span, std::uint32_t seed) noexcept;

}  // namespace sotc
