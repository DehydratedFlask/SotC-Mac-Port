// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 360 (RVA 0x7806f0, 1815 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 360. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_360.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shadow Player sword-light debug page: four bounded float variables, four toggles, a name holder and a name cell (inference from the strings).
// Rows, in constructor order (10 objects at 0x3255448..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 4 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cdf18, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
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

inline constexpr std::uint64_t kSlot360Rva = 0x7806f0;
inline constexpr std::size_t kSlot360Size = 1815;
inline constexpr std::uint64_t kSlot360BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot360FloatFnTag = 0x15cdf18;
inline constexpr std::uint32_t kSlot360ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot360VarFlags = 1;
inline constexpr std::uint64_t kSlot360UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot360UFloatCount = 0;
inline constexpr std::size_t kSlot360BoolCount = 0;
inline constexpr std::size_t kSlot360FloatCount = 4;
inline constexpr std::size_t kSlot360RowCount = 10;
inline constexpr std::size_t kSlot360StringCount = 3;
inline constexpr std::size_t kSlot360SliderCount = 0;
inline constexpr std::uint64_t kSlot360Object0Rva = 0x3255448;
inline constexpr std::uint64_t kSlot360HolderVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot360HolderDtorRva = 0x8ef00;
inline constexpr std::size_t kSlot360CellCount = 1;
inline constexpr std::size_t kSlot360HolderCount = 1;
inline constexpr char kSlot360FloatNames[kSlot360FloatCount][33] = {"cp10_swordInLightInfluence", "cp10_swordBlendValueOverride", "cp10_swordBlendLineValueOverride", "cp10_swordInLightInterpSpeed"};
inline constexpr std::size_t kSlot360FloatNameLengths[kSlot360FloatCount] = {0x1a, 0x1c, 0x20, 0x1c};
inline constexpr std::uint32_t kSlot360FloatLoBits[kSlot360FloatCount] = {0x00000000, 0xbdcccccd, 0xbdcccccd, 0x00000000};
inline constexpr std::uint32_t kSlot360FloatHiBits[kSlot360FloatCount] = {0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000};
inline constexpr std::uint32_t kSlot360FloatValueBits[kSlot360FloatCount] = {0x3f800000, 0xbdcccccd, 0xbdcccccd, 0x3e99999a};
inline constexpr char kSlot360MenuStrings[kSlot360StringCount][21] = {"Shadow Player", "cp10_debugSwordLight", "VitalPointFocusTime"};
inline constexpr std::size_t kSlot360MenuStringLengths[kSlot360StringCount] = {0xd, 0x14, 0x13};

struct Slot360Objects {
    ConCmdRangedVar r00;   // 0x3255448 float
    ConCmdRangedVar r01;   // 0x3255520 float
    ConCmdRangedVar r02;   // 0x32555f8 float
    ConCmdRangedVar r03;   // 0x32556d0 float
    DebugMenuItem r04;   // 0x32557a8 tog
    DebugMenuItem r05;   // 0x3255850 tog
    DebugMenuItem r06;   // 0x32558f8 tog
    DebugMenuItem r07;   // 0x32559a0 tog
    NameHolder30 r08;   // 0x3255a48 holder
    NameHashCell r09;   // 0x3255a78 cell
};

struct Slot360Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot360Result init_slot360_7806f0(Slot360Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
