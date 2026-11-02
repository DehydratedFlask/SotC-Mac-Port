// Vtable slot resolved by the vtable-closure sweep: `0x15e1ee0 + 0x10 -> 0xe19440`.
//
// Evidence:
//   * `0xe14c26` is `call *0x10(%rax)` with `%rax` loaded from the RIP-relative table `0x15e1ee0`;
//   * the qword at `0x15e1ee0 + 0x10` in the pinned image is `0xe19440`;
//   * the dynamic relocation table has one type-8 row at `0x15e1ef0` with addend `0xe19440`;
//   * `0xe19440` is an FDE function start with length `0x8f`.
//
// Body (143 B):
//   push rbp; mov rbp,rsp; push r15; push r14; push r12; push rbx
//   mov  $0x8, %edi                 ; the size argument; **rdi at entry is never read**
//   mov  %rsi, %rbx                 ; the only input: the target object
//   call 0x17070                    ; allocate(8), three times
//   ... stamp each 8-byte block with a distinct adapter vtable (+0x10):
//       0x15e1f30, 0x15e1f88, 0x15e1fe0
//   mov  (%rbx), %rax; call *0x10(%rax)   ; target->vtable[0x10](target, node_a)
//   mov  (%rbx), %rax; call *0x10(%rax)   ; target->vtable[0x10](target, node_b)
//   mov  (%rbx), %rax; mov 0x10(%rax),%rax; jmp *%rax   ; tail call with node_c
//
// The first argument register is overwritten with the allocation size before it is read, so this
// entry reads only `%rsi`. Modelled: three allocations through the recovered `0x17070` contract, the
// three vtable stamps, the two calls and the tail dispatch as hooks. Not modelled: the allocator
// body, the adapter vtables, and the slot-0x10 target body. A null allocation or a missing dispatch
// hook is a visible failure; the body would dereference both unconditionally.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440Rva = 0xe19440;
inline constexpr std::size_t kVtable15e1ee0Slot10_e19440Size = 0x8f;
inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440EndAddress = 0xe194cf;
inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440VtableAddress = 0x15e1ee0;
inline constexpr std::uint32_t kVtable15e1ee0Slot10_e19440SlotOffset = 0x10;
inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440RelocAddress = 0x15e1ef0;
inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440AllocRva = 0x17070;
inline constexpr std::size_t kVtable15e1ee0Slot10_e19440NodeSize = 0x8;
inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440StampA = 0x15e1f30;
inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440StampB = 0x15e1f88;
inline constexpr std::uint64_t kVtable15e1ee0Slot10_e19440StampC = 0x15e1fe0;
inline constexpr std::uint32_t kVtable15e1ee0Slot10_e19440DispatchSlot = 0x10;

struct Vtable15e1ee0Slot10_e19440Cells {
    // The target object (rsi at entry). Its first qword is the vtable pointer.
    void* target = nullptr;
};

struct Vtable15e1ee0Slot10_e19440Hooks {
    // Allocation dispatch used by `0x17070` (tag 1, alignment 8, flags 1).
    const AllocDispatchContract* alloc = nullptr;
    // Stands in for `call *0x10(%rax)` on the target's vtable; called three times.
    std::int32_t (*dispatch_slot10)(void* target, void* node) = nullptr;
};

struct Vtable15e1ee0Slot10_e19440Result {
    bool ok = false;
    bool allocated = false;        // all three 8-byte nodes were produced
    void* node_a = nullptr;
    void* node_b = nullptr;
    void* node_c = nullptr;
    bool stamped = false;          // all three vtable stamps were written
    std::uint32_t calls = 0;       // the two non-tail dispatches
    bool tail_dispatched = false;
    std::int32_t code = 0;         // the tail dispatch's return value
};

[[nodiscard]] Vtable15e1ee0Slot10_e19440Result vtable_15e1ee0_slot10_e19440(
    const Vtable15e1ee0Slot10_e19440Cells& cells = {},
    const Vtable15e1ee0Slot10_e19440Hooks& hooks = {}) noexcept;

}  // namespace sotc
