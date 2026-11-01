// Resolved vtable target `0xa460f0` (reached from vtable `0x15d1678` slot `0x280`).
//
// Evidence: `0xa3dff7` is `call *0x280(%rax)` with `%rax` from the RIP-relative table `0x15d1678`;
// the qword at slot `0x280` is `0xa460f0`, the dynamic relocation at `0x15d18f8` is type 8 with
// addend `0xa460f0`, and `0xa460f0` is an FDE function start of length `0x3e`.
//
// Body (62 B):
//   push rbp; mov rbp,rsp; push rbx; push rax
//   lea  0x15d1808(%rip), %rax   ; table base
//   mov  %rdi, %rbx              ; this
//   lea  0xa0(%rbx), %rdi        ; sub-object at this+0xa0
//   add  $0x10, %rax             ; rax = 0x15d1818
//   mov  %rax, (%rbx)            ; install the vtable pointer at *this
//   mov  0xc0(%rbx), %rax        ; helper object at this+0xc0
//   call *0x18(%rax)             ; virtual call on that helper, slot 0x18
//   mov  %rbx, %rdi
//   call 0x20b50                 ; post-pass with this
//   mov  %rbx, %rdi
//   add  $0x8,%rsp; pop rbx; pop rbp
//   jmp  0x170b0                 ; tag-1 free of this
//
// This is a teardown shape: install a base vtable, run one virtual call on the helper at
// `this+0xc0`, run one post-pass, then free. Modelled: the vtable store (recorded), the helper call
// and the post-pass as hooks, and the free through the allocation contract. Not modelled: the
// helper's virtual function, `0x20b50`, and the sub-object beyond passing `this+0xa0` to it.
// Cell and helper pointers are fail-visible because every read and store here is unconditional.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0Rva = 0xa460f0;
inline constexpr std::size_t kVtable15d1678Slot280_a460f0Size = 0x3e;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0EndAddress = 0xa4612e;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0VtableAddress = 0x15d1678;
inline constexpr std::uint32_t kVtable15d1678Slot280_a460f0SlotOffset = 0x280;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0RelocAddress = 0x15d18f8;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0InstalledTable = 0x15d1818;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0HelperFieldOffset = 0xc0;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0SubObjectOffset = 0xa0;
inline constexpr std::uint32_t kVtable15d1678Slot280_a460f0HelperSlot = 0x18;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0PostCallRva = 0x20b50;
inline constexpr std::uint64_t kVtable15d1678Slot280_a460f0FreeTargetRva = 0x170b0;

struct Vtable15d1678Slot280_a460f0Cells {
    void* self = nullptr;                 // rdi at entry
    void** vtable_field = nullptr;        // *this, receives 0x15d1818
    const void* const* helper_field = nullptr;  // this+0xc0
};

struct Vtable15d1678Slot280_a460f0Hooks {
    void (*helper_vcall18)(void* helper) = nullptr;
    void (*post_call_20b50)(void* self) = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Vtable15d1678Slot280_a460f0Result {
    bool ok = false;
    void* vtable_written = nullptr;
    bool helper_called = false;
    bool post_called = false;
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable15d1678Slot280_a460f0Result vtable_15d1678_slot280_a460f0(
    const Vtable15d1678Slot280_a460f0Cells& cells = {},
    const Vtable15d1678Slot280_a460f0Hooks& hooks = {}) noexcept;

}  // namespace sotc
