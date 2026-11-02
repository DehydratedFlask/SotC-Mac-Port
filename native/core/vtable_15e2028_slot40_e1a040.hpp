// Vtable slot resolved by the vtable-closure sweep: `0x15e2028 + 0x40 -> 0xe1a040`.
//
// Evidence:
//   * `0xe1b1f1` is `call *0x40(%rax)` with `%rax` loaded from the RIP-relative table `0x15e2028`;
//   * the qword at `0x15e2028 + 0x40` in the pinned image is `0xe1a040`;
//   * the dynamic relocation table has one type-8 row at `0x15e2068` with addend `0xe1a040`;
//   * `0xe1a040` is an FDE function start with length `0x81`.
//
// Body (129 B) is a name+id lookup over the container:
//   if (key == null) return 0;                       ; `testq %r15,%r15; je zero`
//   if (this->count == 0) return 0;                  ; count at this+0x10
//   for (i = 0; i < this->count; ++i) {
//       element = this->array[i];                    ; array at this+0x8
//       const char* name = element->vtable[0x10/8]();
//       if (name == null) continue;
//       if (strcmp(name, key) != 0) continue;        ; PLT 0xf8f490 = strcmp
//       if ((int)element->vtable[0x18/8]() == value) return 1;
//   }
//   return 0;
//
// Modelled: the two early outs, the scan, the exact `strcmp`, the id comparison and the 0/1 result.
// Not modelled: the element vtable bodies. The element slots are hooks; a missing hook where the
// body would call it is a visible failure, never a silent skip.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e2028Slot40_e1a040Rva = 0xe1a040;
inline constexpr std::size_t kVtable15e2028Slot40_e1a040Size = 0x81;
inline constexpr std::uint64_t kVtable15e2028Slot40_e1a040EndAddress = 0xe1a0c1;
inline constexpr std::uint64_t kVtable15e2028Slot40_e1a040VtableAddress = 0x15e2028;
inline constexpr std::uint32_t kVtable15e2028Slot40_e1a040SlotOffset = 0x40;
inline constexpr std::uint64_t kVtable15e2028Slot40_e1a040RelocAddress = 0x15e2068;
inline constexpr std::uint64_t kVtable15e2028Slot40_e1a040ArrayOffset = 0x08;
inline constexpr std::uint64_t kVtable15e2028Slot40_e1a040CountOffset = 0x10;
inline constexpr std::uint32_t kVtable15e2028Slot40_e1a040NameSlot = 0x10;
inline constexpr std::uint32_t kVtable15e2028Slot40_e1a040IdSlot = 0x18;
inline constexpr std::uint64_t kVtable15e2028Slot40_e1a040StrcmpTarget = 0xf8f490;

struct Vtable15e2028Slot40_e1a040Cells {
    void* self = nullptr;
    // this+0x8: array of element pointers.
    void* const* elements = nullptr;
    // this+0x10: element count.
    const std::uint32_t* count = nullptr;
    // arg1 (rsi): key compared with strcmp (exact, case-sensitive).
    const char* key = nullptr;
    // arg2 (edx): expected id from the element's slot 0x18.
    std::int32_t value = 0;
};

struct Vtable15e2028Slot40_e1a040Hooks {
    const char* (*element_name)(void* element) = nullptr;
    std::int32_t (*element_id)(void* element) = nullptr;
};

struct Vtable15e2028Slot40_e1a040Result {
    bool ok = false;
    bool early_return = false;   // null key or empty container
    std::int32_t code = 0;       // eax: 1 on a match, 0 otherwise
    std::int32_t found_index = -1;
    std::uint32_t elements_scanned = 0;
    std::uint32_t names_compared = 0;
};

[[nodiscard]] Vtable15e2028Slot40_e1a040Result vtable_15e2028_slot40_e1a040(
    const Vtable15e2028Slot40_e1a040Cells& cells = {},
    const Vtable15e2028Slot40_e1a040Hooks& hooks = {}) noexcept;

}  // namespace sotc
