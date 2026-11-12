// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 561 (RVA 0x3ac00, 1808 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 561. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_561.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Threading/JS-stats page (inference from the strings): js_PrintStats / js_EnableTracking bools, js_StackCheckerStride ranged int, js_WipeStackMemory unbounded int and the Systems / Threading toggles.
// Rows, in constructor order (9 objects at 0x2ea4898..): 2 bool variables (caller functor tag 0x1595640, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: Systems / Threading group with four 0x26ea0 toggles (two bool, one ranged int, one unbounded int).
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot561Rva = 0x3ac00;
inline constexpr std::size_t kSlot561Size = 1808;
inline constexpr std::uint64_t kSlot561BoolFnTag = 0x1595640;
inline constexpr std::uint64_t kSlot561FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot561ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot561VarFlags = 1;
inline constexpr std::uint64_t kSlot561UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot561UFloatCount = 0;
inline constexpr std::size_t kSlot561BoolCount = 2;
inline constexpr std::size_t kSlot561FloatCount = 0;
inline constexpr std::size_t kSlot561RowCount = 9;
inline constexpr std::size_t kSlot561StringCount = 6;
inline constexpr std::uint64_t kSlot561IntFnTag = 0x1595660;
inline constexpr std::size_t kSlot561IntCount = 1;
inline constexpr std::size_t kSlot561LocalIntCount = 0;
inline constexpr std::size_t kSlot561SliderCount = 0;
inline constexpr std::uint64_t kSlot561UIntFnTag = 0x1595660;
inline constexpr std::size_t kSlot561UIntCount = 1;
inline constexpr std::size_t kSlot561CmdCount = 0;
inline constexpr std::uint64_t kSlot561Object0Rva = 0x2ea4898;
inline constexpr std::size_t kSlot561CellCount = 0;
inline constexpr std::size_t kSlot561HolderCount = 0;
inline constexpr char kSlot561BoolNames[kSlot561BoolCount][22] = {"js_PrintStats", "js_EnableTracking"};
inline constexpr std::size_t kSlot561BoolNameLengths[kSlot561BoolCount] = {0xd, 0x11};
inline constexpr std::uint8_t kSlot561BoolValues[kSlot561BoolCount] = {0, 0};
inline constexpr char kSlot561IntNames[kSlot561IntCount][22] = {"js_StackCheckerStride"};
inline constexpr std::size_t kSlot561IntNameLengths[kSlot561IntCount] = {0x15};
inline constexpr std::int32_t kSlot561IntMin[kSlot561IntCount] = {1};
inline constexpr std::int32_t kSlot561IntMax[kSlot561IntCount] = {128};
inline constexpr std::int32_t kSlot561IntValue[kSlot561IntCount] = {16};
inline constexpr char kSlot561UIntNames[kSlot561UIntCount][22] = {"js_WipeStackMemory"};
inline constexpr std::size_t kSlot561UIntNameLengths[kSlot561UIntCount] = {0x12};
inline constexpr std::int32_t kSlot561UIntValue[kSlot561UIntCount] = {0};
inline constexpr char kSlot561MenuStrings[kSlot561StringCount][21] = {"Systems", "Threading", "Display Stats", "Enable Tracking", "Stack Checker Stride", "Wipe Stack Memory"};
inline constexpr std::size_t kSlot561MenuStringLengths[kSlot561StringCount] = {0x7, 0x9, 0xd, 0xf, 0x14, 0x11};

struct Slot561Objects {
    ConCmdBoolVar r00;   // 0x2ea4898 bool
    ConCmdBoolVar r01;   // 0x2ea4960 bool
    ConCmdRangedVar r02;   // 0x2ea4a28 int
    ConCmdRangedVar r03;   // 0x2ea4b00 uint
    DebugMenuItem r04;   // 0x2ea4bd8 group
    DebugMenuItem r05;   // 0x2ea4c80 toge
    DebugMenuItem r06;   // 0x2ea4d28 toge
    DebugMenuItem r07;   // 0x2ea4dd0 toge
    DebugMenuItem r08;   // 0x2ea4e78 toge
};

struct Slot561Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot561Result init_slot561_3ac00(Slot561Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
