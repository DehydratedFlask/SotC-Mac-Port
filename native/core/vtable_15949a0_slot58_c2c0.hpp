// Vtable slot resolved by the vtable-closure sweep: `0x15949a0 + 0x58 -> 0xc2c0`.
//
// Evidence:
//   * `0x465eb` is `call *0x58(%rax)` with `%rax` loaded from the RIP-relative table `0x15949a0`;
//   * the qword at `0x15949a0 + 0x58` in the pinned image is `0xc2c0`;
//   * the dynamic relocation table has one type-8 row at `0x15949f8` with addend `0xc2c0`;
//   * `0xc2c0` is an FDE function start with length `0x14`.
//
// Body (20 B, `48 8d 05 b9 7f 58 01 48 89 47 20 48 8b 46 10 48 89 47 10 c3`):
//   `leaq 0x1594280(%rip), %rax; movq %rax, 0x20(%rdi)`
//   `movq 0x10(%rsi), %rax;      movq %rax, 0x10(%rdi); retq`
// It stamps the object with the fixed image address `0x1594280` at `+0x20` and copies one qword from
// the source's `+0x10` to the object's `+0x10`. It reads `%rdi` and `%rsi` only; both are dereferenced
// unconditionally, so null cells fail visibly.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0Rva = 0xc2c0;
inline constexpr std::size_t kVtable15949a0Slot58_c2c0Size = 0x14;
inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0EndAddress = 0xc2d4;
inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0VtableAddress = 0x15949a0;
inline constexpr std::uint32_t kVtable15949a0Slot58_c2c0SlotOffset = 0x58;
inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0RelocAddress = 0x15949f8;
inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0StampAddress = 0x1594280;
inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0StampOffset = 0x20;
inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0CopyOffset = 0x10;
inline constexpr std::uint64_t kVtable15949a0Slot58_c2c0SourceOffset = 0x10;

struct Vtable15949a0Slot58_c2c0Cells {
    // Host storage for the object; the body writes `+0x10` and `+0x20`.
    std::uint8_t* object = nullptr;
    // Host storage for the source; the body reads `+0x10`.
    const std::uint8_t* source = nullptr;
};

struct Vtable15949a0Slot58_c2c0Result {
    bool ok = false;
    std::uint64_t stamp = 0;       // the constant written at +0x20
    std::uint64_t copied = 0;      // the qword copied into +0x10
    std::uint64_t previous = 0;    // the old qword at +0x10
};

// `*(u64*)(this + 0x20) = 0x1594280; *(u64*)(this + 0x10) = *(u64*)(source + 0x10)`.
[[nodiscard]] Vtable15949a0Slot58_c2c0Result vtable_15949a0_slot58_c2c0(
    const Vtable15949a0Slot58_c2c0Cells& cells = {}) noexcept;

}  // namespace sotc
