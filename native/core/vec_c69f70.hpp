// Native recovery of original ctor RVA 0xc69f70 (index 191; vector-copy manifest position 127;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-11. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f0a30 into BSS @0x3351800. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC69f70Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC69f70 = std::array<std::uint32_t, 4>;

void init_vec_c69f70(VecC69f70& g) noexcept;

}  // namespace sotc
