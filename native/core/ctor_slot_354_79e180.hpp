// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 354 (RVA 0x79e180, 2173 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 354. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_354.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Photo-mode / camera debug page: three bool variables, one bounded float, four unbounded floats (with descriptions) and one 0x26ea0 toggle (inference from the strings).
// Rows, in constructor order (9 objects at 0x32594a8..): 3 bool variables (caller functor tag 0x15ce098, dtor 0x20b0), 1 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15ce0b8, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
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

inline constexpr std::uint64_t kSlot354Rva = 0x79e180;
inline constexpr std::size_t kSlot354Size = 2173;
inline constexpr std::uint64_t kSlot354BoolFnTag = 0x15ce098;
inline constexpr std::uint64_t kSlot354FloatFnTag = 0x15ce0b8;
inline constexpr std::uint32_t kSlot354ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot354VarFlags = 1;
inline constexpr std::uint64_t kSlot354UFloatFnTag = 0x15ce0b8;
inline constexpr std::size_t kSlot354UFloatCount = 4;
inline constexpr std::size_t kSlot354BoolCount = 3;
inline constexpr std::size_t kSlot354FloatCount = 1;
inline constexpr std::size_t kSlot354RowCount = 9;
inline constexpr std::size_t kSlot354StringCount = 4;
inline constexpr std::size_t kSlot354SliderCount = 0;
inline constexpr std::uint64_t kSlot354Object0Rva = 0x32594a8;
inline constexpr std::size_t kSlot354CellCount = 0;
inline constexpr std::size_t kSlot354HolderCount = 0;
inline constexpr char kSlot354BoolNames[kSlot354BoolCount][36] = {"nico_PrintCameraDebugName", "cp10_cameraSelectNextTarget", "shadow_printCameraView"};
inline constexpr std::size_t kSlot354BoolNameLengths[kSlot354BoolCount] = {0x19, 0x1b, 0x16};
inline constexpr std::uint8_t kSlot354BoolValues[kSlot354BoolCount] = {0, 0, 0};
inline constexpr char kSlot354FloatNames[kSlot354FloatCount][36] = {"nico_PhotoModeMaxHorseFocusDistance"};
inline constexpr std::size_t kSlot354FloatNameLengths[kSlot354FloatCount] = {0x23};
inline constexpr std::uint32_t kSlot354FloatLoBits[kSlot354FloatCount] = {0x00000000};
inline constexpr std::uint32_t kSlot354FloatHiBits[kSlot354FloatCount] = {0x42b40000};
inline constexpr std::uint32_t kSlot354FloatValueBits[kSlot354FloatCount] = {0x41f00000};
inline constexpr char kSlot354UFloatNames[kSlot354UFloatCount][36] = {"nico_PhotoModeDOFDistMin", "nico_PhotoModeDOFDistMax", "nico_PhotoModeDOFWidthMin", "nico_PhotoModeDOFWidthMax"};
inline constexpr std::size_t kSlot354UFloatNameLengths[kSlot354UFloatCount] = {0x18, 0x18, 0x19, 0x19};
inline constexpr std::uint32_t kSlot354UFloatValueBits[kSlot354UFloatCount] = {0x00000000, 0x42c80000, 0x00000000, 0x42c80000};
inline constexpr std::uint8_t kSlot354UFloatEarly[kSlot354UFloatCount] = {0, 0, 0, 0};
inline constexpr std::int32_t kSlot354UFloatDesc[kSlot354UFloatCount] = {2, 3, 2, 3};
inline constexpr char kSlot354MenuStrings[kSlot354StringCount][42] = {"Shadow Misc", "Print Camera View", "How small is the DOF dist allowed to get?", "How large is the DOF dist allowed to get?"};
inline constexpr std::size_t kSlot354MenuStringLengths[kSlot354StringCount] = {0xb, 0x11, 0x29, 0x29};

struct Slot354Objects {
    ConCmdBoolVar r00;   // 0x32594a8 bool
    ConCmdBoolVar r01;   // 0x3259570 bool
    ConCmdRangedVar r02;   // 0x3259638 float
    ConCmdBoolVar r03;   // 0x3259710 bool
    DebugMenuItem r04;   // 0x32597d8 toge
    ConCmdRangedVar r05;   // 0x3259880 ufloat
    ConCmdRangedVar r06;   // 0x3259958 ufloat
    ConCmdRangedVar r07;   // 0x3259a30 ufloat
    ConCmdRangedVar r08;   // 0x3259b08 ufloat
};

struct Slot354Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot354Result init_slot354_79e180(Slot354Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
