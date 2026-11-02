// Vtable slot resolved by the vtable-closure sweep: `0x15e2028 + 0x38 -> 0xe19f70`.
//
// Evidence:
//   * `0xe1b204` is `call *0x38(%rax)` with `%rax` loaded from the RIP-relative table `0x15e2028`;
//   * the qword at `0x15e2028 + 0x38` in the pinned image is `0xe19f70`;
//   * the dynamic relocation table has one type-8 row at `0x15e2060` with addend `0xe19f70`;
//   * `0xe19f70` is an FDE function start with length `0xc5`.
//
// Body (197 B) is a name scan over the same container shape as `0xe1a040`:
//   if (key == null) return 0;                       ; `testq %r14,%r14; je zero`
//   if (this->count == 0) return 0;                  ; count at this+0x10, array at this+0x8
//   found = false; best = 0;
//   for (i = 0; i < this->count; ++i) {
//       element = this->array[i];
//       const char* name = element->vtable[0x10/8]();
//       if (name == null) continue;
//       if (strcmp(name, key) != 0) continue;        ; PLT 0xf8f490 = strcmp
//       if (!found) { found = true; best = i; continue; }
//       const int current = element->vtable[0x18/8]();
//       const int best_id = this->array[best]->vtable[0x18/8]();
//       if (current > best_id) best = i;             ; `cmpl %eax,%ebx; jbe next`
//   }
//   return found ? 1 : 0;                            ; `andb $0x1,%r15b`
//
// The max-id scan does not change the return value (only the found flag is returned), so the model
// reports the picked index and the id calls as observable fields instead of inventing a result the
// body does not produce. Modelled: the early outs, the scan, the exact `strcmp`, the id comparisons
// and the call order. Not modelled: the element vtable bodies.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15e2028Slot38_e19f70Rva = 0xe19f70;
inline constexpr std::size_t kVtable15e2028Slot38_e19f70Size = 0xc5;
inline constexpr std::uint64_t kVtable15e2028Slot38_e19f70EndAddress = 0xe1a035;
inline constexpr std::uint64_t kVtable15e2028Slot38_e19f70VtableAddress = 0x15e2028;
inline constexpr std::uint32_t kVtable15e2028Slot38_e19f70SlotOffset = 0x38;
inline constexpr std::uint64_t kVtable15e2028Slot38_e19f70RelocAddress = 0x15e2060;
inline constexpr std::uint64_t kVtable15e2028Slot38_e19f70ArrayOffset = 0x08;
inline constexpr std::uint64_t kVtable15e2028Slot38_e19f70CountOffset = 0x10;
inline constexpr std::uint32_t kVtable15e2028Slot38_e19f70NameSlot = 0x10;
inline constexpr std::uint32_t kVtable15e2028Slot38_e19f70IdSlot = 0x18;
inline constexpr std::uint64_t kVtable15e2028Slot38_e19f70StrcmpTarget = 0xf8f490;

struct Vtable15e2028Slot38_e19f70Cells {
    void* self = nullptr;
    // this+0x8: array of element pointers.
    void* const* elements = nullptr;
    // this+0x10: element count.
    const std::uint32_t* count = nullptr;
    // arg1 (rsi): key compared with strcmp (exact, case-sensitive).
    const char* key = nullptr;
};

struct Vtable15e2028Slot38_e19f70Hooks {
    const char* (*element_name)(void* element) = nullptr;
    std::int32_t (*element_id)(void* element) = nullptr;
};

struct Vtable15e2028Slot38_e19f70Result {
    bool ok = false;
    bool early_return = false;    // null key or empty container
    std::int32_t code = 0;        // eax: found ? 1 : 0
    bool found = false;
    std::int32_t best_index = -1;
    std::int32_t best_id = 0;
    std::uint32_t elements_scanned = 0;
    std::uint32_t name_matches = 0;
    std::uint32_t id_calls = 0;
};

[[nodiscard]] Vtable15e2028Slot38_e19f70Result vtable_15e2028_slot38_e19f70(
    const Vtable15e2028Slot38_e19f70Cells& cells = {},
    const Vtable15e2028Slot38_e19f70Hooks& hooks = {}) noexcept;

}  // namespace sotc
