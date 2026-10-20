// Native recovery of original ctor RVA 0x196c90 (index 520; ELF sha256 d0723afb...5861d3), task CPU-01.
// Nine-byte body: xor eax,eax; xchg [cell], eax; ret — seq_cst store of zero to a u32 cell.
#pragma once
#include <atomic>
#include <cstdint>

namespace sotc {

// Stand-in for original cell @0x2ee4530 (sole direct code ref found: this xchg).
void clear_flag32(std::atomic<std::uint32_t>& cell) noexcept;

}  // namespace sotc
