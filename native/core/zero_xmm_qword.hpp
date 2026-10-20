// Native recovery of original ctor RVA 0x58bd10 (index 412; ELF sha256 d0723afb...5861d3), task CPU-01g.
// Body: vxorps xmm0; vmovups [xmm_cell], xmm0; mov qword [q_cell], 0; ret.
// Cells OBS: xmm @0x2f4e4b0, qword @0x2f4e4d0 (not adjacent — 16-byte gap).
#pragma once
#include <cstdint>

#include "zero_xmm16.hpp"

namespace sotc {

void zero_xmm_qword(Xmm16& xmm_cell, std::uint64_t& q_cell) noexcept;

}  // namespace sotc
