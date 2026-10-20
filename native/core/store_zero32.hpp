// Native recovery of original ctor RVA 0xa3ade0 (index 315; ELF sha256 d0723afb...5861d3), task CPU-01f.
// Eleven-byte body: mov dword [cell], 0; ret. Cell @0x32f5980.
#pragma once
#include <cstdint>

namespace sotc {

void store_zero32(std::uint32_t& cell) noexcept;

}  // namespace sotc
