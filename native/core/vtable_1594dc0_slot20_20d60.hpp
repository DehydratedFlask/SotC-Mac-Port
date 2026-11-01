// Vtable slot resolved by the indirect-frontier card: `0x1594dc0 + 0x20 -> 0x20d60`.
//
// Evidence:
//   * `0x28471` is `call *0x20(%rax)` and the preceding instruction is `lea 0x1594dc0, %rax`;
//   * the qword at `0x1594dc0 + 0x20` in the pinned image is `0x20d60`;
//   * the dynamic relocation table has exactly one row at that slot (`0x1594de0`, type 8,
//     addend `0x20d60`), so the pointer is a relocated static table entry, not runtime data;
//   * `0x20d60` is an FDE function start with length `0x1a`.
//
// Body (26 B, `80 7f 18 00 74 01 c3 48 8d 05 5a be dd 02 c6 47 18 01 48 8b 30 e9 06 00 00 00`):
//   cmpb $0x0, 0x18(%rdi)          ; "already ran" byte on the object
//   je   +1
//   retq                           ; second and later calls return immediately
//   lea  0x2dfcbc8(%rip), %rax     ; global pointer cell
//   movb $0x1, 0x18(%rdi)          ; set the flag before doing any work
//   mov  (%rax), %rsi              ; first argument for the tail target
//   jmp  0x20d80                   ; tail call, separate FDE function (length 0x188)
//
// What is modelled: the once-flag gate, the flag write, and the tail call as a hook.
// What is NOT modelled: the tail target `0x20d80` itself, the meaning of the global, and any
// argument beyond `rdi` (the body overwrites `rsi` before the jump, so no other input is read).
// ABI status: `void f(This* rdi)` is inferred from this body only, not from a caller contract.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable1594dc0Slot20_20d60Rva = 0x20d60;
inline constexpr std::size_t kVtable1594dc0Slot20_20d60Size = 0x1a;
inline constexpr std::uint64_t kVtable1594dc0Slot20_20d60EndAddress = 0x20d7a;
inline constexpr std::uint64_t kVtable1594dc0Slot20_20d60VtableAddress = 0x1594dc0;
inline constexpr std::uint32_t kVtable1594dc0Slot20_20d60SlotOffset = 0x20;
inline constexpr std::uint64_t kVtable1594dc0Slot20_20d60RelocAddress = 0x1594de0;
inline constexpr std::uint64_t kVtable1594dc0Slot20_20d60OnceFlagOffset = 0x18;
inline constexpr std::uint64_t kVtable1594dc0Slot20_20d60GlobalAddress = 0x2dfcbc8;
inline constexpr std::uint64_t kVtable1594dc0Slot20_20d60TailTargetRva = 0x20d80;

struct Vtable1594dc0Slot20_20d60Cells {
    // Host storage for the object's `+0x18` byte. Null is fail-visible, not "assume unset".
    std::uint8_t* once_flag = nullptr;
    // Host stand-in for the qword at 0x2dfcbc8; null means "no global value available".
    const void* const* global_cell = nullptr;
};

struct Vtable1594dc0Slot20_20d60Hooks {
    // Stands in for the tail call to 0x20d80. The target body is out of scope.
    void (*tail_call)(void* self, void* global_value) = nullptr;
};

struct Vtable1594dc0Slot20_20d60Result {
    bool ok = false;
    bool early_return = false;      // the once flag was already set
    bool flag_written = false;      // this call set the flag
    bool tail_call_modelled = false;
    void* global_value = nullptr;
};

[[nodiscard]] Vtable1594dc0Slot20_20d60Result vtable_1594dc0_slot20_20d60(
    void* self, const Vtable1594dc0Slot20_20d60Cells& cells = {},
    const Vtable1594dc0Slot20_20d60Hooks& hooks = {}) noexcept;

}  // namespace sotc
