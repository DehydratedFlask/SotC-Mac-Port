// Resolved vtable target `0xc2e0` (reached from vtable `0x15d1678` slot `0xe8`).
//
// Evidence: `0xa3df9c` is `call *0xe8(%rax)` with `%rax` from the RIP-relative table `0x15d1678`;
// the qword at slot `0xe8` is `0xc2e0`, the dynamic relocation at `0x15d1760` is type 8 with addend
// `0xc2e0`, and `0xc2e0` is an FDE function start of length `0x9`.
//
// Body (9 B, `48 8b 46 10 48 89 47 10 c3`): `mov 0x10(%rsi), %rax; mov %rax, 0x10(%rdi); retq` —
// copy one qword from the source argument into the object. No table install, nothing returned.
// Signature inferred from the body only: `void f(This* rdi, const Source* rsi)`.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15d1678SlotE8_c2e0Rva = 0xc2e0;
inline constexpr std::size_t kVtable15d1678SlotE8_c2e0Size = 0x9;
inline constexpr std::uint64_t kVtable15d1678SlotE8_c2e0EndAddress = 0xc2e9;
inline constexpr std::uint64_t kVtable15d1678SlotE8_c2e0VtableAddress = 0x15d1678;
inline constexpr std::uint32_t kVtable15d1678SlotE8_c2e0SlotOffset = 0xe8;
inline constexpr std::uint64_t kVtable15d1678SlotE8_c2e0RelocAddress = 0x15d1760;
inline constexpr std::uint64_t kVtable15d1678SlotE8_c2e0SourceOffset = 0x10;
inline constexpr std::uint64_t kVtable15d1678SlotE8_c2e0CopyFieldOffset = 0x10;

struct Vtable15d1678SlotE8_c2e0Cells {
    const void* const* source_field = nullptr;  // source + 0x10
    void** copy_field = nullptr;                // object + 0x10
};

struct Vtable15d1678SlotE8_c2e0Result {
    bool ok = false;
    void* copied = nullptr;
};

[[nodiscard]] Vtable15d1678SlotE8_c2e0Result vtable_15d1678_slot_e8_c2e0(
    const Vtable15d1678SlotE8_c2e0Cells& cells = {}) noexcept;

}  // namespace sotc
