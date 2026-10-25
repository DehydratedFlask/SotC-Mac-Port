// Native lift of original ctor RVA `0x1bdb10` — builds the 0x60 B three-list container that
// `insert_1beeb0` (E531) inserts into.
//
// ELF sha256 d0723afb…861d3. Body `0x1bdb10..0x1bdb48` = 56 B, fn sha256 056d7514…580a.
// Byte-verified effect order (root replay in tools/lift/check_f1bdb10.py):
//   lea rax,[rdi+0x20] ; lea rcx,[rdi+0x40]
//   [rdi+0x10] = 0 ; [rdi] = rdi ; [rdi+0x08] = rdi      (list-1 self loop, count 0)
//   [rdi+0x30] = 0 ; [rdi+0x20] = rax ; [rdi+0x28] = rax  (list-2 self loop, count 0)
//   [rdi+0x50] = 0 ; [rdi+0x40] = rcx ; [rdi+0x48] = rcx  (list-3 self loop, count 0)
//   ret
//
// NOT modelled: nothing beyond the stores — this ctor is complete. It does not touch
// +0x18/+0x38/+0x58 (observed: no stores), does not allocate, and takes no arguments.
// The owner that embeds this container is still constructed elsewhere (0x533100/0x2569d0).
#pragma once

#include <cstddef>
#include <cstdint>

#include "insert_1beeb0.hpp"

namespace sotc {

inline constexpr std::uint64_t kContainer1bdb10Rva = 0x1bdb10;
inline constexpr std::size_t kContainer1bdb10Size = 56;
inline constexpr std::uint64_t kContainer1bdb10EndRva = 0x1bdb48;
inline constexpr std::uint64_t kContainer1bdb10ShaPrefix = 0x056d7514;

// Builds the three self-referential lists and zeroes their counts. Any prior contents of the
// 0x60 B object are overwritten for the modelled fields; +0x18/+0x38/+0x58 are left untouched.
void init_container_1bdb10(Container1beeb0& container) noexcept;

}  // namespace sotc
