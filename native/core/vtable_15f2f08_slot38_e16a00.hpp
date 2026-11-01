// Resolved vtable target `0xe16a00` (reached from vtable `0x15f2f08` slot `0x38`).
//
// Evidence: `0xed171c` and `0xed1733` are `call *0x38(%rax)` with `%rax` from the RIP-relative
// table `0x15f2f08`; the qword at slot `0x38` is `0xe16a00`; the dynamic relocation is at
// `0x15f2f40` (type 8, addend `0xe16a00`); `0xe16a00` is an FDE function start of length `0x26`.
// Note: an earlier note in the frontier summary said `0x15f25f0`; the live RELA row is `0x15f2f40`.
//
// Body (38 B):
//   push rbp; mov rbp,rsp; push r14; push rbx
//   mov  %rdi, %rbx          ; this
//   mov  %rsi, %r14          ; second argument
//   mov  (%rbx), %rax        ; vtable
//   call *0x18(%rax)         ; virtual call slot 0x18 with (this, arg1)
//   mov  (%rbx), %rax        ; vtable again (the call may have replaced it)
//   mov  %rbx, %rdi
//   mov  %r14, %rsi
//   mov  0x20(%rax), %rax    ; slot 0x20
//   pop rbx; pop r14; pop rbp
//   jmp  *%rax               ; tail call slot 0x20 with the same arguments
//
// Modelled: the vtable read, the slot 0x18 call and the slot 0x20 tail call as ordered hooks.
// Not modelled: both callees. The vtable pointer is reloaded between the calls, so the model reads
// it once per call from the same cell; a null object or null vtable fails visibly because the
// retail body would fault on `mov (%rbx),%rax`.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15f2f08Slot38_e16a00Rva = 0xe16a00;
inline constexpr std::size_t kVtable15f2f08Slot38_e16a00Size = 0x26;
inline constexpr std::uint64_t kVtable15f2f08Slot38_e16a00EndAddress = 0xe16a26;
inline constexpr std::uint64_t kVtable15f2f08Slot38_e16a00VtableAddress = 0x15f2f08;
inline constexpr std::uint32_t kVtable15f2f08Slot38_e16a00SlotOffset = 0x38;
inline constexpr std::uint64_t kVtable15f2f08Slot38_e16a00RelocAddress = 0x15f2f40;
inline constexpr std::uint32_t kVtable15f2f08Slot38_e16a00FirstCallSlot = 0x18;
inline constexpr std::uint32_t kVtable15f2f08Slot38_e16a00TailCallSlot = 0x20;

struct Vtable15f2f08Slot38_e16a00Cells {
    void* self = nullptr;
    // Host storage for the object's vtable pointer at `*this`.
    const void* const* vtable_field = nullptr;
    // Second argument passed unchanged to both calls.
    void* argument = nullptr;
};

struct Vtable15f2f08Slot38_e16a00Hooks {
    void (*call_slot18)(void* self, void* argument) = nullptr;
    void (*tail_slot20)(void* self, void* argument) = nullptr;
};

struct Vtable15f2f08Slot38_e16a00Result {
    bool ok = false;
    bool first_call_modelled = false;
    bool tail_modelled = false;
    const void* vtable = nullptr;
};

[[nodiscard]] Vtable15f2f08Slot38_e16a00Result vtable_15f2f08_slot38_e16a00(
    const Vtable15f2f08Slot38_e16a00Cells& cells = {},
    const Vtable15f2f08Slot38_e16a00Hooks& hooks = {}) noexcept;

}  // namespace sotc
