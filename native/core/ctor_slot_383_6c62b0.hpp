// CPU-01-STARTUP-SLOT-383 (lane M1): native model of startup constructor slot 383 (RVA 0x6c62b0, 386 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 383 (checker-pinned). Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by
// tools/lift/check_ctor_slot_383.py. NOTHING here executes, loads or calls an original RVA: vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) bool console variable `shadow_debugDrawCloth` (object 0x3239648, value 0, default 0, flags 1, functor
// tag 0x15cc7d8) -- the inline 0x20af0 / 0x20d60 / 0xf590 / atexit(0x20b0) sequence already native as
// init_bool_var_site; (2) a debug-menu toggle built by 0x26db0 (item 0x3239710) under the parent path
// `Shadow Cloth` (length 0xc), its caption taken from the variable's name, flags 0x30; the toggle is registered
// through its tail call 0x268b0, then f590(parent) and atexit(0x272d0, item). Same two-stage shape as slot 344.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot383Rva = 0x6c62b0;
inline constexpr std::size_t kSlot383Size = 386;
inline constexpr char kSlot383VarName[] = "shadow_debugDrawCloth";
inline constexpr char kSlot383Parent[] = "Shadow Cloth";
inline constexpr std::uint64_t kSlot383FnTag = 0x15cc7d8;
inline constexpr std::uint64_t kSlot383ObjectRva = 0x3239648;
inline constexpr std::uint64_t kSlot383ItemRva = 0x3239710;

struct Slot383Objects {
    ConCmdBoolVar var;     // 0x3239648
    DebugMenuItem item;    // 0x3239710
};

struct Slot383Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot383Result init_slot383_6c62b0(Slot383Objects& objects, ConCmdRegistry& concmd,
                                                    DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
