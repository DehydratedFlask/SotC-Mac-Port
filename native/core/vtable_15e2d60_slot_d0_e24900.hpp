// Resolved vtable target `0xe24900` (reached from vtable `0x15e2d60` slot `0xd0`).
//
// Evidence: sites `0xe1ee0d` and `0xe1ee31` are `call *0xd0(%rax)` with `%rax` from the RIP-relative
// table `0x15e2d60`; the qword at `0x15e2d60 + 0xd0 = 0x15e2e30` is `0xe24900`; the dynamic
// relocation there is type 8 with addend `0xe24900`; `0xe24900` is an FDE function start of length
// `0x84` (132 B).
//
// Body shape:
//   sub   = *(this + 0x8)
//   fn40  = sub->vtable[0x40]
//   a     = this->vtable[0x28]()                 ; slot 0x28
//   index = this->vtable[0x48]()                 ; slot 0x48 -> eax
//   result = fn40(sub, a, index)                 ; call through sub slot 0x40
//   *arg1 = result
//   if (result == null) return 0x1b
//   status = result->vtable[0xb8](result, this)  ; slot 0xb8
//   if (status == 0) { *arg1 = null; return 0; }
//   (*arg1)->vtable[0x10]();
//   *arg1 = null; return status;
//
// Modelled: the five calls in that order, the store into `*arg1`, the null early return, and the
// clear-then-return paths. Not modelled: every callee body. The sub-object and the result are real
// arguments, so missing hooks (or a null sub with a non-null store target) fail visibly.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e2d60SlotD0_e24900Rva = 0xe24900;
inline constexpr std::size_t kVtable15e2d60SlotD0_e24900Size = 0x84;
inline constexpr std::uint64_t kVtable15e2d60SlotD0_e24900EndAddress = 0xe24984;
inline constexpr std::uint64_t kVtable15e2d60SlotD0_e24900VtableAddress = 0x15e2d60;
inline constexpr std::uint32_t kVtable15e2d60SlotD0_e24900SlotOffset = 0xd0;
inline constexpr std::uint64_t kVtable15e2d60SlotD0_e24900RelocAddress = 0x15e2e30;
inline constexpr std::uint64_t kVtable15e2d60SlotD0_e24900SubOffset = 0x08;
inline constexpr std::uint32_t kVtable15e2d60SlotD0_e24900SlotA = 0x28;
inline constexpr std::uint32_t kVtable15e2d60SlotD0_e24900SlotIndex = 0x48;
inline constexpr std::uint32_t kVtable15e2d60SlotD0_e24900SlotSubCall = 0x40;
inline constexpr std::uint32_t kVtable15e2d60SlotD0_e24900SlotStatus = 0xb8;
inline constexpr std::uint32_t kVtable15e2d60SlotD0_e24900SlotRelease = 0x10;
inline constexpr std::int32_t kVtable15e2d60SlotD0_e24900StatusNull = 0x1b;

struct Vtable15e2d60SlotD0_e24900Cells {
    void* self = nullptr;
    void* sub = nullptr;           // *(this + 0x8)
    void** out = nullptr;          // *arg1
};

struct Vtable15e2d60SlotD0_e24900Hooks {
    void* (*slot28)(void* self) = nullptr;
    std::uint32_t (*slot48)(void* self) = nullptr;
    void* (*sub_slot40)(void* sub, void* a, std::uint32_t index) = nullptr;
    std::int32_t (*result_slot_b8)(void* result, void* self) = nullptr;
    void (*result_slot10)(void* result) = nullptr;
};

struct Vtable15e2d60SlotD0_e24900Result {
    bool ok = false;
    std::int32_t status = 0;
    bool early_null = false;
    void* result = nullptr;
    bool status_hook_called = false;
    bool release_hook_called = false;
    bool out_cleared = false;
};

[[nodiscard]] Vtable15e2d60SlotD0_e24900Result vtable_15e2d60_slot_d0_e24900(
    const Vtable15e2d60SlotD0_e24900Cells& cells = {},
    const Vtable15e2d60SlotD0_e24900Hooks& hooks = {}) noexcept;

}  // namespace sotc
