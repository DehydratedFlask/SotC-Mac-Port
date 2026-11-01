// Resolved vtable target `0xe19980` (reached from vtable `0x15e1ee0` slot `0xb0`).
//
// Evidence: `0xe11c64` is `call *0xb0(%rax)` with `%rax` from the RIP-relative table `0x15e1ee0`;
// the qword at slot `0xb0` is `0xe19980`; the dynamic relocation at `0x15e1f90` is type 8 with
// addend `0xe19980`; `0xe19980` is an FDE function start of length `0xa2`.
//
// Body (162 B) is a two-path wrapper with a stack canary:
//   saved = canary (0x15fe518)
//   if (arg3 & 7) != 0            -> slow path
//   else if (arg4 & 7) == 0       -> fast path
//   else                          -> slow path
//   slow: snprintf-like format (0xb30570) into a 0x100 stack buffer, then
//         element->vtable[0xd8/8](element, buffer); returns 0
//   fast: helper(arg3, arg2, arg4, arg5) (0xe1eda0), then *arg3 = *(global 0x3353478);
//         returns arg3
//   both: canary compare, 0xf8f3a0 on mismatch
//
// Modelled: the alignment decision, both paths as hooks, the fast-path global store, and the canary
// check. Not modelled: the snprintf formatting, the 0xd8 callee, the helper body and the global's
// meaning. A null canary cell fails visibly because the retail body always reads and compares it.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980Rva = 0xe19980;
inline constexpr std::size_t kVtable15e1ee0SlotB0_e19980Size = 0xa2;
inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980EndAddress = 0xe19a22;
inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980VtableAddress = 0x15e1ee0;
inline constexpr std::uint32_t kVtable15e1ee0SlotB0_e19980SlotOffset = 0xb0;
inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980RelocAddress = 0x15e1f90;
inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980CanaryAddress = 0x15fe518;
inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980FormatRva = 0xb30570;
inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980FastHelperRva = 0xe1eda0;
inline constexpr std::uint64_t kVtable15e1ee0SlotB0_e19980FastStoreGlobal = 0x3353478;
inline constexpr std::uint32_t kVtable15e1ee0SlotB0_e19980SlowSlot = 0xd8;
inline constexpr std::uint32_t kVtable15e1ee0SlotB0_e19980AlignmentMask = 0x7;

struct Vtable15e1ee0SlotB0_e19980Args {
    void* arg2 = nullptr;  // rsi
    void* arg3 = nullptr;  // rdx, alignment-checked and written on the fast path
    void* arg4 = nullptr;  // rcx, alignment-checked
    void* arg5 = nullptr;  // r8
};

struct Vtable15e1ee0SlotB0_e19980Cells {
    // Host storage for the stack canary value. Null is fail-visible.
    std::uint64_t* canary = nullptr;
    // Host storage for the fast-path global store target (`*arg3`), and the value to store.
    void** fast_store_target = nullptr;
    const void* const* fast_store_value = nullptr;
};

struct Vtable15e1ee0SlotB0_e19980Hooks {
    // Slow path: element->vtable slot 0xd8 with (element, formatted buffer).
    void (*slow_vcall_d8)(void* element, void* buffer) = nullptr;
    // Buffer handed to the slow hook; must hold at least 0x100 bytes when supplied.
    void* slow_buffer = nullptr;
    // Fast path helper 0xe1eda0.
    void (*fast_helper)(void* arg3, void* arg2, void* arg4, void* arg5) = nullptr;
};

enum class Vtable15e1ee0SlotB0_e19980Path : std::uint8_t { none = 0, slow = 1, fast = 2 };

const char* vtable_15e1ee0_slot_b0_e19980_path_name(
    Vtable15e1ee0SlotB0_e19980Path path) noexcept;

struct Vtable15e1ee0SlotB0_e19980Result {
    bool ok = false;
    bool canary_checked = false;
    bool canary_mismatch = false;
    bool path_modelled = false;
    bool stored = false;
    Vtable15e1ee0SlotB0_e19980Path path = Vtable15e1ee0SlotB0_e19980Path::none;
};

[[nodiscard]] Vtable15e1ee0SlotB0_e19980Result vtable_15e1ee0_slot_b0_e19980(
    const Vtable15e1ee0SlotB0_e19980Args& args = {},
    const Vtable15e1ee0SlotB0_e19980Cells& cells = {},
    const Vtable15e1ee0SlotB0_e19980Hooks& hooks = {}) noexcept;

}  // namespace sotc
