// Vtable slot resolved by the vtable-closure sweep: `0x15f2f08 + 0x88 -> 0xed4860`.
//
// Evidence:
//   * `0xed2c55` is `call *0x88(%rax)` with `%rax` loaded from the RIP-relative table `0x15f2f08`;
//   * the qword at `0x15f2f08 + 0x88` in the pinned image is `0xed4860`;
//   * the dynamic relocation table has one type-8 row at `0x15f2f90` with addend `0xed4860`;
//   * `0xed4860` is an FDE function start with length `0x5`.
//
// Body (5 B, `e9 4b 28 14 ff`): `jmp 0x170b0` — the tag-1 free tail with no pre-pass. There is no
// other instruction, so the model is exactly the free: no field is read and no vtable is touched.
// The free dispatch is the shared allocation contract; a null `this` is a no-op success there.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15f2f08Slot88_ed4860Rva = 0xed4860;
inline constexpr std::size_t kVtable15f2f08Slot88_ed4860Size = 0x5;
inline constexpr std::uint64_t kVtable15f2f08Slot88_ed4860EndAddress = 0xed4865;
inline constexpr std::uint64_t kVtable15f2f08Slot88_ed4860VtableAddress = 0x15f2f08;
inline constexpr std::uint32_t kVtable15f2f08Slot88_ed4860SlotOffset = 0x88;
inline constexpr std::uint64_t kVtable15f2f08Slot88_ed4860RelocAddress = 0x15f2f90;
inline constexpr std::uint64_t kVtable15f2f08Slot88_ed4860FreeTargetRva = 0x170b0;

struct Vtable15f2f08Slot88_ed4860Cells {
    void* self = nullptr;
};

struct Vtable15f2f08Slot88_ed4860Hooks {
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable15f2f08Slot88_ed4860Result {
    bool ok = false;
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable15f2f08Slot88_ed4860Result vtable_15f2f08_slot88_ed4860(
    const Vtable15f2f08Slot88_ed4860Cells& cells = {},
    const Vtable15f2f08Slot88_ed4860Hooks& hooks = {}) noexcept;

}  // namespace sotc
