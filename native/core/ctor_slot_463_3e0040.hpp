// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 463 (RVA 0x3e0040, 410 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 463 is at 0x2dd70e0 - 463 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_463.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) bool variable `cp10_playerClothDebug` (object 0x2ef72b0, length 21, value 0, default 0,
// flags 1, empty description, functor tag 0x15b1828): 0x20af0, f590(description), 0x20d60, f590(name),
// __cxa_atexit(0x20b0) -- native as init_bool_var_site; (2) a menu toggle through the NEW constructor 0x26db0
// (item 0x2ef7378, the object directly after the 0xc8-byte variable) under the parent path `Shadow Player`
// (length 13), its caption taken from the variable's name; flags 0x30; f590(parent); __cxa_atexit(0x272d0) --
// native as init_toggle_site_26db0. OBSERVATION only: what reads the variable (a cloth debug switch, by its
// name) is NOT modelled here. The first debug-menu registration initialises the root list and records
// __cxa_atexit(0x27280).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot463Rva = 0x3e0040;
inline constexpr std::size_t kSlot463Size = 410;
inline constexpr char kSlot463Name[] = "cp10_playerClothDebug";
inline constexpr char kSlot463Parent[] = "Shadow Player";
inline constexpr std::uint64_t kSlot463FnTag = 0x15b1828;
inline constexpr std::uint64_t kSlot463ObjectRva = 0x2ef72b0;
inline constexpr std::uint64_t kSlot463ItemRva = 0x2ef7378;
inline constexpr std::uint8_t kSlot463Value = 0;
inline constexpr std::uint8_t kSlot463Default = 0;

struct Slot463Objects {
    ConCmdBoolVar var;      // 0x2ef72b0
    DebugMenuItem item;     // 0x2ef7378
};

struct Slot463Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot463Result init_slot463_3e0040(Slot463Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
