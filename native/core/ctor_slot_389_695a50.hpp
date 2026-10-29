// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 389 (RVA 0x695a50, 386 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 389. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_389.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: the bool variable `shadow_mungeFishFinCloth` (object 0x3015ac8, flags 1, functor tag 0x15cc690, value and
// default 1) and its debug-menu toggle built by 0x26db0 (item 0x3015b90, parent path `Shadow Cloth`, flags 0x30).
// Sequence: bool-variable initialiser (0x20af0, f590, 0x20d60, f590, atexit 0x20b0) ; 0x26db0 ; f590 ; atexit 0x272d0.
// What the variable gates is NOT recovered here (observation: construction only).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot389Rva = 0x695a50;
inline constexpr std::size_t kSlot389Size = 386;
inline constexpr char kSlot389VarName[] = "shadow_mungeFishFinCloth";
inline constexpr char kSlot389Parent[] = "Shadow Cloth";
inline constexpr std::uint64_t kSlot389FnTag = 0x15cc690;

struct Slot389Objects {
    ConCmdBoolVar var;       // 0x3015ac8
    DebugMenuItem item;      // 0x3015b90
};

struct Slot389Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot389Result init_slot389_695a50(Slot389Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
