// Vtable slot resolved by the vtable-closure sweep: `0x15e2028 + 0x18 -> 0xe19cb0`.
//
// Evidence grade: **table-level only** (E1066). The qword at this slot, the type-8 RELA addend and the
// FDE start tie the target to the table; they do NOT prove that the call site named here dispatches
// there. That site's call register is not provably this table load, so it can dispatch through
// another table at run time. `docs/INDIRECT_FRONTIER.md` "Evidence grades" defines both grades.
//
// Evidence:
//   * `0xe1b245` is `call *0x18(%rax)` with `%rax` loaded from the RIP-relative table `0x15e2028`;
//   * the qword at `0x15e2028 + 0x18` in the pinned image is `0xe19cb0`;
//   * the dynamic relocation table has one type-8 row at `0x15e2040` with addend `0xe19cb0`;
//   * `0xe19cb0` is an FDE function start with length `0x1c`.
//
// Body (28 B, `55 48 89 e5 53 50 48 89 fb e8 f2 14 00 00 48 89 df 48 83 c4 08 5b 5d e9 e4 d3 1f ff`):
//   push rbp; mov rbp,rsp; push rbx; push rax
//   mov  %rdi, %rbx          ; save the object
//   call 0xe1b1b0            ; unconditional pre-pass on the object
//   mov  %rbx, %rdi          ; reload the object as the free argument
//   add  $0x8, %rsp; pop rbx; pop rbp
//   jmp  0x170b0             ; tail call: tag-1 free
//
// This is the same teardown shape as `0x187c0` (`0x15949a0 + 0x88`) with a different pre-pass.
// Modelled: the pre-pass as a hook and the tag-1 free through the allocation contract. Not
// modelled: the body of `0xe1b1b0` and any post-free use.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e2028Slot18_e19cb0Rva = 0xe19cb0;
inline constexpr std::size_t kVtable15e2028Slot18_e19cb0Size = 0x1c;
inline constexpr std::uint64_t kVtable15e2028Slot18_e19cb0EndAddress = 0xe19ccc;
inline constexpr std::uint64_t kVtable15e2028Slot18_e19cb0VtableAddress = 0x15e2028;
inline constexpr std::uint32_t kVtable15e2028Slot18_e19cb0SlotOffset = 0x18;
inline constexpr std::uint64_t kVtable15e2028Slot18_e19cb0RelocAddress = 0x15e2040;
inline constexpr std::uint64_t kVtable15e2028Slot18_e19cb0PreCallRva = 0xe1b1b0;
inline constexpr std::uint64_t kVtable15e2028Slot18_e19cb0FreeTargetRva = 0x170b0;

struct Vtable15e2028Slot18_e19cb0Cells {
    void* self = nullptr;
};

struct Vtable15e2028Slot18_e19cb0Hooks {
    // Stands in for `call 0xe1b1b0`. The callee body is out of scope.
    void (*pre_call_e1b1b0)(void* self) = nullptr;
    // Allocation dispatch used by the tail free. Null means "no contract": a non-null object then
    // fails visibly instead of pretending it was released.
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable15e2028Slot18_e19cb0Result {
    bool ok = false;
    bool pre_call_modelled = false;  // the 0xe1b1b0 call was counted
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable15e2028Slot18_e19cb0Result vtable_15e2028_slot18_e19cb0(
    const Vtable15e2028Slot18_e19cb0Cells& cells = {},
    const Vtable15e2028Slot18_e19cb0Hooks& hooks = {}) noexcept;

}  // namespace sotc
