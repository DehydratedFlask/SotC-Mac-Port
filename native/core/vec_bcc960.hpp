// Native recovery of original ctor RVA 0xbcc960 (index 251; vector-copy manifest position 183;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-22. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e04e0 into BSS @0x3351130. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBCC960Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBCC960 = std::array<std::uint32_t, 4>;

void init_vec_bcc960(VecBCC960& g) noexcept;

}  // namespace sotc
