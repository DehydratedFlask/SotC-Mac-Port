// Native recovery of original helper RVA 0x14ccb0 (ELF sha256 d0723afb...5861d3), task CPU-01.
// Five-byte leaf `lock inc dword [rdi+0x30]; ret` (f0 ff 47 30 c3): a sequentially consistent
// increment of the int32 at byte offset 0x30 of the object passed in rdi. It has no return value
// (rax is not written), makes no call and touches no other memory. 819 direct call sites exist
// (two inside startup ctors 0xb25470 and 0x7ec4c0); the meaning of the field is not recovered here,
// only the operation, so the name says "add_ref" after the shape and is not a semantic claim.
#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kAddRef14ccb0Offset = 0x30;

// Increment the counter cell itself (seq_cst, wraps modulo 2^32 like the locked add).
void add_ref_14ccb0(std::atomic<std::int32_t>& field) noexcept;

// Original calling convention: `object` is the base address; the cell is the int32 at +0x30, which
// must be 4-byte aligned (an unaligned `lock inc` would be a split lock on x86 and is not modelled).
// Only those four bytes are read or written.
void add_ref_14ccb0(void* object) noexcept;

}  // namespace sotc
