// Native lift of the 18-byte remarshal `0x1bde30` that tail-calls the list-3 insert.
//
// ELF sha256 d0723afb…861d3. Bytes `0x1bde30..0x1bde42`:
//   48 89 f0        mov rax,rsi
//   48 83 c7 40     add rdi,0x40
//   48 89 fe        mov rsi,rdi
//   48 89 c2        mov rdx,rax
//   e9 6e 10 00 00  jmp 0x1beeb0
// Entry (from 0x1290): rdi = the owner/container base, rsi = the 0x50 B descriptor.
// Net: the insert is entered with rdi = rsi = &container[0x40] (the list-3 sentinel) and
// rdx = &descriptor. There is no other effect, no dereference and no null check.
//
// NOT modelled: nothing in this function; the insert body it tail-calls lives in
// `insert_1beeb0` and the owner object is still created elsewhere (0x533100/0x2569d0).
#pragma once

#include <cstddef>
#include <cstdint>

#include "insert_1beeb0.hpp"

namespace sotc {

inline constexpr std::uint64_t kInsert1bde30Rva = 0x1BDE30;
inline constexpr std::size_t kInsert1bde30Size = 18;
inline constexpr std::uint64_t kInsert1bde30EndRva = 0x1BDE42;
inline constexpr std::uint64_t kInsert1bde30TailTargetRva = 0x1BEEB0;

// Runs the remarshal and the tail-called insert. `container` is the owner object; the sentinel
// is `&container.list3_prev`, exactly what `add rdi,0x40` computes.
[[nodiscard]] Insert1beeb0Result insert_1bde30(Container1beeb0& container,
                                               const Descriptor1beeb0& descriptor,
                                               const Insert1beeb0Alloc& alloc) noexcept;

}  // namespace sotc
