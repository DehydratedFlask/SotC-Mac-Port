// Vtable slot resolved by the indirect-frontier card: `0x15949a0 + 0x88 -> 0x187c0`.
//
// Evidence:
//   * `0x1254d` is `call *0x88(%rax)` with `%rax` loaded from the RIP-relative table `0x15949a0`;
//   * the qword at `0x15949a0 + 0x88` in the pinned image is `0x187c0`;
//   * the dynamic relocation table has one type-8 row at `0x1594a28` with addend `0x187c0`;
//   * `0x187c0` is an FDE function start with length `0x1c`.
//
// Body (28 B, `55 48 89 e5 53 50 48 89 fb e8 52 ee 02 00 48 89 df 48 83 c4 08 5b 5d e9 d4 e8 ff ff`):
//   push rbp; mov rbp,rsp; push rbx; push rax
//   mov  %rdi, %rbx          ; save the object
//   call 0x47620             ; unconditional pre-pass on the object
//   mov  %rbx, %rdi          ; reload the object as the free argument
//   add  $0x8, %rsp; pop rbx; pop rbp
//   jmp  0x170b0             ; tail call: tag-1 free (native `free_170b0`)
//
// What is modelled: the unconditional `0x47620` call as a hook, then the tag-1 free through the
// allocation dispatch contract. What is NOT modelled: the body of `0x47620` and any post-free use.
// The function returns nothing (`jmp` tail call), so `ok` reports only that the free succeeded.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15949a0Slot88_187c0Rva = 0x187c0;
inline constexpr std::size_t kVtable15949a0Slot88_187c0Size = 0x1c;
inline constexpr std::uint64_t kVtable15949a0Slot88_187c0EndAddress = 0x187dc;
inline constexpr std::uint64_t kVtable15949a0Slot88_187c0VtableAddress = 0x15949a0;
inline constexpr std::uint32_t kVtable15949a0Slot88_187c0SlotOffset = 0x88;
inline constexpr std::uint64_t kVtable15949a0Slot88_187c0RelocAddress = 0x1594a28;
inline constexpr std::uint64_t kVtable15949a0Slot88_187c0PreCallRva = 0x47620;
inline constexpr std::uint64_t kVtable15949a0Slot88_187c0FreeTargetRva = 0x170b0;

struct Vtable15949a0Slot88_187c0Cells {
    void* self = nullptr;
};

struct Vtable15949a0Slot88_187c0Hooks {
    // Stands in for `call 0x47620`. The callee body is out of scope.
    void (*pre_call_47620)(void* self) = nullptr;
    // Allocation dispatch used by the tail free. Null means "no contract": a non-null object then
    // fails visibly instead of pretending it was released.
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable15949a0Slot88_187c0Result {
    bool ok = false;
    bool pre_call_modelled = false;  // the 0x47620 call was counted
    bool freed = false;              // tag-1 free reported success
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable15949a0Slot88_187c0Result vtable_15949a0_slot88_187c0(
    const Vtable15949a0Slot88_187c0Cells& cells = {},
    const Vtable15949a0Slot88_187c0Hooks& hooks = {}) noexcept;

}  // namespace sotc
