// Native recovery of original helpers RVA 0xa2ddc0 -> 0x162020 (ELF sha256 d0723afb...5861d3).
// Used by the Init-hash registry path (second name on input-table entries; also save chunk names).
//
//   0xa2ddc0 (10 bytes):  mov esi, 0x811c9dc5 ; jmp 0x162020     -> fnv_cstr_a2ddc0(s)
//   0x162020 (98 bytes):  rdi = string, esi = seed; returns eax
//       if (s == NULL) return 0;
//       h = seed;
//       for each byte c until NUL:
//         if c == 0xff: folded = 0xff;           // skips the import (cmpq $0xff)
//         else: folded = low byte of _Getptoupper()[c]   // PLT 0xf90180, NID rcQCUr0EaRU
//         if folded == '\\' (0x5c): folded = '/' (0x2f);
//         h = (h ^ folded) * 0x01000193;         // zero-extended path (movzbl), unlike 0x161fb0
//
// Native policy for _Getptoupper: ASCII C-locale toupper ('a'..'z' -> 'A'..'Z'); all other bytes
// hashed unchanged. This matches the documented SAV-01 / save_chunk ascii_upper_and_slash fixture.
// Locale-dependent or non-ASCII table behaviour is NOT claimed. Empty string returns the seed.
// Distinct from fnv_cstr_161fb0 (sign-extended, no fold) and fnv_span_161f80 (span, zero-ext, no fold).
#pragma once
#include <cstdint>

namespace sotc {

inline constexpr std::uint32_t kFnvOffsetBasisA2ddc0 = 0x811c9dc5u;
inline constexpr std::uint32_t kFnvPrime162020 = 0x01000193u;

// Original RVA 0x162020: seeded path-normalizing FNV-1a (null -> 0).
[[nodiscard]] std::uint32_t fnv_path_cstr_162020(const char* s, std::uint32_t seed) noexcept;

// Original RVA 0xa2ddc0: the same with the FNV offset basis as seed.
[[nodiscard]] std::uint32_t fnv_cstr_a2ddc0(const char* s) noexcept;

}  // namespace sotc
