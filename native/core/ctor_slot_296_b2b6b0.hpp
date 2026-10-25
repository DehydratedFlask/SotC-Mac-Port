// Native recovery of original constructor-table index 296 / RVA 0xb2b6b0 (ELF sha256
// d0723afb...5861d3). The original is a 12-byte static-init tail thunk:
//   lea  rdi,[0x334ac18]      ; 48 8d 3d 61 f5 81 02
//   jmp  0x323d0              ; e9 14 6d 50 ff
// The callee is the already-lifted TSC store (core/store_tsc_323d0). This module models the THUNK
// only and reuses that module; it does not reimplement the timer. The constructed object is one
// 8-byte stamp at original .bss 0x334ac18 (no vptr, no padding).
//
// Reachability (offline static evidence): the object is written by this slot and by the debug TU at
// 0xb254f0..0xb25837, and its only reader is 0xb25690, which has no references in the image. The
// slot is therefore startup-contract completeness, not a gameplay path.
//
// Frequency half: 0xf8f860 (PLT, NID 1j3S3n-tTW4, sceKernelGetTscFrequency) is not modelled. When
// the host hook is absent or returns 0, the callee keeps its documented non-zero sentinel 1 and the
// result reports frequency_from_hook == false. That is a visible PARTIAL, not a hard failure, and
// the stamp is still written (the slot's only required effect).
#pragma once
#include <cstddef>
#include <cstdint>

#include "store_tsc_323d0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot296ThunkRva = 0xb2b6b0;
inline constexpr std::size_t kSlot296ThunkSize = 12;
inline constexpr std::uint64_t kSlot296ObjectRva = 0x334ac18;   // 8-byte .bss stamp
inline constexpr std::uint64_t kSlot296CalleeRva = 0x323d0;     // store_tsc_323d0

// Original object layout: exactly one u64. No vptr, no padding.
struct Slot296Stamp {
    std::uint64_t tsc = 0;
};

struct Slot296Result {
    bool guard_was_set = false;         // [0x2ea4700] != 0 before the call
    bool frequency_from_hook = false;   // false => sentinel 1 / no usable frequency (PARTIAL)
    std::uint64_t frequency = 0;        // [0x2ea4700] after
    std::uint64_t base = 0;             // [0x2ea4708] after
    std::uint64_t stamp = 0;            // the object
};

// Models 0xb2b6b0: record the guard, run store_tsc_323d0(&g.tsc, hooks), read back both process
// cells and the object. Never fabricates a frequency.
[[nodiscard]] Slot296Result init_slot296_b2b6b0(Slot296Stamp& g,
                                               const StoreTsc323d0Hooks& hooks = {}) noexcept;

}  // namespace sotc
