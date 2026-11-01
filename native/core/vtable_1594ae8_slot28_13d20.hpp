// Vtable slot resolved by the indirect-frontier card: `0x1594ae8 + 0x28 -> 0x13d20`.
//
// Evidence:
//   * `0x13620` is `call *0x28(%rax)` with `%rax` loaded from the RIP-relative table `0x1594ae8`;
//   * the qword at `0x1594ae8 + 0x28` in the pinned image is `0x13d20`;
//   * the dynamic relocation table has one type-8 row at `0x1594b10` with addend `0x13d20`;
//   * `0x13d20` is an FDE function start with length `0x4`.
//
// Body (4 B, `89 77 1c c3`): `mov %esi, 0x1c(%rdi); retq` — a one-field setter.
// Signature inferred from the body only: `void f(This* rdi, std::uint32_t value esi)`.
// No other register is read, nothing is returned, and the store is unconditional.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1594ae8Slot28_13d20Rva = 0x13d20;
inline constexpr std::size_t kVtable1594ae8Slot28_13d20Size = 0x4;
inline constexpr std::uint64_t kVtable1594ae8Slot28_13d20EndAddress = 0x13d24;
inline constexpr std::uint64_t kVtable1594ae8Slot28_13d20VtableAddress = 0x1594ae8;
inline constexpr std::uint32_t kVtable1594ae8Slot28_13d20SlotOffset = 0x28;
inline constexpr std::uint64_t kVtable1594ae8Slot28_13d20RelocAddress = 0x1594b10;
inline constexpr std::uint64_t kVtable1594ae8Slot28_13d20FieldOffset = 0x1c;

struct Vtable1594ae8Slot28_13d20Cells {
    // Host storage for the object's `+0x1c` dword. Null is fail-visible.
    std::uint32_t* field = nullptr;
};

struct Vtable1594ae8Slot28_13d20Result {
    bool ok = false;
    std::uint32_t previous = 0;
    std::uint32_t written = 0;
};

[[nodiscard]] Vtable1594ae8Slot28_13d20Result vtable_1594ae8_slot28_13d20(
    std::uint32_t value, const Vtable1594ae8Slot28_13d20Cells& cells = {}) noexcept;

}  // namespace sotc
