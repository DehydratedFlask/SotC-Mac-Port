// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 382 (RVA 0x6c8600, 1870 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 382. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_382.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shadow Items / ancient-bow glow debug page: a VitalPointFocusTime name cell, one bool and four bounded float variables, four toggles (inference from the strings).
// Rows, in constructor order (10 objects at 0x32398e0..): 1 bool variables (caller functor tag 0x15cc818, dtor 0x20b0), 4 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cc7f8, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu parent paths are literals; every item has flags 0x30.
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot382Rva = 0x6c8600;
inline constexpr std::size_t kSlot382Size = 1870;
inline constexpr std::uint64_t kSlot382BoolFnTag = 0x15cc818;
inline constexpr std::uint64_t kSlot382FloatFnTag = 0x15cc7f8;
inline constexpr std::uint32_t kSlot382ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot382VarFlags = 1;
inline constexpr std::uint64_t kSlot382UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot382UFloatCount = 0;
inline constexpr std::size_t kSlot382BoolCount = 1;
inline constexpr std::size_t kSlot382FloatCount = 4;
inline constexpr std::size_t kSlot382RowCount = 10;
inline constexpr std::size_t kSlot382StringCount = 3;
inline constexpr std::size_t kSlot382SliderCount = 0;
inline constexpr std::uint64_t kSlot382Object0Rva = 0x32398e0;
inline constexpr std::size_t kSlot382CellCount = 1;
inline constexpr std::size_t kSlot382HolderCount = 0;
inline constexpr char kSlot382BoolNames[kSlot382BoolCount][30] = {"cp10_debugAncientBow"};
inline constexpr std::size_t kSlot382BoolNameLengths[kSlot382BoolCount] = {0x14};
inline constexpr std::uint8_t kSlot382BoolValues[kSlot382BoolCount] = {0};
inline constexpr char kSlot382FloatNames[kSlot382FloatCount][30] = {"shadow_ancientBowGlowAmount", "shadow_ancientBowGlowMaxAngle", "cp10_bowGlowValueOverride", "cp10_bowGlowInterpSpeed"};
inline constexpr std::size_t kSlot382FloatNameLengths[kSlot382FloatCount] = {0x1b, 0x1d, 0x19, 0x17};
inline constexpr std::uint32_t kSlot382FloatLoBits[kSlot382FloatCount] = {0x00000000, 0x00000000, 0xbdcccccd, 0x00000000};
inline constexpr std::uint32_t kSlot382FloatHiBits[kSlot382FloatCount] = {0x3f800000, 0x43340000, 0x3f800000, 0x3f800000};
inline constexpr std::uint32_t kSlot382FloatValueBits[kSlot382FloatCount] = {0x00000000, 0x3f000000, 0xbdcccccd, 0x3a83126f};
inline constexpr char kSlot382MenuStrings[kSlot382StringCount][20] = {"VitalPointFocusTime", "Shadow Items", "Shadow Player"};
inline constexpr std::size_t kSlot382MenuStringLengths[kSlot382StringCount] = {0x13, 0xc, 0xd};

struct Slot382Objects {
    NameHashCell r00;   // 0x32398e0 cell
    ConCmdRangedVar r01;   // 0x32398f0 float
    ConCmdRangedVar r02;   // 0x32399c8 float
    ConCmdBoolVar r03;   // 0x3239aa0 bool
    ConCmdRangedVar r04;   // 0x3239b68 float
    ConCmdRangedVar r05;   // 0x3239c40 float
    DebugMenuItem r06;   // 0x3239d18 tog
    DebugMenuItem r07;   // 0x3239dc0 tog
    DebugMenuItem r08;   // 0x3239e68 tog
    DebugMenuItem r09;   // 0x3239f10 tog
};

struct Slot382Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot382Result init_slot382_6c8600(Slot382Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
