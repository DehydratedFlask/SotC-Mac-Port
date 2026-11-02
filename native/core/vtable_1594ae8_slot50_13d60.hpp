// Vtable slot resolved by the vtable-closure sweep: `0x1594ae8 + 0x50 -> 0x13d60`.
//
// Evidence:
//   * `0x11462` is `call *0x50(%rax)` with `%rax` loaded from the RIP-relative table `0x1594ae8`;
//   * `0x113d6` reaches the same target through the table `0x1595b60` at slot `0x50`;
//   * the qword at `0x1594ae8 + 0x50` in the pinned image is `0x13d60`;
//   * the dynamic relocation table has one type-8 row at `0x1594b38` with addend `0x13d60`;
//   * `0x13d60` is an FDE function start with length `0x9`.
//
// Body (9 B, `48 8d 47 60 48 89 47 70 c3`):
//   `leaq 0x60(%rdi), %rax; movq %rax, 0x70(%rdi); retq`
// The function stores a pointer to its own `+0x60` sub-object into `+0x70` and returns it. It reads
// only `%rdi`; the store is unconditional and there is no other side effect.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1594ae8Slot50_13d60Rva = 0x13d60;
inline constexpr std::size_t kVtable1594ae8Slot50_13d60Size = 0x9;
inline constexpr std::uint64_t kVtable1594ae8Slot50_13d60EndAddress = 0x13d69;
inline constexpr std::uint64_t kVtable1594ae8Slot50_13d60VtableAddress = 0x1594ae8;
inline constexpr std::uint32_t kVtable1594ae8Slot50_13d60SlotOffset = 0x50;
inline constexpr std::uint64_t kVtable1594ae8Slot50_13d60RelocAddress = 0x1594b38;
inline constexpr std::uint64_t kVtable1594ae8Slot50_13d60BaseOffset = 0x60;
inline constexpr std::uint64_t kVtable1594ae8Slot50_13d60FieldOffset = 0x70;

struct Vtable1594ae8Slot50_13d60Cells {
    // Host storage for the object; at least `FieldOffset + 8` bytes. Null is fail-visible.
    std::uint8_t* object = nullptr;
};

struct Vtable1594ae8Slot50_13d60Result {
    bool ok = false;
    void* stored = nullptr;     // this + 0x60, the value written to +0x70 and returned
    void* previous = nullptr;   // the old qword at +0x70
};

// `*(void**)(this + 0x70) = this + 0x60`, returning that pointer.
[[nodiscard]] Vtable1594ae8Slot50_13d60Result vtable_1594ae8_slot50_13d60(
    const Vtable1594ae8Slot50_13d60Cells& cells = {}) noexcept;

}  // namespace sotc
