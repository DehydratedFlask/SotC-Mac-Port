// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 398 (RVA 0x63cf60, 1408 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 398. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_398.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// CP10 animation display toggles plus two local-int spew variables (shadow_spewMotShift / shadow_spewSubMotShift; inference from the strings).
// Rows, in constructor order (8 objects at 0x300f888..): 2 bool variables (caller functor tag 0x15cc4e8, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: CP10-Anim and Shadow Misc toggles.
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
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot398Rva = 0x63cf60;
inline constexpr std::size_t kSlot398Size = 1408;
inline constexpr std::uint64_t kSlot398BoolFnTag = 0x15cc4e8;
inline constexpr std::uint64_t kSlot398FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot398ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot398VarFlags = 1;
inline constexpr std::uint64_t kSlot398UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot398UFloatCount = 0;
inline constexpr std::size_t kSlot398BoolCount = 2;
inline constexpr std::size_t kSlot398FloatCount = 0;
inline constexpr std::size_t kSlot398RowCount = 8;
inline constexpr std::size_t kSlot398StringCount = 4;
inline constexpr std::uint64_t kSlot398IntFnTag = 0x0;
inline constexpr std::size_t kSlot398IntCount = 0;
inline constexpr std::size_t kSlot398LocalIntCount = 2;
inline constexpr std::size_t kSlot398SliderCount = 0;
inline constexpr std::uint64_t kSlot398Object0Rva = 0x300f888;
inline constexpr std::size_t kSlot398CellCount = 0;
inline constexpr std::size_t kSlot398HolderCount = 0;
inline constexpr char kSlot398BoolNames[kSlot398BoolCount][30] = {"CP10_AnimDisplayBlendWindows", "CP10_AnimForceWrapAnimeFinish"};
inline constexpr std::size_t kSlot398BoolNameLengths[kSlot398BoolCount] = {0x1c, 0x1d};
inline constexpr std::uint8_t kSlot398BoolValues[kSlot398BoolCount] = {0, 1};
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
inline constexpr LocalIntSiteSpec kSlot398LocalIntSites[kSlot398LocalIntCount] = {{0x529160, "shadow_spewMotShift", 19, 0, 0, 4, 1, 0x15cc508, false}, {0x529160, "shadow_spewSubMotShift", 22, 0, 0, 4, 1, 0x15cc508, false}};
inline constexpr char kSlot398MenuStrings[kSlot398StringCount][27] = {"CP10-Anim", "Shadow Misc", "Spew Character MotShift", "Spew Character SubMotShift"};
inline constexpr std::size_t kSlot398MenuStringLengths[kSlot398StringCount] = {0x9, 0xb, 0x17, 0x1a};

struct Slot398Objects {
    ConCmdBoolVar r00;   // 0x300f888 bool
    DebugMenuItem r01;   // 0x300f950 tog
    ConCmdBoolVar r02;   // 0x300f9f8 bool
    DebugMenuItem r03;   // 0x300fac0 tog
    ConCmdRangedVar r04;   // 0x300fb68 lint
    ConCmdRangedVar r05;   // 0x300fc40 lint
    DebugMenuItem r06;   // 0x300fd18 toge
    DebugMenuItem r07;   // 0x300fdc0 toge
};

struct Slot398Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot398Result init_slot398_63cf60(Slot398Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
