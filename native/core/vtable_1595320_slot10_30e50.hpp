// Vtable slot resolved by the vtable-closure sweep: `0x1595320 + 0x10 -> 0x30e50`.
//
// Evidence:
//   * `0x30f56` is `call *0x10(%rax)` with `%rax` loaded from the RIP-relative table `0x1595320`;
//   * the qword at `0x1595320 + 0x10` in the pinned image is `0x30e50`;
//   * the dynamic relocation table has one type-8 row at `0x1595330` with addend `0x30e50`;
//   * `0x30e50` is an FDE function start with length `0x5`.
//
// Body (5 B, `e9 4b 04 00 00`): `jmp 0x312a0` — a pure tail thunk. It reads no field, stores
// nothing, and forwards `%rdi` unchanged. `0x312a0` is not lifted, so the forward is a hook; a
// missing hook is a visible failure instead of a silent success.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1595320Slot10_30e50Rva = 0x30e50;
inline constexpr std::size_t kVtable1595320Slot10_30e50Size = 0x5;
inline constexpr std::uint64_t kVtable1595320Slot10_30e50EndAddress = 0x30e55;
inline constexpr std::uint64_t kVtable1595320Slot10_30e50VtableAddress = 0x1595320;
inline constexpr std::uint32_t kVtable1595320Slot10_30e50SlotOffset = 0x10;
inline constexpr std::uint64_t kVtable1595320Slot10_30e50RelocAddress = 0x1595330;
inline constexpr std::uint64_t kVtable1595320Slot10_30e50TailTargetRva = 0x312a0;

struct Vtable1595320Slot10_30e50Hooks {
    // Stands in for the `jmp 0x312a0` tail target. Its body is out of scope.
    std::int32_t (*tail_312a0)(void* self) = nullptr;
};

struct Vtable1595320Slot10_30e50Result {
    bool ok = false;
    bool forwarded = false;
    std::int32_t code = 0;
};

[[nodiscard]] Vtable1595320Slot10_30e50Result vtable_1595320_slot10_30e50(
    void* self = nullptr, const Vtable1595320Slot10_30e50Hooks& hooks = {}) noexcept;

}  // namespace sotc
