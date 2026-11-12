// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 399 (RVA 0x632ca0, 4642 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 399. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_399.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Player-controls page (inference from the strings): save-volume / persist-move-level / controller-assignment variables, four 0x27050 sliders, 0x26ea0 toggles and nine name-hash cells.
// Rows, in constructor order (31 objects at 0x300e778..): 6 bool variables (caller functor tag 0x15cc468, dtor 0x20b0), 4 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cc488, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: Shadow Misc / Shadow Player / Shadow Controls / Grip toggles and 0x27050 sliders.
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

inline constexpr std::uint64_t kSlot399Rva = 0x632ca0;
inline constexpr std::size_t kSlot399Size = 4642;
inline constexpr std::uint64_t kSlot399BoolFnTag = 0x15cc468;
inline constexpr std::uint64_t kSlot399FloatFnTag = 0x15cc488;
inline constexpr std::uint32_t kSlot399ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot399VarFlags = 1;
inline constexpr std::uint64_t kSlot399UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot399UFloatCount = 0;
inline constexpr std::size_t kSlot399BoolCount = 6;
inline constexpr std::size_t kSlot399FloatCount = 4;
inline constexpr std::size_t kSlot399RowCount = 31;
inline constexpr std::size_t kSlot399StringCount = 23;
inline constexpr std::uint64_t kSlot399IntFnTag = 0x0;
inline constexpr std::size_t kSlot399IntCount = 0;
inline constexpr std::size_t kSlot399LocalIntCount = 2;
inline constexpr std::size_t kSlot399SliderCount = 4;
inline constexpr std::uint64_t kSlot399UIntFnTag = 0x0;
inline constexpr std::size_t kSlot399UIntCount = 0;
inline constexpr std::size_t kSlot399CmdCount = 0;
inline constexpr std::uint64_t kSlot399Object0Rva = 0x300e778;
inline constexpr std::size_t kSlot399CellCount = 9;
inline constexpr std::size_t kSlot399HolderCount = 0;
inline constexpr char kSlot399BoolNames[kSlot399BoolCount][40] = {"saveUseFacingDirection", "cp10_enablePersistMoveLevel", "cp10_debugPersistMoveLevel", "cp10_playControlDebugDrawInput", "shadow_controls_edgeEndDetection", "cp10_enableModernAiming"};
inline constexpr std::size_t kSlot399BoolNameLengths[kSlot399BoolCount] = {0x16, 0x1b, 0x1a, 0x1e, 0x20, 0x17};
inline constexpr std::uint8_t kSlot399BoolValues[kSlot399BoolCount] = {1, 1, 0, 0, 1, 1};
inline constexpr char kSlot399FloatNames[kSlot399FloatCount][40] = {"cp10_persistMoveLevelHoldTime", "cp10_playControlCharacterWorldTurnBlend", "shadow_controls_doubleTapThreshold", "shadow_controls_weakLandingThreshold"};
inline constexpr std::size_t kSlot399FloatNameLengths[kSlot399FloatCount] = {0x1d, 0x27, 0x22, 0x24};
inline constexpr std::uint32_t kSlot399FloatLoBits[kSlot399FloatCount] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
inline constexpr std::uint32_t kSlot399FloatHiBits[kSlot399FloatCount] = {0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000};
inline constexpr std::uint32_t kSlot399FloatValueBits[kSlot399FloatCount] = {0x3de147ae, 0x3f000000, 0x3e800000, 0x3f4ccccd};
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
inline constexpr LocalIntSiteSpec kSlot399LocalIntSites[kSlot399LocalIntCount] = {{0x529160, "shadow_spewCtrlAsgn", 19, 0, 0, 4, 1, 0x15cc4a8, false}, {0x529160, "shadow_spewSubCtrlAsgn", 22, 0, 0, 4, 1, 0x15cc4a8, false}};
inline constexpr char kSlot399MenuStrings[kSlot399StringCount][40] = {"Shadow Misc", "Spew Character CtrlAsgn", "Spew Character SubCtrlAsgn", "Shadow Player", "Persist Jump Momentum Enable", "Persist Jump Momentum Debug", "Persist Jump Momentum stick return time", "Shadow Controls", "Double-tap threshold (s)", "PlayableCtrl Debug Draw Input", "PlayableCtrl Char-World Turn Blend", "Weak landing detection threshold", "Grip", "Detect Ledge endings", "InSaveVolume", "LastInSaveVolume", "FacingSaveVolume", "HighestElevation", "DistanceOnFoot", "DistanceOnHorseback", "TotalPlayTimeEASY", "TotalPlayTimeNORMAL", "TotalPlayTimeHARD"};
inline constexpr std::size_t kSlot399MenuStringLengths[kSlot399StringCount] = {0xb, 0x17, 0x1a, 0xd, 0x1c, 0x1b, 0x27, 0xf, 0x18, 0x1d, 0x22, 0x20, 0x4, 0x14, 0xc, 0x10, 0x10, 0x10, 0xe, 0x13, 0x11, 0x13, 0x11};
inline constexpr std::uint32_t kSlot399SliderLoBits[kSlot399SliderCount] = {0x3a83126f, 0x3a83126f, 0x3c23d70a, 0x3a83126f};
inline constexpr std::uint32_t kSlot399SliderHiBits[kSlot399SliderCount] = {0x3c23d70a, 0x3c23d70a, 0x3dcccccd, 0x3c23d70a};

struct Slot399Objects {
    ConCmdBoolVar r00;   // 0x300e778 bool
    ConCmdBoolVar r01;   // 0x300e840 bool
    ConCmdBoolVar r02;   // 0x300e908 bool
    ConCmdRangedVar r03;   // 0x300e9d0 float
    ConCmdRangedVar r04;   // 0x300eaa8 float
    ConCmdBoolVar r05;   // 0x300eb80 bool
    ConCmdRangedVar r06;   // 0x300ec48 float
    ConCmdRangedVar r07;   // 0x300ed20 float
    ConCmdBoolVar r08;   // 0x300edf8 bool
    ConCmdRangedVar r09;   // 0x300eec0 lint
    ConCmdRangedVar r10;   // 0x300ef98 lint
    DebugMenuItem r11;   // 0x300f070 toge
    DebugMenuItem r12;   // 0x300f118 toge
    DebugMenuItem r13;   // 0x300f1c0 toge
    DebugMenuItem r14;   // 0x300f268 toge
    DebugMenuItem r15;   // 0x300f310 slider50
    DebugMenuItem r16;   // 0x300f3b8 slider50
    DebugMenuItem r17;   // 0x300f460 toge
    DebugMenuItem r18;   // 0x300f508 slider50
    DebugMenuItem r19;   // 0x300f5b0 slider50
    DebugMenuItem r20;   // 0x300f658 toge
    std::uint8_t gap21[0x8];
    ConCmdBoolVar r21;   // 0x300f708 bool
    NameHashCell r22;   // 0x300f7d0 cell
    NameHashCell r23;   // 0x300f7e0 cell
    NameHashCell r24;   // 0x300f7f0 cell
    std::uint8_t gap25[0x8];
    NameHashCell r25;   // 0x300f808 cell
    NameHashCell r26;   // 0x300f818 cell
    std::uint8_t gap27[0x10];
    NameHashCell r27;   // 0x300f838 cell
    std::uint8_t gap28[0x10];
    NameHashCell r28;   // 0x300f858 cell
    NameHashCell r29;   // 0x300f868 cell
    NameHashCell r30;   // 0x300f878 cell
};

struct Slot399Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot399Result init_slot399_632ca0(Slot399Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
