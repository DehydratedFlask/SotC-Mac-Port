// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 493 (RVA 0x277560, 1088 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 493. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_493.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// A debug page of bool variables, three of them built by the out-of-line bool constructor 0x274730, with toggle menu items.
// Rows, in constructor order (6 objects at 0x2eef078..): 2 bool variables (caller functor tag 0x159ca60, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// render / optimize debug page
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
#include "startup_row_kinds.hpp"
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_a81700_named_record.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"
#include "helper_274730_bool_var.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot493Rva = 0x277560;
inline constexpr std::size_t kSlot493Size = 1088;
inline constexpr std::uint64_t kSlot493BoolFnTag = 0x159ca60;
inline constexpr std::uint64_t kSlot493FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot493ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot493VarFlags = 1;
inline constexpr std::uint64_t kSlot493UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot493UFloatCount = 0;
inline constexpr std::size_t kSlot493BoolCount = 2;
inline constexpr std::size_t kSlot493FloatCount = 0;
inline constexpr std::size_t kSlot493RowCount = 6;
inline constexpr std::size_t kSlot493StringCount = 3;
inline constexpr std::uint64_t kSlot493IntFnTag = 0x0;
inline constexpr std::size_t kSlot493IntCount = 0;
inline constexpr std::size_t kSlot493LocalIntCount = 0;
inline constexpr std::size_t kSlot493SliderCount = 0;
inline constexpr std::uint64_t kSlot493UIntFnTag = 0x0;
inline constexpr std::size_t kSlot493UIntCount = 0;
inline constexpr std::size_t kSlot493CmdCount = 0;
inline constexpr std::size_t kSlot493StrVarCount = 0;
inline constexpr std::uint64_t kSlot493Object0Rva = 0x2eef078;
inline constexpr std::size_t kSlot493BoolXCount = 1;
inline constexpr std::size_t kSlot493CellCount = 0;
inline constexpr std::size_t kSlot493HolderCount = 0;
inline constexpr std::size_t kSlot493SentinelCount = 0;
inline constexpr char kSlot493BoolNames[kSlot493BoolCount][33] = {"r_verifyStaticModelsHaveLighting", "r_threadRObjectNewAndFree"};
inline constexpr std::size_t kSlot493BoolNameLengths[kSlot493BoolCount] = {0x20, 0x19};
inline constexpr std::uint8_t kSlot493BoolValues[kSlot493BoolCount] = {0, 1};
inline constexpr std::int32_t kSlot493BoolDesc[kSlot493BoolCount] = {-1, -1};
inline constexpr std::uint8_t kSlot493BoolEarly[kSlot493BoolCount] = {1, 1};
// Out-of-line bool variables (0x274730): the call site's name, initial byte, REAL argument-functor class tag (a bool-family table) and argument block {self, q1, q2, q3, payload, aux};
// q[0] is the block's own address (rewritten into the object by the self-reference rule), q[1] is a .text RVA read from a relocated global, q[3] a callback RVA. All identity-only.
inline constexpr BoolVar274730SiteSpec kSlot493BoolXSites[kSlot493BoolXCount] = {{"r_EnableXPR", 0xb, 1, 0x159ca60, {0, 0x274710, 0x0, 0x26fcf0}, 0x0, 0x0}};
inline constexpr char kSlot493MenuStrings[kSlot493StringCount][31] = {"BakedLighting", "VerifyStaticModelsHaveLighting", "Optimize"};
inline constexpr std::size_t kSlot493MenuStringLengths[kSlot493StringCount] = {0xd, 0x1e, 0x8};

struct Slot493Objects {
    ConCmdBoolVar r00;   // 0x2eef078 bool
    DebugMenuItem r01;   // 0x2eef140 toge
    ConCmdBoolVar r02;   // 0x2eef1e8 boolx
    ConCmdBoolVar r03;   // 0x2eef2b0 bool
    DebugMenuItem r04;   // 0x2eef378 tog
    DebugMenuItem r05;   // 0x2eef420 tog
};

struct Slot493Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot493Result init_slot493_277560(Slot493Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
