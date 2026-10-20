// Native recovery of original ctor RVA 0x24ebf0 (index 500; ELF sha256 d0723afb...5861d3), task CPU-01h.
// Body: lea rax, cell; vxorps xmm0; vmovlps [rax], xmm0; mov dword [rax+8], 0; ret.
// Clears 12 bytes at cell @0x2eebc48 (8 via vmovlps + 4 at +8).
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

using Bytes12 = std::array<std::uint8_t, 12>;

void zero_struct12(Bytes12& cell) noexcept;

}  // namespace sotc
