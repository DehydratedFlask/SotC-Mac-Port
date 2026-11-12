// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 366 (RVA 0x744ac0, 695 bytes). Original ELF sha256
// d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 366. Every fact below is byte-pinned by
// tools/lift/check_ctor_slot_366.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, the
// functor tag and the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order (two copies of the slot-441 var + 0x26db0-toggle pair): (1) bool variable `shadow_smoothAvoidControlRootY`
// (object 0x3252120, tag 0x15cd200, value 1, default 1, empty description, flags 1); (2) a debug-menu toggle built by the
// 0x26db0 constructor (item 0x32521e8) under the parent path `Shadow IK`, its caption taken from the
// variable's name, flags 0x30, f590(parent), atexit(0x272d0); (3) bool variable `shadow_debugAvoidControlRootY` (object 0x3252290, value 0,
// default 0); (4) its toggle (item 0x3252358) under the same parent. The 0x268b0 registration is the tail
// of 0x26db0 and already native (init_toggle_site_26db0); 0x26db0 is the bool-class adapter here (target class
// 0x1594218, slot 3 -> 1, slot 6 -> 0; tools/lift/check_helper_26db0.py), so there is no functor slot-3 call site.
// The __stack_chk_guard compare is a hook (not modelled), as in every other lane-M6 bool slot.
//
// NOT established: what reads the two variables at run time. Reading from the strings: two Shadow IK avoid-control-root switches (smooth / debug); INFERENCE only, no consumer is lifted.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot366Rva = 0x744ac0;
inline constexpr std::size_t kSlot366Size = 695;
inline constexpr std::uint64_t kSlot366FnTag = 0x15cd200;
inline constexpr char kSlot366Var0Name[] = "shadow_smoothAvoidControlRootY";
inline constexpr std::size_t kSlot366Var0NameLength = 0x1e;
inline constexpr char kSlot366Var1Name[] = "shadow_debugAvoidControlRootY";
inline constexpr std::size_t kSlot366Var1NameLength = 0x1d;
inline constexpr char kSlot366Parent[] = "Shadow IK";
inline constexpr std::size_t kSlot366ParentLength = 9;
inline constexpr std::uint64_t kSlot366Var0Rva = 0x3252120;
inline constexpr std::uint64_t kSlot366Item0Rva = 0x32521e8;
inline constexpr std::uint64_t kSlot366Var1Rva = 0x3252290;
inline constexpr std::uint64_t kSlot366Item1Rva = 0x3252358;

struct Slot366Objects {
    ConCmdBoolVar var0;      // 0x3252120
    DebugMenuItem item0;     // 0x32521e8
    ConCmdBoolVar var1;      // 0x3252290
    DebugMenuItem item1;     // 0x3252358
};

struct Slot366Result {
    bool ok = false;
    BoolVarResult var0{};
    MenuSiteResult item0{};
    BoolVarResult var1{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot366Result init_slot366_744ac0(Slot366Objects& objects, ConCmdRegistry& concmd,
                                    DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
