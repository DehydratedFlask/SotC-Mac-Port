// Resolved vtable target `0xe1a5c0` (reached from vtable `0x15e2028` slot `0x70`).
//
// Evidence: `0xe119d3` is `call *0x70(%rax)` with `%rax` from the RIP-relative table `0x15e2028`;
// the qword at slot `0x70` is `0xe1a5c0`; the dynamic relocation at `0x15e2098` is type 8 with
// addend `0xe1a5c0`; `0xe1a5c0` is an FDE function start of length `0x97`.
//
// Body (151 B) is a lookup over a container:
//   if (arg1 == null) return 1;                       ; null key
//   if (this->count == 0) return 1;                   ; count at this+0x10
//   for (i = 0; i < this->count; ++i) {
//       element = this->array[i];                     ; array at this+0x8
//       const char* key = element->vtable[0x10/8]();  ; slot 0x10
//       if (key == null) continue;
//       if (strcmp(key, arg1) != 0) continue;         ; PLT 0xf8f490 = strcmp
//       if ((int)element->vtable[0x18/8]() != arg2) continue;   ; slot 0x18
//       return element->vtable[0x20/8]();             ; tail call slot 0x20
//   }
//   return 1;
//
// Modelled: the boundary checks, the scan, the exact `strcmp` comparison, the value comparison and
// the tail call as a hook. Not modelled: the element vtable bodies. The element slots are called
// once per element, so a missing key/value hook fails visibly instead of silently skipping.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e2028Slot70_e1a5c0Rva = 0xe1a5c0;
inline constexpr std::size_t kVtable15e2028Slot70_e1a5c0Size = 0x97;
inline constexpr std::uint64_t kVtable15e2028Slot70_e1a5c0EndAddress = 0xe1a657;
inline constexpr std::uint64_t kVtable15e2028Slot70_e1a5c0VtableAddress = 0x15e2028;
inline constexpr std::uint32_t kVtable15e2028Slot70_e1a5c0SlotOffset = 0x70;
inline constexpr std::uint64_t kVtable15e2028Slot70_e1a5c0RelocAddress = 0x15e2098;
inline constexpr std::uint64_t kVtable15e2028Slot70_e1a5c0CountOffset = 0x10;
inline constexpr std::uint64_t kVtable15e2028Slot70_e1a5c0ArrayOffset = 0x08;
inline constexpr std::uint32_t kVtable15e2028Slot70_e1a5c0KeySlot = 0x10;
inline constexpr std::uint32_t kVtable15e2028Slot70_e1a5c0ValueSlot = 0x18;
inline constexpr std::uint32_t kVtable15e2028Slot70_e1a5c0MatchSlot = 0x20;
inline constexpr std::uint64_t kVtable15e2028Slot70_e1a5c0StrcmpTarget = 0xf8f490;

struct Vtable15e2028Slot70_e1a5c0Cells {
    void* self = nullptr;
    // this+0x8: array of element pointers.
    void* const* elements = nullptr;
    // this+0x10: element count.
    const std::uint32_t* count = nullptr;
    // arg1: key to compare with strcmp (exact, case-sensitive).
    const char* key = nullptr;
    // arg2: expected value from the element's slot 0x18.
    std::int32_t value = 0;
};

struct Vtable15e2028Slot70_e1a5c0Hooks {
    const char* (*element_key)(void* element) = nullptr;
    std::int32_t (*element_value)(void* element) = nullptr;
    std::int32_t (*element_match)(void* element) = nullptr;
};

struct Vtable15e2028Slot70_e1a5c0Result {
    bool ok = false;
    bool early_return = false;   // null key or empty container
    std::int32_t code = 0;       // 1 when not found
    std::int32_t found_index = -1;
    std::uint32_t elements_scanned = 0;
    bool match_called = false;
};

[[nodiscard]] Vtable15e2028Slot70_e1a5c0Result vtable_15e2028_slot70_e1a5c0(
    const Vtable15e2028Slot70_e1a5c0Cells& cells = {},
    const Vtable15e2028Slot70_e1a5c0Hooks& hooks = {}) noexcept;

}  // namespace sotc
