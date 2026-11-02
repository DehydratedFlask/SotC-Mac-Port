// Vtable slot resolved by the vtable-closure sweep: `0x1595b60 + 0x18 -> 0x488d0`.
//
// Evidence:
//   * `0x113c5` is `call *0x18(%rax)` with `%rax` loaded from the RIP-relative table `0x1595b60`;
//   * the qword at `0x1595b60 + 0x18` in the pinned image is `0x488d0`;
//   * the dynamic relocation table has one type-8 row at `0x1595b78` with addend `0x488d0`;
//   * `0x488d0` is an FDE function start with length `0x1c`.
//
// Body (28 B, `55 48 89 e5 53 50 48 89 fb e8 b2 fe ff ff 48 89 df 48 83 c4 08 5b 5d e9 c4 e7 fc ff`):
//   push rbp; mov rbp,rsp; push rbx; push rax
//   mov  %rdi, %rbx          ; save the object
//   call 0x48790             ; unconditional pre-pass on the object
//   mov  %rbx, %rdi          ; reload the object as the free argument
//   add  $0x8, %rsp; pop rbx; pop rbp
//   jmp  0x170b0             ; tail call: tag-1 free
//
// The third instance of the same teardown shape (`0x187c0`, `0xe19cb0`), with pre-pass `0x48790`.
// Modelled: the pre-pass as a hook and the tag-1 free through the allocation contract. Not
// modelled: the body of `0x48790` and any post-free use.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1595b60Slot18_488d0Rva = 0x488d0;
inline constexpr std::size_t kVtable1595b60Slot18_488d0Size = 0x1c;
inline constexpr std::uint64_t kVtable1595b60Slot18_488d0EndAddress = 0x488ec;
inline constexpr std::uint64_t kVtable1595b60Slot18_488d0VtableAddress = 0x1595b60;
inline constexpr std::uint32_t kVtable1595b60Slot18_488d0SlotOffset = 0x18;
inline constexpr std::uint64_t kVtable1595b60Slot18_488d0RelocAddress = 0x1595b78;
inline constexpr std::uint64_t kVtable1595b60Slot18_488d0PreCallRva = 0x48790;
inline constexpr std::uint64_t kVtable1595b60Slot18_488d0FreeTargetRva = 0x170b0;

struct Vtable1595b60Slot18_488d0Cells {
    void* self = nullptr;
};

struct Vtable1595b60Slot18_488d0Hooks {
    // Stands in for `call 0x48790`. The callee body is out of scope.
    void (*pre_call_48790)(void* self) = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable1595b60Slot18_488d0Result {
    bool ok = false;
    bool pre_call_modelled = false;
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable1595b60Slot18_488d0Result vtable_1595b60_slot18_488d0(
    const Vtable1595b60Slot18_488d0Cells& cells = {},
    const Vtable1595b60Slot18_488d0Hooks& hooks = {}) noexcept;

}  // namespace sotc
