// Vtable slot resolved by the indirect-frontier card: `0x15955c0 + 0x08 -> 0x358c0`.
//
// Evidence:
//   * `0x35165` is `call *0x8(%rax)` with `%rax` loaded from the RIP-relative table `0x15955c0`;
//   * the qword at `0x15955c0 + 0x08` in the pinned image is `0x358c0`;
//   * the dynamic relocation table has one type-8 row at `0x15955c8` with addend `0x358c0`;
//   * `0x358c0` is an FDE function start with length `0x14`.
//
// Body (20 B, `48 8d 05 f9 fc 55 01 48 89 47 20 48 8b 46 10 48 89 47 10 c3`):
//   lea  0x15955c0(%rip), %rax   ; this object's own table address
//   mov  %rax, 0x20(%rdi)        ; install it at this+0x20
//   mov  0x10(%rsi), %rax        ; read one qword from the source argument
//   mov  %rax, 0x10(%rdi)        ; publish it at this+0x10
//   retq
// Signature inferred from the body only: `void f(This* rdi, const Source* rsi)`; nothing is
// returned and both stores are unconditional, so a missing host cell is fail-visible.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15955c0Slot08_358c0Rva = 0x358c0;
inline constexpr std::size_t kVtable15955c0Slot08_358c0Size = 0x14;
inline constexpr std::uint64_t kVtable15955c0Slot08_358c0EndAddress = 0x358d4;
inline constexpr std::uint64_t kVtable15955c0Slot08_358c0VtableAddress = 0x15955c0;
inline constexpr std::uint32_t kVtable15955c0Slot08_358c0SlotOffset = 0x08;
inline constexpr std::uint64_t kVtable15955c0Slot08_358c0RelocAddress = 0x15955c8;
inline constexpr std::uint64_t kVtable15955c0Slot08_358c0InstalledTable = 0x15955c0;
inline constexpr std::uint64_t kVtable15955c0Slot08_358c0TableFieldOffset = 0x20;
inline constexpr std::uint64_t kVtable15955c0Slot08_358c0SourceOffset = 0x10;
inline constexpr std::uint64_t kVtable15955c0Slot08_358c0CopyFieldOffset = 0x10;

struct Vtable15955c0Slot08_358c0Cells {
    // Host storage for the object's `+0x20` qword (where the table address is installed).
    void** table_field = nullptr;
    // Host storage for the source object's `+0x10` qword.
    const void* const* source_field = nullptr;
    // Host storage for the object's `+0x10` qword.
    void** copy_field = nullptr;
};

struct Vtable15955c0Slot08_358c0Result {
    bool ok = false;
    void* table_written = nullptr;
    void* copied = nullptr;
};

[[nodiscard]] Vtable15955c0Slot08_358c0Result vtable_15955c0_slot08_358c0(
    const Vtable15955c0Slot08_358c0Cells& cells = {}) noexcept;

}  // namespace sotc
