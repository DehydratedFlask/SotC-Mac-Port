// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 514 (RVA 0x1c4c50, 739 bytes). Original ELF sha256
// d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 514. Every fact below is byte-pinned by
// tools/lift/check_ctor_slot_514.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, the
// functor tag and the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) bool variable `_CameraPrintJumpCuts` (object 0x2ee68d8, tag 0x1598f50, value 0, default 0, empty
// description, flags 1); (2) bool variable `broadcastActiveCameraToGameConnection` (object 0x2ee69a0, value 1, default 1);
// (3) a debug-menu toggle built by the 0x26db0 constructor (item 0x2ee6a68) for the first variable under the parent path
// `Camera`, its caption taken from the variable's name, flags 0x30, f590(parent), atexit(0x272d0); (4) the same for the second
// variable (item 0x2ee6b10). Unlike slots 366 / 411 the two variables are built back to back, then the two toggles. The
// 0x268b0 registration is the tail of 0x26db0 and already native (init_toggle_site_26db0); 0x26db0 is the bool-class adapter
// here (target class 0x1594218, slot 3 -> 1, slot 6 -> 0; tools/lift/check_helper_26db0.py), so there is no functor slot-3
// call site. The __stack_chk_guard compare is a hook (not modelled), as in every other lane-M6 bool slot.
//
// NOT established: what reads the two variables at run time. Reading from the strings: a camera jump-cut print switch and a
// broadcast-active-camera switch; INFERENCE only, no consumer is lifted.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot514Rva = 0x1c4c50;
inline constexpr std::size_t kSlot514Size = 739;
inline constexpr std::uint64_t kSlot514FnTag = 0x1598f50;
inline constexpr char kSlot514Var0Name[] = "_CameraPrintJumpCuts";
inline constexpr std::size_t kSlot514Var0NameLength = 0x14;
inline constexpr char kSlot514Var1Name[] = "broadcastActiveCameraToGameConnection";
inline constexpr std::size_t kSlot514Var1NameLength = 0x25;
inline constexpr char kSlot514Parent[] = "Camera";
inline constexpr std::size_t kSlot514ParentLength = 6;
inline constexpr std::uint64_t kSlot514Var0Rva = 0x2ee68d8;
inline constexpr std::uint64_t kSlot514Item0Rva = 0x2ee6a68;
inline constexpr std::uint64_t kSlot514Var1Rva = 0x2ee69a0;
inline constexpr std::uint64_t kSlot514Item1Rva = 0x2ee6b10;

struct Slot514Objects {
    ConCmdBoolVar var0;      // 0x2ee68d8
    ConCmdBoolVar var1;      // 0x2ee69a0
    DebugMenuItem item0;     // 0x2ee6a68
    DebugMenuItem item1;     // 0x2ee6b10
};

struct Slot514Result {
    bool ok = false;
    BoolVarResult var0{};
    MenuSiteResult item0{};
    BoolVarResult var1{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot514Result init_slot514_1c4c50(Slot514Objects& objects, ConCmdRegistry& concmd,
                                    DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
