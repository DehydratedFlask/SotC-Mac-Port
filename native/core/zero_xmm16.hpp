// Native recovery of original ctor RVA 0x5777a0 (index 416; ELF sha256 d0723afb...5861d3), task CPU-01f.
// Thirteen-byte body: vxorps xmm0,xmm0,xmm0; vmovups [cell], xmm0; ret. Cell @0x2f295b8.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

using Xmm16 = std::array<std::uint32_t, 4>;

void zero_xmm16(Xmm16& cell) noexcept;

}  // namespace sotc
