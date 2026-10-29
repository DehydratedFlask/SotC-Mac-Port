// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 403 (RVA 0x5f0160, 386 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 403. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_403.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: the bool variable `shadow_drawModelCharacterClasses` (object 0x300c168, flags 1, functor tag 0x15cc078,
// value and default 0) and its 0x26db0 menu toggle (item 0x300c230, parent path `Shadow Misc`, flags 0x30).
// Same construction as slot 389 with different constants.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot403Rva = 0x5f0160;
inline constexpr std::size_t kSlot403Size = 386;
inline constexpr char kSlot403VarName[] = "shadow_drawModelCharacterClasses";
inline constexpr char kSlot403Parent[] = "Shadow Misc";
inline constexpr std::uint64_t kSlot403FnTag = 0x15cc078;

struct Slot403Objects {
    ConCmdBoolVar var;       // 0x300c168
    DebugMenuItem item;      // 0x300c230
};

struct Slot403Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot403Result init_slot403_5f0160(Slot403Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
