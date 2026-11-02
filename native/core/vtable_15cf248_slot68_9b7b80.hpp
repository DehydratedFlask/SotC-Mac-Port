// Table slot resolved by the provenance-audited vtable-closure sweep:
// `0x15cf248 + 0x68 -> 0x9b7b80`.
//
// Evidence:
//   * thirteen call sites are `call *0x68(%rax)` where `%rax` is loaded by
//     `leaq 0x15cf248(%rip), %rax` a few instructions earlier and is never rewritten before the
//     call (the nearest write to `%rax` is that `lea`): `0x21a695`, `0x21ba92`, `0x21bb84`,
//     `0x21c573`, `0x21c725`, `0x21d134`, `0x21d306`, `0x2f54da`, `0x3e4af3`, `0x3e4dc4`,
//     `0x3e7237`, `0x3edf1a`, `0x3ee19f`;
//   * the qword at `0x15cf248 + 0x68` in the pinned image is `0x9b7b80`;
//   * the dynamic relocation table has one type-8 row at `0x15cf2b0` with addend `0x9b7b80`;
//   * `0x9b7b80` is an FDE function start with length `0x6`.
//
// The container is a vtable: each caller stores `0x15cf248 + 0x10` as the object's vptr and then
// dispatches through the base table's slot `0x68` (a qualified call, not a virtual one).
//
// Body (6 B, `c5 fa 10 47 38 c3`): `vmovss 0x38(%rdi), %xmm0; retq`
// Reads the 32-bit float at `this + 0x38` and returns it in `%xmm0`. Nothing is stored, no other
// cell is read, and the result depends only on that one float. The body is modelled as the read it
// performs, not as a hook.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15cf248Slot68_9b7b80Rva = 0x9b7b80;
inline constexpr std::size_t kVtable15cf248Slot68_9b7b80Size = 0x6;
inline constexpr std::uint64_t kVtable15cf248Slot68_9b7b80EndAddress = 0x9b7b86;
inline constexpr std::uint64_t kVtable15cf248Slot68_9b7b80VtableAddress = 0x15cf248;
inline constexpr std::uint32_t kVtable15cf248Slot68_9b7b80SlotOffset = 0x68;
inline constexpr std::uint64_t kVtable15cf248Slot68_9b7b80RelocAddress = 0x15cf2b0;
inline constexpr std::uint64_t kVtable15cf248Slot68_9b7b80ValueOffset = 0x38;

struct Vtable15cf248Slot68_9b7b80Cells {
    // `rdi` at entry; the body reads the four bytes at `+0x38`.
    const std::uint8_t* self = nullptr;
};

struct Vtable15cf248Slot68_9b7b80Result {
    bool ok = false;
    float value = 0.0F;  // xmm0: the float at `self + 0x38`
};

// Returns `*(const float*)(self + 0x38)`; a null `self` is a visible failure, never a zero.
[[nodiscard]] Vtable15cf248Slot68_9b7b80Result vtable_15cf248_slot68_9b7b80(
    const Vtable15cf248Slot68_9b7b80Cells& cells = {}) noexcept;

}  // namespace sotc
