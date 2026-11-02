// Table slot resolved by the provenance-audited vtable-closure sweep:
// `0x15de228 + 0x20 -> 0xd7c4b0`.
//
// Evidence:
//   * `0xbda7e5` is `call *0x20(%rax)` where `%rax` is loaded by
//     `leaq 0x15de228(%rip), %rax` at `0xbda7be` and only read afterwards
//     (`leaq 0x10(%rax), %rcx; movq %rcx, (%rbx); ...`);
//   * the qword at `0x15de228 + 0x20` in the pinned image is `0xd7c4b0`;
//   * the dynamic relocation table has one type-8 row at `0x15de248` with addend `0xd7c4b0`;
//   * `0xd7c4b0` is an FDE function start with length `0x3`.
//
// Body (3 B, `31 c0 c3`): `xor %eax, %eax; retq` — an unconditional zero return. No register is
// read, nothing is stored, and the result does not depend on `this`. The body is modelled as the
// constant it computes, not as a hook. The bytes and the sha256 are the same as `0x13d50`; the two
// entries are separate rows of separate tables.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15de228Slot20_d7c4b0Rva = 0xd7c4b0;
inline constexpr std::size_t kVtable15de228Slot20_d7c4b0Size = 0x3;
inline constexpr std::uint64_t kVtable15de228Slot20_d7c4b0EndAddress = 0xd7c4b3;
inline constexpr std::uint64_t kVtable15de228Slot20_d7c4b0VtableAddress = 0x15de228;
inline constexpr std::uint32_t kVtable15de228Slot20_d7c4b0SlotOffset = 0x20;
inline constexpr std::uint64_t kVtable15de228Slot20_d7c4b0RelocAddress = 0x15de248;

struct Vtable15de228Slot20_d7c4b0Result {
    bool ok = false;
    std::int32_t code = 0;  // eax: the constant zero the body returns
};

// Returns `eax = 0` on every input; `this` is never dereferenced.
[[nodiscard]] Vtable15de228Slot20_d7c4b0Result vtable_15de228_slot20_d7c4b0() noexcept;

}  // namespace sotc
