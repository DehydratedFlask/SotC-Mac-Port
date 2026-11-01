// Resolved vtable target `0xddf4b0` (reached from vtable `0x15e0b48` slot `0x28`).
//
// Evidence: `0xddf017` is `call *0x28(%rax)` with `%rax` from the RIP-relative table `0x15e0b48`;
// the qword at slot `0x28` is `0xddf4b0`, the dynamic relocation at `0x15e0b70` is type 8 with
// addend `0xddf4b0`, and `0xddf4b0` is an FDE function start of length `0x34`.
//
// Body (52 B):
//   push rbp; mov rbp,rsp; push rbx; push rax
//   mov  %rdi, %rbx          ; save the object
//   call 0xddf310            ; unconditional pre-pass
//   test %rbx, %rbx
//   je   0xddf4dd            ; null object -> plain epilogue and return
//   call 0xb2f940            ; fetch a manager/singleton
//   mov  (%rax), %rcx        ; its vtable
//   mov  %rax, %rdi          ; arg0 = manager
//   mov  %rbx, %rsi          ; arg1 = the object
//   mov  0x18(%rcx), %rcx    ; slot 0x18
//   ... epilogue ...
//   jmp  *%rcx               ; tail call through the manager's vtable
//
// Modelled: the pre-pass, the null early return, the manager fetch and the tail call as a hook.
// Not modelled: the pre-pass body, the manager fetch body, and the tail callee. The manager value is
// a real argument, so a missing or null manager fails visibly instead of being skipped.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e0b48Slot28_ddf4b0Rva = 0xddf4b0;
inline constexpr std::size_t kVtable15e0b48Slot28_ddf4b0Size = 0x34;
inline constexpr std::uint64_t kVtable15e0b48Slot28_ddf4b0EndAddress = 0xddf4e4;
inline constexpr std::uint64_t kVtable15e0b48Slot28_ddf4b0VtableAddress = 0x15e0b48;
inline constexpr std::uint32_t kVtable15e0b48Slot28_ddf4b0SlotOffset = 0x28;
inline constexpr std::uint64_t kVtable15e0b48Slot28_ddf4b0RelocAddress = 0x15e0b70;
inline constexpr std::uint64_t kVtable15e0b48Slot28_ddf4b0PreCallRva = 0xddf310;
inline constexpr std::uint64_t kVtable15e0b48Slot28_ddf4b0ManagerFetchRva = 0xb2f940;
inline constexpr std::uint32_t kVtable15e0b48Slot28_ddf4b0ManagerSlot = 0x18;

struct Vtable15e0b48Slot28_ddf4b0Cells {
    void* self = nullptr;
};

struct Vtable15e0b48Slot28_ddf4b0Hooks {
    void (*pre_call_ddf310)(void* self) = nullptr;
    // Stands in for `call 0xb2f940`; must return a non-null manager object.
    void* (*fetch_manager)() = nullptr;
    // Stands in for the tail call through the manager's vtable slot 0x18.
    void (*tail_vcall18)(void* manager, void* self) = nullptr;
};

struct Vtable15e0b48Slot28_ddf4b0Result {
    bool ok = false;
    bool pre_call_modelled = false;
    bool early_return = false;  // null object: the body skips the manager path entirely
    bool tail_modelled = false;
    void* manager = nullptr;
};

[[nodiscard]] Vtable15e0b48Slot28_ddf4b0Result vtable_15e0b48_slot28_ddf4b0(
    const Vtable15e0b48Slot28_ddf4b0Cells& cells = {},
    const Vtable15e0b48Slot28_ddf4b0Hooks& hooks = {}) noexcept;

}  // namespace sotc
