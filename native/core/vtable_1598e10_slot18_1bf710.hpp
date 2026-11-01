// Resolved vtable target `0x1bf710` (reached from vtable `0x1598e10` slot `0x18`).
//
// Evidence: `0xa70f4d` is `call *0x18(%rax)` with `%rax` from the RIP-relative table `0x1598e10`;
// the qword at slot `0x18` is `0x1bf710`, the dynamic relocation at `0x1598e28` is type 8 with
// addend `0x1bf710`, and `0x1bf710` is an FDE function start of length `0x1c`.
//
// Body (28 B, `55 48 89 e5 53 50 48 89 fb e8 12 00 00 00 48 89 df 48 83 c4 08 5b 5d e9 e4 e8 da ff`):
//   push rbp; mov rbp,rsp; push rbx; push rax
//   mov  %rdi, %rbx      ; save the object
//   call 0x1c0010        ; unconditional pre-pass
//   mov  %rbx, %rdi
//   add  $0x8, %rsp; pop rbx; pop rbp
//   jmp  0x170b0         ; tail call: tag-1 free
//
// Same teardown shape as `0x187c0` with a different pre-pass: the pre-call is unconditional, the
// function returns nothing, and the only argument before the tail jump is `rdi`. Nothing else is
// read, so `void f(This* rdi)` is inferred from the body, not from a caller contract.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1598e10Slot18_1bf710Rva = 0x1bf710;
inline constexpr std::size_t kVtable1598e10Slot18_1bf710Size = 0x1c;
inline constexpr std::uint64_t kVtable1598e10Slot18_1bf710EndAddress = 0x1bf72c;
inline constexpr std::uint64_t kVtable1598e10Slot18_1bf710VtableAddress = 0x1598e10;
inline constexpr std::uint32_t kVtable1598e10Slot18_1bf710SlotOffset = 0x18;
inline constexpr std::uint64_t kVtable1598e10Slot18_1bf710RelocAddress = 0x1598e28;
inline constexpr std::uint64_t kVtable1598e10Slot18_1bf710PreCallRva = 0x1c0010;
inline constexpr std::uint64_t kVtable1598e10Slot18_1bf710FreeTargetRva = 0x170b0;

struct Vtable1598e10Slot18_1bf710Cells {
    void* self = nullptr;
};

struct Vtable1598e10Slot18_1bf710Hooks {
    void (*pre_call_1c0010)(void* self) = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable1598e10Slot18_1bf710Result {
    bool ok = false;
    bool pre_call_modelled = false;
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable1598e10Slot18_1bf710Result vtable_1598e10_slot18_1bf710(
    const Vtable1598e10Slot18_1bf710Cells& cells = {},
    const Vtable1598e10Slot18_1bf710Hooks& hooks = {}) noexcept;

}  // namespace sotc
