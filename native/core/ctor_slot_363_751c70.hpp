// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 363 (RVA 0x751c70, 701 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 363. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_363.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// cp3 animal debug page (menu-less): three local ranged-int variables cp3_animalDebugType / DisplayType / DebugID (inference from the strings).
// Rows, in constructor order (3 objects at 0x3252730..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// No menu rows.
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

inline constexpr std::uint64_t kSlot363Rva = 0x751c70;
inline constexpr std::size_t kSlot363Size = 701;
inline constexpr std::uint64_t kSlot363BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot363FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot363ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot363VarFlags = 1;
inline constexpr std::uint64_t kSlot363UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot363UFloatCount = 0;
inline constexpr std::size_t kSlot363BoolCount = 0;
inline constexpr std::size_t kSlot363FloatCount = 0;
inline constexpr std::size_t kSlot363RowCount = 3;
inline constexpr std::size_t kSlot363StringCount = 0;
inline constexpr std::uint64_t kSlot363IntFnTag = 0x0;
inline constexpr std::size_t kSlot363IntCount = 0;
inline constexpr std::size_t kSlot363LocalIntCount = 3;
inline constexpr std::size_t kSlot363SliderCount = 0;
inline constexpr std::uint64_t kSlot363Object0Rva = 0x3252730;
inline constexpr std::size_t kSlot363CellCount = 0;
inline constexpr std::size_t kSlot363HolderCount = 0;
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
inline constexpr LocalIntSiteSpec kSlot363LocalIntSites[kSlot363LocalIntCount] = {{0x751190, "cp3_animalDebugType", 19, 0, 0, 6, 1, 0x15cd3c8, false}, {0x751550, "cp3_animalDisplayType", 21, 0, 0, 9, 1, 0x15cd460, false}, {0x751910, "cp3_animalDebugID", 17, 20, 0, 20, 1, 0x15cd4f8, false}};
inline constexpr char kSlot363MenuStrings[1][1] = {""};   // no strings in this slot
inline constexpr std::size_t kSlot363MenuStringLengths[1] = {0};

struct Slot363Objects {
    ConCmdRangedVar r00;   // 0x3252730 lint
    ConCmdRangedVar r01;   // 0x3252808 lint
    ConCmdRangedVar r02;   // 0x32528e0 lint
};

struct Slot363Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot363Result init_slot363_751c70(Slot363Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
