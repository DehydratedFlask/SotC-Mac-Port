// Resolved vtable target `0x2687b0` (reached from vtable `0x159c908` slot `0x58`).
//
// Evidence: `0x257068` is `call *0x58(%rax)` with `%rax` from the RIP-relative table `0x159c908`;
// the qword at slot `0x58` is `0x2687b0`, the dynamic relocation at `0x159c960` is type 8 with
// addend `0x2687b0`, and `0x2687b0` is an FDE function start of length `0x1c`.
//
// Body (28 B, `55 48 89 e5 53 50 48 89 fb e8 12 00 00 00 48 89 df 48 83 c4 08 5b 5d e9 e4 e8 da ff`):
//   push rbp; mov rbp,rsp; push rbx; push rax
//   mov  %rdi, %rbx      ; save the object
//   call 0x2687d0        ; unconditional pre-pass
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

inline constexpr std::uint64_t kVtable159c908Slot58_2687b0Rva = 0x2687b0;
inline constexpr std::size_t kVtable159c908Slot58_2687b0Size = 0x1c;
inline constexpr std::uint64_t kVtable159c908Slot58_2687b0EndAddress = 0x2687cc;
inline constexpr std::uint64_t kVtable159c908Slot58_2687b0VtableAddress = 0x159c908;
inline constexpr std::uint32_t kVtable159c908Slot58_2687b0SlotOffset = 0x58;
inline constexpr std::uint64_t kVtable159c908Slot58_2687b0RelocAddress = 0x159c960;
inline constexpr std::uint64_t kVtable159c908Slot58_2687b0PreCallRva = 0x2687d0;
inline constexpr std::uint64_t kVtable159c908Slot58_2687b0FreeTargetRva = 0x170b0;

struct Vtable159c908Slot58_2687b0Cells {
    void* self = nullptr;
};

struct Vtable159c908Slot58_2687b0Hooks {
    void (*pre_call_2687d0)(void* self) = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable159c908Slot58_2687b0Result {
    bool ok = false;
    bool pre_call_modelled = false;
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable159c908Slot58_2687b0Result vtable_159c908_slot58_2687b0(
    const Vtable159c908Slot58_2687b0Cells& cells = {},
    const Vtable159c908Slot58_2687b0Hooks& hooks = {}) noexcept;

}  // namespace sotc
