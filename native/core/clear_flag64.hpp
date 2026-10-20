// Native recovery of original ctor RVA 0x3bd70 (index 560; ELF sha256 d0723afb...5861d3), task CPU-01e.
// Ten-byte body: xor eax,eax; xchg rax, [cell]; ret — seq_cst store of zero to a u64 cell.
// Distinct from Drive's 0x196c90 (u32 xchg @0x2ee4530): this uses REX.W xchg and cell @0x2ea4f20.
#pragma once
#include <atomic>
#include <cstdint>

namespace sotc {

// Stand-in for original cell @0x2ea4f20.
void clear_flag64(std::atomic<std::uint64_t>& cell) noexcept;

}  // namespace sotc
