// Resolved vtable target `0xe26f90` (reached from vtable `0x15ed2a0` slot `0xc0`).
//
// Evidence: sites `0xe52b97`, `0xe52bb3`, `0xe52bd2` are `call *0xc0(%rax)` with `%rax` from the
// RIP-relative table `0x15ed2a0`; the qword at `0x15ed2a0 + 0xc0 = 0x15ed360` is `0xe26f90`; the
// dynamic relocation there is type 8 with addend `0xe26f90`; `0xe26f90` is an FDE start of length
// `0xb7` (183 B).
//
// Body shape (with a stack canary):
//   h      = this->vtable[0x78]()                 ; stored in the frame
//   status = this->vtable[0x90](name, &buf)       ; name at 0x131706d
//   if (status != 0) return 0;                    ; `xor eax,eax`
//   return helper(this, arg2, arg3, &buf, &local) ; helper 0xe255b0
//   canary compare on exit.
//
// Modelled: the slot 0x78 fetch, the slot 0x90 status call, the early zero return and the helper call
// as ordered hooks, plus the canary. Not modelled: the three callees and the buffer contents.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15ed2a0SlotC0_e26f90Rva = 0xe26f90;
inline constexpr std::size_t kVtable15ed2a0SlotC0_e26f90Size = 0xb7;
inline constexpr std::uint64_t kVtable15ed2a0SlotC0_e26f90EndAddress = 0xe27047;
inline constexpr std::uint64_t kVtable15ed2a0SlotC0_e26f90VtableAddress = 0x15ed2a0;
inline constexpr std::uint32_t kVtable15ed2a0SlotC0_e26f90SlotOffset = 0xc0;
inline constexpr std::uint64_t kVtable15ed2a0SlotC0_e26f90RelocAddress = 0x15ed360;
inline constexpr std::uint64_t kVtable15ed2a0SlotC0_e26f90CanaryAddress = 0x15fe518;
inline constexpr std::uint64_t kVtable15ed2a0SlotC0_e26f90NameRva = 0x131706d;
inline constexpr std::uint32_t kVtable15ed2a0SlotC0_e26f90FetchSlot = 0x78;
inline constexpr std::uint32_t kVtable15ed2a0SlotC0_e26f90StatusSlot = 0x90;
inline constexpr std::uint64_t kVtable15ed2a0SlotC0_e26f90HelperRva = 0xe255b0;

struct Vtable15ed2a0SlotC0_e26f90Cells {
    void* self = nullptr;
    void* arg2 = nullptr;
    std::int32_t arg3 = 0;
    std::uint64_t* canary = nullptr;
    void* buffer = nullptr;   // at least 0x10 bytes when supplied
    void* local = nullptr;    // at least 4 bytes when supplied
};

struct Vtable15ed2a0SlotC0_e26f90Hooks {
    std::uint64_t (*slot78)(void* self) = nullptr;
    std::int32_t (*slot90)(void* self, const char* name, void* buffer) = nullptr;
    std::int32_t (*helper)(void* self, void* arg2, std::int32_t arg3, void* buffer,
                           void* local) = nullptr;
};

struct Vtable15ed2a0SlotC0_e26f90Result {
    bool ok = false;
    bool status_called = false;
    bool early_zero = false;
    bool helper_called = false;
    bool canary_checked = false;
    bool canary_mismatch = false;
    std::uint64_t fetched = 0;
    std::int32_t status = 0;
};

[[nodiscard]] Vtable15ed2a0SlotC0_e26f90Result vtable_15ed2a0_slot_c0_e26f90(
    const Vtable15ed2a0SlotC0_e26f90Cells& cells = {},
    const Vtable15ed2a0SlotC0_e26f90Hooks& hooks = {}) noexcept;

}  // namespace sotc
