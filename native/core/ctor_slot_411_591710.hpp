// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 411 (RVA 0x591710, 692 bytes). Original ELF sha256
// d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 411. Every fact below is byte-pinned by
// tools/lift/check_ctor_slot_411.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, the
// functor tag and the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order (two copies of the slot-441 var + 0x26db0-toggle pair): (1) bool variable `shadow_cameraUsePureLwsOrientation`
// (object 0x2fd98a0, tag 0x15cb168, value 1, default 1, empty description, flags 1); (2) a debug-menu toggle built by the
// 0x26db0 constructor (item 0x2fd9a30) under the parent path `Shadow Camera`, its caption taken from the
// variable's name, flags 0x30, f590(parent), atexit(0x272d0); (3) bool variable `shadow_cameraDebugPureLwsOrientation` (object 0x2fd9968, value 0,
// default 0); (4) its toggle (item 0x2fd9ad8) under the same parent. The 0x268b0 registration is the tail
// of 0x26db0 and already native (init_toggle_site_26db0); 0x26db0 is the bool-class adapter here (target class
// 0x1594218, slot 3 -> 1, slot 6 -> 0; tools/lift/check_helper_26db0.py), so there is no functor slot-3 call site.
// The __stack_chk_guard compare is a hook (not modelled), as in every other lane-M6 bool slot.
//
// NOT established: what reads the two variables at run time. Reading from the strings: two Shadow camera pure-LWS-orientation switches (use / debug); INFERENCE only, no consumer is lifted.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot411Rva = 0x591710;
inline constexpr std::size_t kSlot411Size = 692;
inline constexpr std::uint64_t kSlot411FnTag = 0x15cb168;
inline constexpr char kSlot411Var0Name[] = "shadow_cameraUsePureLwsOrientation";
inline constexpr std::size_t kSlot411Var0NameLength = 0x22;
inline constexpr char kSlot411Var1Name[] = "shadow_cameraDebugPureLwsOrientation";
inline constexpr std::size_t kSlot411Var1NameLength = 0x24;
inline constexpr char kSlot411Parent[] = "Shadow Camera";
inline constexpr std::size_t kSlot411ParentLength = 13;
inline constexpr std::uint64_t kSlot411Var0Rva = 0x2fd98a0;
inline constexpr std::uint64_t kSlot411Item0Rva = 0x2fd9a30;
inline constexpr std::uint64_t kSlot411Var1Rva = 0x2fd9968;
inline constexpr std::uint64_t kSlot411Item1Rva = 0x2fd9ad8;

struct Slot411Objects {
    ConCmdBoolVar var0;      // 0x2fd98a0
    ConCmdBoolVar var1;      // 0x2fd9968
    DebugMenuItem item0;     // 0x2fd9a30
    DebugMenuItem item1;     // 0x2fd9ad8
};

struct Slot411Result {
    bool ok = false;
    BoolVarResult var0{};
    MenuSiteResult item0{};
    BoolVarResult var1{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot411Result init_slot411_591710(Slot411Objects& objects, ConCmdRegistry& concmd,
                                    DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
