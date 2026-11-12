// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 373 (RVA 0x7081e0, 2165 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 373. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_373.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shadow crosshair / aim debug page: three bool variables, four unbounded float angle limits and five toggles (inference from the strings).
// Rows, in constructor order (12 objects at 0x3244b18..): 3 bool variables (caller functor tag 0x15ccd10, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
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

inline constexpr std::uint64_t kSlot373Rva = 0x7081e0;
inline constexpr std::size_t kSlot373Size = 2165;
inline constexpr std::uint64_t kSlot373BoolFnTag = 0x15ccd10;
inline constexpr std::uint64_t kSlot373FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot373ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot373VarFlags = 1;
inline constexpr std::uint64_t kSlot373UFloatFnTag = 0x15ccd30;
inline constexpr std::size_t kSlot373UFloatCount = 4;
inline constexpr std::size_t kSlot373BoolCount = 3;
inline constexpr std::size_t kSlot373FloatCount = 0;
inline constexpr std::size_t kSlot373RowCount = 12;
inline constexpr std::size_t kSlot373StringCount = 1;
inline constexpr std::size_t kSlot373SliderCount = 0;
inline constexpr std::uint64_t kSlot373Object0Rva = 0x3244b18;
inline constexpr std::size_t kSlot373CellCount = 0;
inline constexpr std::size_t kSlot373HolderCount = 0;
inline constexpr char kSlot373BoolNames[kSlot373BoolCount][48] = {"shadow_enableInitialAimAlongCamera", "shadow_addNoiseToCrosshairWhenRiding", "cp10_constrainCrosshairAngleWhenLocked"};
inline constexpr std::size_t kSlot373BoolNameLengths[kSlot373BoolCount] = {0x22, 0x24, 0x26};
inline constexpr std::uint8_t kSlot373BoolValues[kSlot373BoolCount] = {1, 0, 1};
inline constexpr char kSlot373UFloatNames[kSlot373UFloatCount][48] = {"cp10_constrainCrosshairAngleWhenLocked_MinYaw", "cp10_constrainCrosshairAngleWhenLocked_MaxYaw", "cp10_constrainCrosshairAngleWhenLocked_MinPitch", "cp10_constrainCrosshairAngleWhenLocked_MaxPitch"};
inline constexpr std::size_t kSlot373UFloatNameLengths[kSlot373UFloatCount] = {0x2d, 0x2d, 0x2f, 0x2f};
inline constexpr std::uint32_t kSlot373UFloatValueBits[kSlot373UFloatCount] = {0xc1f00000, 0x41a00000, 0xc1500000, 0x41500000};
inline constexpr std::uint8_t kSlot373UFloatEarly[kSlot373UFloatCount] = {1, 1, 1, 1};
inline constexpr std::int32_t kSlot373UFloatDesc[kSlot373UFloatCount] = {-1, -1, -1, -1};
inline constexpr char kSlot373MenuStrings[kSlot373StringCount][14] = {"Shadow Camera"};
inline constexpr std::size_t kSlot373MenuStringLengths[kSlot373StringCount] = {0xd};

struct Slot373Objects {
    ConCmdBoolVar r00;   // 0x3244b18 bool
    ConCmdBoolVar r01;   // 0x3244be0 bool
    ConCmdBoolVar r02;   // 0x3244ca8 bool
    ConCmdRangedVar r03;   // 0x3244d70 ufloat
    ConCmdRangedVar r04;   // 0x3244e48 ufloat
    ConCmdRangedVar r05;   // 0x3244f20 ufloat
    ConCmdRangedVar r06;   // 0x3244ff8 ufloat
    DebugMenuItem r07;   // 0x32450d0 tog
    DebugMenuItem r08;   // 0x3245178 tog
    DebugMenuItem r09;   // 0x3245220 tog
    DebugMenuItem r10;   // 0x32452c8 tog
    DebugMenuItem r11;   // 0x3245370 tog
};

struct Slot373Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot373Result init_slot373_7081e0(Slot373Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
