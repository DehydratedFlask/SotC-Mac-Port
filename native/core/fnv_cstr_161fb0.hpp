// Native recovery of original helper RVA 0x52a990 and the routine it tail-jumps to, RVA 0x161fb0
// (ELF sha256 d0723afb...5861d3). Both are ordinary leaf functions: no calls, no indirect branches,
// no imports, no globals; they only read the caller's NUL-terminated string.
//
//   0x52a990 (10 bytes):  mov esi, 0x811c9dc5 ; jmp 0x161fb0         -> fnv_cstr_52a990(s)
//   0x161fb0 (57 bytes):  rdi = string, esi = seed; returns eax
//       if (s == NULL) return 0;                 // `xor eax,eax; test rdi,rdi; je ret`
//       h = seed; for each byte c until NUL: h = (h ^ (int32)(int8)c) * 0x01000193
//       (an empty string returns the seed unchanged)
//
// The byte is SIGN-extended (`movsx ecx, cl`), so bytes >= 0x80 xor as 0xffffffNN. That makes it
// differ from sotc::fnv1a32 (save_checksum, zero-extended) for non-ASCII input, and it is a different
// function from the djb2-style sotc::name_hash (RVA 0xe70aa0). Names are ours; the original has no
// symbols. What the hash is used for downstream is not analysed here.
#pragma once
#include <cstdint>

namespace sotc {

inline constexpr std::uint32_t kFnvOffsetBasis52a990 = 0x811c9dc5u;

// Original RVA 0x161fb0: seeded variant (null -> 0, otherwise FNV-1a with sign-extended bytes).
[[nodiscard]] std::uint32_t fnv_cstr_161fb0(const char* s, std::uint32_t seed) noexcept;

// Original RVA 0x52a990: the same with the FNV offset basis as seed.
[[nodiscard]] std::uint32_t fnv_cstr_52a990(const char* s) noexcept;

}  // namespace sotc
