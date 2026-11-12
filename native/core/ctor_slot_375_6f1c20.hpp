// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 375 (RVA 0x6f1c20, 12802 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 375. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_375.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shadow horse debug page: avoid-player and turning tuning (inference from the strings).
// Rows, in constructor order (61 objects at 0x3241900..): 11 bool variables (caller functor tag 0x15cc9d8, dtor 0x20b0), 23 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cc9f8, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
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

inline constexpr std::uint64_t kSlot375Rva = 0x6f1c20;
inline constexpr std::size_t kSlot375Size = 12802;
inline constexpr std::uint64_t kSlot375BoolFnTag = 0x15cc9d8;
inline constexpr std::uint64_t kSlot375FloatFnTag = 0x15cc9f8;
inline constexpr std::uint32_t kSlot375ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot375VarFlags = 1;
inline constexpr std::uint64_t kSlot375UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot375UFloatCount = 0;
inline constexpr std::size_t kSlot375BoolCount = 11;
inline constexpr std::size_t kSlot375FloatCount = 23;
inline constexpr std::size_t kSlot375RowCount = 61;
inline constexpr std::size_t kSlot375StringCount = 20;
inline constexpr std::size_t kSlot375SliderCount = 3;
inline constexpr std::uint64_t kSlot375Object0Rva = 0x3241900;
inline constexpr std::size_t kSlot375CellCount = 0;
inline constexpr std::size_t kSlot375HolderCount = 0;
inline constexpr char kSlot375BoolNames[kSlot375BoolCount][61] = {"shadow_doHorseAvoidPlayer", "shadow_debugHorseAvoidPlayer", "shadow_printHorsePlayerRestTimer", "shadow_playHorseKickFailSE", "shadow_horseNeuterDuringBossEnable", "shadow_horseNeuterDuringBossDebug", "shadow_horseDebugWarpToPlayer", "shadow_horseDrawn_shareRatioOverride_debug", "shadow_horseDrawn_shareRatioOverride_horse", "shadow_horseDrawn_shareRatioOverride_player", "shadow_horse_automaticWallCtrlInterpolation"};
inline constexpr std::size_t kSlot375BoolNameLengths[kSlot375BoolCount] = {0x19, 0x1c, 0x20, 0x1a, 0x22, 0x21, 0x1d, 0x2a, 0x2a, 0x2b, 0x2b};
inline constexpr std::uint8_t kSlot375BoolValues[kSlot375BoolCount] = {1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1};
inline constexpr char kSlot375FloatNames[kSlot375FloatCount][61] = {"shadow_horseAvoidPlayerMinDist", "shadow_horseAvoidPlayerMaxDist", "shadow_horseAvoidPlayerMinSpeed", "shadow_horseAvoidPlayerOffsetBySpeed", "shadow_horseAvoidPlayerOffsetByDist", "shadow_horseAvoidPlayerMaxAngle", "shadow_horseAvoidPlayerMaxOffset", "shadow_horseDrawn_shareRatioOverride_horse_current", "shadow_horseDrawn_shareRatioOverride_horse_scale", "shadow_horseDrawn_shareRatioOverride_horse_hopeSpringCoe", "shadow_horseDrawn_shareRatioOverride_horse_hopeSpringMaxVel", "shadow_horseDrawn_shareRatioOverride_player_current", "shadow_horseDrawn_shareRatioOverride_player_scale", "shadow_horseDrawn_shareRatioOverride_player_hopeSpringCoe", "shadow_horseDrawn_shareRatioOverride_player_hopeSpringMaxVel", "shadow_horsePull_shareRatioOverride_player_current", "shadow_horse_automaticWallCtrlInterpolation_scale_walk", "shadow_horse_automaticWallCtrlInterpolation_scale_run", "shadow_horse_automaticWallCtrlInterpolation_hopeSpringCoe", "shadow_horse_automaticWallCtrlInterpolation_hopeSpringMaxVel", "shadow_horseFkLimitNodeBlendDistMin", "shadow_horseFkLimitNodeBlendDistMax", "shadow_horseFkLimitNodeDistBlendBewteenYandXYZ"};
inline constexpr std::size_t kSlot375FloatNameLengths[kSlot375FloatCount] = {0x1e, 0x1e, 0x1f, 0x24, 0x23, 0x1f, 0x20, 0x32, 0x30, 0x38, 0x3b, 0x33, 0x31, 0x39, 0x3c, 0x32, 0x36, 0x35, 0x39, 0x3c, 0x23, 0x23, 0x2e};
inline constexpr std::uint32_t kSlot375FloatLoBits[kSlot375FloatCount] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xbf800000, 0x00000000, 0xff7fffff, 0xff7fffff, 0xbf800000, 0x00000000, 0xff7fffff, 0xff7fffff, 0xbf800000, 0x00000000, 0x00000000, 0xff7fffff, 0xff7fffff, 0x00000000, 0x00000000, 0x00000000};
inline constexpr std::uint32_t kSlot375FloatHiBits[kSlot375FloatCount] = {0x41a00000, 0x41a00000, 0x41a00000, 0x41200000, 0x41200000, 0x42b40000, 0x3f800000, 0x3f800000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x3f800000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x3f800000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x40000000, 0x40000000, 0x3f800000};
inline constexpr std::uint32_t kSlot375FloatValueBits[kSlot375FloatCount] = {0x40c9999a, 0x41400000, 0x41000000, 0x3e19999a, 0x3f866666, 0x41200000, 0x3f19999a, 0x3f800000, 0x40400000, 0x41800000, 0x41800000, 0x3f800000, 0x40000000, 0x3f800000, 0xbf800000, 0x3f800000, 0x40800000, 0x42480000, 0x42800000, 0x42c80000, 0x3e800000, 0x3ecccccd, 0x3f19999a};
inline constexpr char kSlot375MenuStrings[kSlot375StringCount][34] = {"Shadow Horse", "Horse Turning", "ShareRatio Driver", "Debug", "Horse - Override", "Horse -- Current", "Horse -- Scale", "Horse -- Hope Spring Coef", "Horse -- Hope Spring Max Vel", "Player - Override", "Player -- Current", "Player -- Scale", "Player -- Hope Spring Coef", "Player -- Hope Spring Max Vel", "Automatic Wall Ctrl Interpolation", "Use Interpolation", "- Scale - Walk", "- Scale - Run", "- Hope Spring Coef", "- Hope Spring Max Vel"};
inline constexpr std::size_t kSlot375MenuStringLengths[kSlot375StringCount] = {0xc, 0xd, 0x11, 0x5, 0x10, 0x10, 0xe, 0x19, 0x1c, 0x11, 0x11, 0xf, 0x1a, 0x1d, 0x21, 0x11, 0xe, 0xd, 0x12, 0x15};
inline constexpr std::uint32_t kSlot375SliderLoBits[kSlot375SliderCount] = {0x3c23d70a, 0x3c23d70a, 0x3c23d70a};
inline constexpr std::uint32_t kSlot375SliderHiBits[kSlot375SliderCount] = {0x3dcccccd, 0x3dcccccd, 0x3dcccccd};

