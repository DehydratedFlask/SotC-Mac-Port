// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 446 (RVA 0x4c92a0, 2334 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 446. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_446.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Scripting/debug page (inference from the strings): bools, a holder, agroColor, a group, a toggle and two scripted-command registry entries (waitForAreaLoader, waitForGameToStart).
// Rows, in constructor order (11 objects at 0x2efa898..): 4 bool variables (caller functor tag 0x15c14f0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// scripting page batch 21: bools, holder, agroColor, group, toggle and two registry entries
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
#include "rb_registry_3be2d0.hpp"
#include "ctor_98a90_3be050.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot446Rva = 0x4c92a0;
inline constexpr std::size_t kSlot446Size = 2334;
inline constexpr std::uint64_t kSlot446BoolFnTag = 0x15c14f0;
inline constexpr std::uint64_t kSlot446FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot446ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot446VarFlags = 1;
inline constexpr std::uint64_t kSlot446UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot446UFloatCount = 0;
inline constexpr std::size_t kSlot446BoolCount = 4;
inline constexpr std::size_t kSlot446FloatCount = 0;
inline constexpr std::size_t kSlot446RowCount = 11;
inline constexpr std::size_t kSlot446StringCount = 7;
inline constexpr std::uint64_t kSlot446IntFnTag = 0x0;
inline constexpr std::size_t kSlot446IntCount = 0;
inline constexpr std::size_t kSlot446LocalIntCount = 1;
inline constexpr std::size_t kSlot446SliderCount = 0;
inline constexpr std::uint64_t kSlot446UIntFnTag = 0x0;
inline constexpr std::size_t kSlot446UIntCount = 0;
inline constexpr std::size_t kSlot446CmdCount = 0;
inline constexpr std::size_t kSlot446StrVarCount = 0;
inline constexpr std::uint64_t kSlot446Object0Rva = 0x2efa898;
inline constexpr std::uint64_t kSlot446HolderVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot446HolderDtorRva = 0x8ef00;
inline constexpr std::size_t kSlot446CellCount = 1;
inline constexpr std::size_t kSlot446HolderCount = 1;
inline constexpr std::size_t kSlot446SentinelCount = 0;
inline constexpr std::size_t kSlot446EntryCount = 2;
inline constexpr std::uint64_t kSlot446EntryDtorRva = 0x3c15e0;
inline constexpr std::size_t kSlot446EntrySize = 0x90;
inline constexpr std::uint64_t kSlot446EntryListHeadCellRva = 0x2ef6400;
inline constexpr std::uint64_t kSlot446EntrySelfCellRva = 0x2ef63f8;
inline constexpr char kSlot446BoolNames[kSlot446BoolCount][27] = {"shadowDemoMode", "shadowPreloadSavePointData", "skipSplash", "forceShowBootOptions"};
inline constexpr std::size_t kSlot446BoolNameLengths[kSlot446BoolCount] = {0xe, 0x1a, 0xa, 0x14};
inline constexpr std::uint8_t kSlot446BoolValues[kSlot446BoolCount] = {0, 1, 0, 0};
inline constexpr std::int32_t kSlot446BoolDesc[kSlot446BoolCount] = {-1, -1, 1, 2};
inline constexpr std::uint8_t kSlot446BoolEarly[kSlot446BoolCount] = {1, 1, 0, 0};
// Scripted-command registry entries (0x3c15e0 is the atexit destructor, an IDENTITY TAG): name, description, the three functor tags stored at +0x08 / +0x28 / +0x48 (relocated code addresses, never run).
inline constexpr char kSlot446EntryNames[kSlot446EntryCount][19] = {"waitForAreaLoader", "waitForGameToStart"};
inline constexpr std::size_t kSlot446EntryNameLengths[kSlot446EntryCount] = {0x11, 0x12};
inline constexpr char kSlot446EntryDescs[kSlot446EntryCount][62] = {"Wait until the the Area Loader finishes loading. Params: None", "Wait until the the Game Starts. Params: None"};
inline constexpr std::size_t kSlot446EntryDescLengths[kSlot446EntryCount] = {0x3d, 0x2c};
inline constexpr std::uint64_t kSlot446EntryFns[3 * kSlot446EntryCount] = {0x4c7a60, 0x4c7ce0, 0x4c7e90, 0x4c7ea0, 0x4c7eb0, 0x4c7f00};
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
// Per-site description: nullptr = the static empty string 0xf730 (never released); a literal = a hand-built borrowed view that the site RELEASES between the wrapper and the functor destroy (the 7ec160 order, release_description = true).
inline constexpr const char* kSlot446LintDesc[kSlot446LocalIntCount] = {"Change Agro's color."};
inline constexpr std::size_t kSlot446LintDescLength[kSlot446LocalIntCount] = {20};
inline constexpr LocalIntSiteSpec kSlot446LocalIntSites[kSlot446LocalIntCount] = {{0x4c8f40, "agroColor", 9, 0, 0, 8, 8, 0x15c1510, true}};
inline constexpr char kSlot446MenuStrings[kSlot446StringCount][88] = {"arealoader_nonlocalarea_force_background", "If true the splash screen sequence will automatically skip.", "If true the boot options will be shown regardless of whether the profile is new or not.", "HorseSkin", "CP10", "Agro", "Horse Color"};
inline constexpr std::size_t kSlot446MenuStringLengths[kSlot446StringCount] = {0x28, 0x3b, 0x57, 0x9, 0x4, 0x4, 0xb};

// Scripted-command registry entry (0x90 bytes of which only [+0x00, +0x88) are ever written; +0x88 is never touched): three {obj+0x88, functor} pairs at +0x00 / +0x20 / +0x40, zeros in between,
// the name SharedStr copied to +0x60 (atomic control increment when a control block is present) and the previous list head at +0x80. The destructor 0x3c15e0 is an IDENTITY TAG.
struct Slot446Entry {
    alignas(8) std::uint8_t b[0x90];
};
static_assert(sizeof(Slot446Entry) == 0x90);

struct Slot446Objects {
    ConCmdBoolVar r00;   // 0x2efa898 bool
    ConCmdBoolVar r01;   // 0x2efa960 bool
    NameHolder30 r02;   // 0x2efaa28 holder
    ConCmdBoolVar r03;   // 0x2efaa58 bool
    ConCmdBoolVar r04;   // 0x2efab20 bool
    NameHashCell r05;   // 0x2efabe8 cell
    ConCmdRangedVar r06;   // 0x2efabf8 lint
    DebugMenuItem r07;   // 0x2efacd0 group
    DebugMenuItem r08;   // 0x2efad78 toge
    std::uint8_t gap09[0x20];
    Slot446Entry r09;   // 0x2efae40 entry
    Slot446Entry r10;   // 0x2efaed0 entry
    RbHooks entry_hooks{};   // HOST INPUT: the allocator behind 0x17070 / 0x170b0 and the strcasecmp import (0xf8f500); used only when the registry object *0x2ef63f8 exists (not at the first startup)
};

struct Slot446Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot446Result init_slot446_4c92a0(Slot446Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
