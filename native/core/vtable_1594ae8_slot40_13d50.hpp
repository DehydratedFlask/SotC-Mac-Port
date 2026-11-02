// Vtable slot resolved by the vtable-closure sweep: `0x1594ae8 + 0x40 -> 0x13d50`.
//
// Evidence:
//   * `0x1149d` is `call *0x40(%rax)` with `%rax` loaded from the RIP-relative table `0x1594ae8`;
//   * the qword at `0x1594ae8 + 0x40` in the pinned image is `0x13d50`;
//   * the dynamic relocation table has one type-8 row at `0x1594b28` with addend `0x13d50`;
//   * `0x13d50` is an FDE function start with length `0x3`.
//
// Body (3 B, `31 c0 c3`): `xor %eax, %eax; retq` — an unconditional zero return.
// No register is read, nothing is stored, and the result does not depend on `this`. The body is
// modelled as the constant it computes, not as a hook.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1594ae8Slot40_13d50Rva = 0x13d50;
inline constexpr std::size_t kVtable1594ae8Slot40_13d50Size = 0x3;
inline constexpr std::uint64_t kVtable1594ae8Slot40_13d50EndAddress = 0x13d53;
inline constexpr std::uint64_t kVtable1594ae8Slot40_13d50VtableAddress = 0x1594ae8;
inline constexpr std::uint32_t kVtable1594ae8Slot40_13d50SlotOffset = 0x40;
inline constexpr std::uint64_t kVtable1594ae8Slot40_13d50RelocAddress = 0x1594b28;

struct Vtable1594ae8Slot40_13d50Result {
    bool ok = false;
    std::int32_t code = 0;  // eax: the constant zero the body returns
};

// Returns `eax = 0` on every input; `this` is never dereferenced.
[[nodiscard]] Vtable1594ae8Slot40_13d50Result vtable_1594ae8_slot40_13d50() noexcept;

}  // namespace sotc