struct Slot375Objects {
    ConCmdBoolVar r00;   // 0x3241900 bool
    ConCmdBoolVar r01;   // 0x32419c8 bool
    ConCmdRangedVar r02;   // 0x3241a90 float
    ConCmdRangedVar r03;   // 0x3241b68 float
    ConCmdRangedVar r04;   // 0x3241c40 float
    ConCmdRangedVar r05;   // 0x3241d18 float
    ConCmdRangedVar r06;   // 0x3241df0 float
    ConCmdRangedVar r07;   // 0x3241ec8 float
    ConCmdRangedVar r08;   // 0x3241fa0 float
    ConCmdBoolVar r09;   // 0x3242078 bool
    ConCmdBoolVar r10;   // 0x3242140 bool
    ConCmdBoolVar r11;   // 0x3242208 bool
    ConCmdBoolVar r12;   // 0x32422d0 bool
    ConCmdBoolVar r13;   // 0x3242398 bool
    ConCmdBoolVar r14;   // 0x3242460 bool
    ConCmdBoolVar r15;   // 0x3242528 bool
    ConCmdRangedVar r16;   // 0x32425f0 float
    ConCmdRangedVar r17;   // 0x32426c8 float
    ConCmdRangedVar r18;   // 0x32427a0 float
    ConCmdRangedVar r19;   // 0x3242878 float
    ConCmdBoolVar r20;   // 0x3242950 bool
    ConCmdRangedVar r21;   // 0x3242a18 float
    ConCmdRangedVar r22;   // 0x3242af0 float
    ConCmdRangedVar r23;   // 0x3242bc8 float
    ConCmdRangedVar r24;   // 0x3242ca0 float
    ConCmdRangedVar r25;   // 0x3242d78 float
    ConCmdBoolVar r26;   // 0x3242e50 bool
    ConCmdRangedVar r27;   // 0x3242f18 float
    ConCmdRangedVar r28;   // 0x3242ff0 float
    ConCmdRangedVar r29;   // 0x32430c8 float
    ConCmdRangedVar r30;   // 0x32431a0 float
    ConCmdRangedVar r31;   // 0x3243278 float
    ConCmdRangedVar r32;   // 0x3243350 float
    ConCmdRangedVar r33;   // 0x3243428 float
    DebugMenuItem r34;   // 0x3243500 slider
    DebugMenuItem r35;   // 0x32435a8 slider
    DebugMenuItem r36;   // 0x3243650 slider
    DebugMenuItem r37;   // 0x32436f8 tog
    DebugMenuItem r38;   // 0x32437a0 tog
    DebugMenuItem r39;   // 0x3243848 group
    DebugMenuItem r40;   // 0x32438f0 group
    DebugMenuItem r41;   // 0x3243998 toge
    DebugMenuItem r42;   // 0x3243a40 toge
    DebugMenuItem r43;   // 0x3243ae8 toge
    DebugMenuItem r44;   // 0x3243b90 toge
    DebugMenuItem r45;   // 0x3243c38 toge
    DebugMenuItem r46;   // 0x3243ce0 toge
    DebugMenuItem r47;   // 0x3243d88 toge
    DebugMenuItem r48;   // 0x3243e30 toge
    DebugMenuItem r49;   // 0x3243ed8 toge
    DebugMenuItem r50;   // 0x3243f80 toge
    DebugMenuItem r51;   // 0x3244028 toge
    DebugMenuItem r52;   // 0x32440d0 group
    DebugMenuItem r53;   // 0x3244178 toge
    DebugMenuItem r54;   // 0x3244220 toge
    DebugMenuItem r55;   // 0x32442c8 toge
    DebugMenuItem r56;   // 0x3244370 toge
    DebugMenuItem r57;   // 0x3244418 toge
    DebugMenuItem r58;   // 0x32444c0 tog
    DebugMenuItem r59;   // 0x3244568 tog
    DebugMenuItem r60;   // 0x3244610 tog
};

struct Slot375Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot375Result init_slot375_6f1c20(Slot375Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
