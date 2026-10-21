// Native recovery of original ctor RVA 0xdd4210 (ctor index 66; ELF sha256
// d0723afb...5861d3). Same guarded-constant family as core/math_constants: six guarded float
// cells, a guarded {~0,0,0,0} lane mask and a guarded {~0,~0,~0,0} mask, a guarded u32 1, and
// then unguarded constant stores. Guard/value cells are shared with the sibling clones
// 0xdd91b0 / 0xdd7e30, so in the original only the first clone executed initialises them.
// Names are ours; the original has no symbols.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct GuardedDd4210 {
    // Guarded cells (original value/guard address pairs, in order):
    //   0x3351ef4/0x3351ef8  0x3351f00/0x3351f08  0x3351f10/0x3351f18
    //   0x3351f20/0x3351f28  0x3351f30/0x3351f38  0x3351f40/0x3351f48
    //   0x3351f50/0x3351f60  0x3351f70/0x3351f80  0x3351f88/0x3351f90
    float neg_one;                              std::uint64_t neg_one_guard;
    float half;                                 std::uint64_t half_guard;
    float two;                                  std::uint64_t two_guard;
    float pi;                                   std::uint64_t pi_guard;
    float epsilon;                              std::uint64_t epsilon_guard;
    float max_float;                            std::uint64_t max_float_guard;
    std::array<std::uint32_t, 4> lane0_mask;    std::uint64_t lane0_mask_guard;
    std::array<std::uint32_t, 4> xyz_mask;      std::uint64_t xyz_mask_guard;
    std::uint32_t one;                          std::uint64_t one_guard;

    // Unguarded tail: the original always rewrites 0x3352d30..0x3352db0
    // (128 bytes, gaps are BSS zero).
    std::array<std::uint8_t, 128> tail;
};

// Behaviour of 0xdd4210: initialise each guarded cell once, then rewrite the unguarded tail.
void init_guarded_dd4210(GuardedDd4210& g) noexcept;

}  // namespace sotc
