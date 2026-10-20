// Native recovery of original function RVA 0xf6cad0 (ELF sha256 d0723afb...5861d3), task CPU-01.
// Names are ours (inferred from the stored IEEE values); the original has no symbols.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

// Storage the original function wrote through rip-relative globals. `*_guard` mirrors the
// compiler-emitted one-time-init flags: a constant is written only while its guard is zero.
struct MathConstants {
    float neg_one;   uint64_t neg_one_guard;
    float half;      uint64_t half_guard;
    float two;       uint64_t two_guard;
    float pi;        uint64_t pi_guard;
    float epsilon;   uint64_t epsilon_guard;
    float max_float; uint64_t max_float_guard;
    std::array<uint32_t, 4> lane0_mask;  uint64_t lane0_mask_guard;   // {~0, 0, 0, 0}
    std::array<uint32_t, 4> xyz_mask;    uint64_t xyz_mask_guard;     // {~0, ~0, ~0, 0}
    std::array<uint32_t, 4> xyz_mask_copy;                            // unguarded copy, always rewritten
};

// Behaviour of 0xf6cad0: initialise each guarded constant once, then overwrite the unguarded copy.
void init_math_constants(MathConstants& g) noexcept;

}  // namespace sotc
