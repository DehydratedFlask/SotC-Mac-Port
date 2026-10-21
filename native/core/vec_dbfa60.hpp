// Native recovery of original ctor RVA 0xdbfa60 (index 70; vector-copy manifest position 18;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-21. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x130de20 into BSS @0x3352ca0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDBFA60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecDBFA60 = std::array<std::uint32_t, 4>;

void init_vec_dbfa60(VecDBFA60& g) noexcept;

}  // namespace sotc
