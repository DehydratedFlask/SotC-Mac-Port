// Vtable slot resolved by the indirect-frontier card: `0x15d1d80 + 0x08 -> 0xc740`.
//
// Evidence:
//   * `0xa54b7b` and `0xa54c92` are `call *0x8(%rax)` with `%rax` loaded from the RIP-relative
//     table `0x15d1d80`;
//   * the qword at `0x15d1d80 + 0x08` in the pinned image is `0xc740`;
//   * the dynamic relocation table has one type-8 row at `0x15d1d88` with addend `0xc740`;
//   * `0xc740` is an FDE function start with length `0x14`.
//
// Body (20 B, `48 8d 05 59 7d 58 01 48 89 47 20 48 8b 46 10 48 89 47 10 c3`): the same shape as
// `0x358c0` but installing its own table `0x15944a0` at `this+0x20` and copying `[rsi+0x10]` to
// `this+0x10`. Signature inferred from the body only: `void f(This* rdi, const Source* rsi)`.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15d1d80Slot08_c740Rva = 0xc740;
inline constexpr std::size_t kVtable15d1d80Slot08_c740Size = 0x14;
inline constexpr std::uint64_t kVtable15d1d80Slot08_c740EndAddress = 0xc754;
inline constexpr std::uint64_t kVtable15d1d80Slot08_c740VtableAddress = 0x15d1d80;
inline constexpr std::uint32_t kVtable15d1d80Slot08_c740SlotOffset = 0x08;
inline constexpr std::uint64_t kVtable15d1d80Slot08_c740RelocAddress = 0x15d1d88;
inline constexpr std::uint64_t kVtable15d1d80Slot08_c740InstalledTable = 0x15944a0;
inline constexpr std::uint64_t kVtable15d1d80Slot08_c740TableFieldOffset = 0x20;
inline constexpr std::uint64_t kVtable15d1d80Slot08_c740SourceOffset = 0x10;
inline constexpr std::uint64_t kVtable15d1d80Slot08_c740CopyFieldOffset = 0x10;

struct Vtable15d1d80Slot08_c740Cells {
    void** table_field = nullptr;
    const void* const* source_field = nullptr;
    void** copy_field = nullptr;
};

struct Vtable15d1d80Slot08_c740Result {
    bool ok = false;
    void* table_written = nullptr;
    void* copied = nullptr;
};

[[nodiscard]] Vtable15d1d80Slot08_c740Result vtable_15d1d80_slot08_c740(
    const Vtable15d1d80Slot08_c740Cells& cells = {}) noexcept;

}  // namespace sotc
