// CPU-01-STARTUP-SLOT-468: native model of the 0x90-byte SCRIPTED-COMMAND registry entry
// constructor 0x3c2a00 (398 B, FDE [0x3c2a00, 0x3c2b8e), sha256 3e00391a...a79e59).
//
// Entry layout (0x90 B, a global in .bss):
//   +0x00 / +0x20 / +0x40  three 0x20-byte descriptors {self, fn, zero, flag};
//                          when descriptor.flag != 0 the copied self is replaced by the
//                          address of its own descriptor (`movq %rbx,(%rbx)` and the two
//                          `leaq +0x20/+0x40` forms);
//   +0x60 SharedStr name (copied, atomic `lock inc [ctl]` when ctl != 0);
//   +0x80 previous global list head (0x2ef6400), then [0x2ef6400] = this entry;
//   +0x88 is a tail cell whose ADDRESS is the descriptor self value used by the five
//         slot-468 entries; 0x3c2a00 never writes the cell itself.
//
// When the registry cell 0x2ef63f8 is non-null, the helper performs the lower_bound walk
// (strcasecmp 0xf8f500) and calls the RB find-or-insert 0x3c0840 only when the key is absent
// or maps to null; a non-null mapped entry is left untouched. The tree, allocator and string
// comparator are the first-class rb_registry_3be2d0 module. Vtables/functors are identity tags.
//
// Offline only: this model never executes an original RVA.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "hashed_string_14c3a0.hpp"
#include "rb_registry_3be2d0.hpp"

namespace sotc {

inline constexpr std::uint64_t kScriptedEntryCtorRva = 0x3c2a00;
inline constexpr std::size_t kScriptedEntryCtorSize = 398;
inline constexpr std::uint64_t kScriptedEntryListHeadCellRva = 0x2ef6400;
inline constexpr std::uint64_t kScriptedEntryRegistryCellRva = 0x2ef63f8;
inline constexpr std::uint64_t kScriptedEntryStrcasecmpPltRva = 0xf8f500;
inline constexpr std::size_t kScriptedEntrySize = 0x90;

struct ScriptedEntryDescriptor {
    std::uint64_t self;      // +0x00 copied to the entry; replaced by &descriptor when flag != 0
    std::uint64_t fn;        // +0x08 identity tag (functor address)
    std::uint64_t zero10;    // +0x10
    std::uint64_t flag18;    // +0x18 non-zero selects the self-pointer rule
};
static_assert(sizeof(ScriptedEntryDescriptor) == 0x20);

struct ScriptedEntry {
    ScriptedEntryDescriptor groups[3];   // +0x00 / +0x20 / +0x40
    SharedStr name;                      // +0x60
    ScriptedEntry* next;                 // +0x80 previous list head
    std::uint64_t tail88;                // +0x88 never written by 0x3c2a00
};
static_assert(sizeof(ScriptedEntry) == kScriptedEntrySize &&
              offsetof(ScriptedEntry, name) == 0x60 && offsetof(ScriptedEntry, next) == 0x80 &&
              offsetof(ScriptedEntry, tail88) == 0x88);

struct ScriptedEntryResult {
    bool ok = false;
    bool linked = false;             // the entry was pushed on the global list
    bool registry_present = false;   // *0x2ef63f8 was non-null
    bool inserted = false;           // *0x3c0840(key) = entry
    bool already_present = false;    // a non-null mapped value already existed
};

// Fails closed (ok == false after linking) when the registry is present but has no allocator
// or the RB insert reports a failure; the entry remains linked exactly as the original linked
// it before the registry call.
[[nodiscard]] ScriptedEntryResult init_scripted_entry_3c2a00(
    ScriptedEntry& entry, const std::array<ScriptedEntryDescriptor, 3>& descriptors,
    const SharedStr& name, ScriptedEntry*& list_head, RbMap* registry,
    const RbHooks& hooks = {}) noexcept;

}  // namespace sotc
