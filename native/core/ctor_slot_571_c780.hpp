// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 571 (RVA 0xc780, 10644 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 571. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_571.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Player and horse debug page (inference from the strings): shadow_print*/shadow_mountHorse*/shadow_playerSlope variables, BP_JumpToCredits / BP_JumpEnding / BP_JumpToEnd and launchReleaseElf commands, and the CP10 Launch Release command item.
// Rows, in constructor order (44 objects at 0x2dd7628..): 26 bool variables (caller functor tag 0x1594280, dtor 0x20b0), 13 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x1594318, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 14: 20 bools, 11 floats, more bools and floats, five console commands and a 0x26f90 command item
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

namespace sotc {

inline constexpr std::uint64_t kSlot571Rva = 0xc780;
inline constexpr std::size_t kSlot571Size = 10644;
inline constexpr std::uint64_t kSlot571BoolFnTag = 0x1594280;
inline constexpr std::uint64_t kSlot571FloatFnTag = 0x1594318;
inline constexpr std::uint32_t kSlot571ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot571VarFlags = 1;
inline constexpr std::uint64_t kSlot571UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot571UFloatCount = 0;
inline constexpr std::size_t kSlot571BoolCount = 26;
inline constexpr std::size_t kSlot571FloatCount = 13;
inline constexpr std::size_t kSlot571RowCount = 44;
inline constexpr std::size_t kSlot571StringCount = 7;
inline constexpr std::uint64_t kSlot571IntFnTag = 0x0;
inline constexpr std::size_t kSlot571IntCount = 0;
inline constexpr std::size_t kSlot571LocalIntCount = 0;
inline constexpr std::size_t kSlot571SliderCount = 0;
inline constexpr std::uint64_t kSlot571UIntFnTag = 0x0;
inline constexpr std::size_t kSlot571UIntCount = 0;
inline constexpr std::size_t kSlot571CmdCount = 4;
inline constexpr std::size_t kSlot571StrVarCount = 0;
inline constexpr std::uint64_t kSlot571Object0Rva = 0x2dd7628;
inline constexpr std::size_t kSlot571CellCount = 0;
inline constexpr std::size_t kSlot571HolderCount = 0;
inline constexpr std::size_t kSlot571SentinelCount = 0;
inline constexpr char kSlot571BoolNames[kSlot571BoolCount][42] = {"shadow_showPositionInfo", "shadow_showPositionInfoEx", "shadow_playerAutoGrabAgroJumping", "shadow_playerAltRollControls", "shadow_playerAltDiveAttackControls", "shadow_printPlayerSlopeInfo", "shadow_printPlayerHorsePatInfo", "shadow_printPlayerVColAttr", "shadow_printPlayerHColAttr", "shadow_printPlayerParentColAttr", "shadow_debugHorseMount", "shadow_printPlayerControlMode", "shadow_printHorseControlMode", "shadow_printPlayerMotKind", "shadow_printPlayerCamSightYaw", "shadow_printPlayerHorseOffsetZ", "shadow_printPlayerHorseHipOffsetZ", "shadow_printPlayerHorseAngleY", "shadow_printHorsePitch", "shadow_printPlayerToeHeightDiff", "shadow_playerDebugDrawSlope", "shadow_playerEnableAverageSlope", "shadow_playerEnableWalkFixes", "shadow_playerEnableRollThroughWorldFix", "shadow_charRootDynEnableExtraLandingCheck", "shadow_bossShakeDisabled"};
inline constexpr std::size_t kSlot571BoolNameLengths[kSlot571BoolCount] = {0x17, 0x19, 0x20, 0x1c, 0x22, 0x1b, 0x1e, 0x1a, 0x1a, 0x1f, 0x16, 0x1d, 0x1c, 0x19, 0x1d, 0x1e, 0x21, 0x1d, 0x16, 0x1f, 0x1b, 0x1f, 0x1c, 0x26, 0x29, 0x18};
inline constexpr std::uint8_t kSlot571BoolValues[kSlot571BoolCount] = {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0};
inline constexpr std::int32_t kSlot571BoolDesc[kSlot571BoolCount] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
inline constexpr char kSlot571FloatNames[kSlot571FloatCount][42] = {"shadow_mountHorseSideFrontMinZ", "shadow_mountHorseSideBackMaxZ", "shadow_mountHorseSideBack2MaxZ", "shadow_mountHorseJumpBackMaxZ", "shadow_mountHorseBackJumpMaxZ", "shadow_mountHorseBackGroundMaxZ", "shadow_mountHorseBackMinZ", "shadow_mountHorseBackMaxAngleY", "shadow_playerClimbEdgeHeightOverride", "shadow_playerSlip2WallTouchDist", "shadow_playerSlip2WallTouchApproachAngle", "shadow_playerAverageSlopeXZDistance", "shadow_playerAverageSlopeUpperLower"};
inline constexpr std::size_t kSlot571FloatNameLengths[kSlot571FloatCount] = {0x1e, 0x1d, 0x1e, 0x1d, 0x1d, 0x1f, 0x19, 0x1e, 0x24, 0x1f, 0x28, 0x23, 0x23};
inline constexpr std::uint32_t kSlot571FloatLoBits[kSlot571FloatCount] = {0x3c23d70a, 0xc1200000, 0xc1200000, 0xc1200000, 0xc0000000, 0xc0000000, 0xc0000000, 0x00000000, 0x3f333333, 0x00000000, 0x00000000, 0x3c23d70a, 0x3c23d70a};
inline constexpr std::uint32_t kSlot571FloatHiBits[kSlot571FloatCount] = {0x41200000, 0xbc23d70a, 0xbc23d70a, 0xbc23d70a, 0x40000000, 0x40000000, 0x40000000, 0x43340000, 0x3fc00000, 0x40000000, 0x42b40000, 0x3f800000, 0x3f800000};
inline constexpr std::uint32_t kSlot571FloatValueBits[kSlot571FloatCount] = {0x3e99999a, 0xbe99999a, 0xbf19999a, 0xbe800000, 0x00000000, 0xbe3851ec, 0xbf8ccccd, 0x43200000, 0x3f4a3d71, 0x3f800000, 0x42200000, 0x3e4ccccd, 0x3e4ccccd};
inline constexpr char kSlot571CmdNames[kSlot571CmdCount][42] = {"BP_JumpToCredits", "BP_JumpEnding", "BP_JumpToEnd", "launchReleaseElf"};
inline constexpr std::size_t kSlot571CmdNameLengths[kSlot571CmdCount] = {0x10, 0xd, 0xc, 0x10};
inline constexpr std::int32_t kSlot571CmdDesc[kSlot571CmdCount] = {0, 1, 2, 3};
inline constexpr std::uint64_t kSlot571CmdHandler[kSlot571CmdCount] = {0xb730, 0xb780, 0xb7a0, 0xb7f0};
inline constexpr char kSlot571MenuStrings[kSlot571StringCount][62] = {"Jumps to the credits level. Params: None", "Jumps to the end cutscenes. Version used from the debug menu.", "Jumps to the end cutscenes <args: 1-3>", "Launch Shadow Release Elf", "CP10", "Launch Release", "launchReleaseElf"};
inline constexpr std::size_t kSlot571MenuStringLengths[kSlot571StringCount] = {0x28, 0x3d, 0x26, 0x19, 0x4, 0xe, 0x10};

struct Slot571Objects {
    ConCmdBoolVar r00;   // 0x2dd7628 bool
    ConCmdBoolVar r01;   // 0x2dd76f0 bool
    std::uint8_t gap02[0x8];
    ConCmdBoolVar r02;   // 0x2dd77c0 bool
    ConCmdBoolVar r03;   // 0x2dd7888 bool
    ConCmdBoolVar r04;   // 0x2dd7950 bool
    ConCmdBoolVar r05;   // 0x2dd7a18 bool
    ConCmdBoolVar r06;   // 0x2dd7ae0 bool
    ConCmdBoolVar r07;   // 0x2dd7ba8 bool
    ConCmdBoolVar r08;   // 0x2dd7c70 bool
    ConCmdBoolVar r09;   // 0x2dd7d38 bool
    ConCmdBoolVar r10;   // 0x2dd7e00 bool
    ConCmdBoolVar r11;   // 0x2dd7ec8 bool
    ConCmdBoolVar r12;   // 0x2dd7f90 bool
    ConCmdBoolVar r13;   // 0x2dd8058 bool
    ConCmdBoolVar r14;   // 0x2dd8120 bool
    ConCmdBoolVar r15;   // 0x2dd81e8 bool
    ConCmdBoolVar r16;   // 0x2dd82b0 bool
    ConCmdBoolVar r17;   // 0x2dd8378 bool
    ConCmdBoolVar r18;   // 0x2dd8440 bool
    ConCmdBoolVar r19;   // 0x2dd8508 bool
    ConCmdRangedVar r20;   // 0x2dd85d0 float
    ConCmdRangedVar r21;   // 0x2dd86a8 float
    ConCmdRangedVar r22;   // 0x2dd8780 float
    ConCmdRangedVar r23;   // 0x2dd8858 float
    ConCmdRangedVar r24;   // 0x2dd8930 float
    ConCmdRangedVar r25;   // 0x2dd8a08 float
    ConCmdRangedVar r26;   // 0x2dd8ae0 float
    ConCmdRangedVar r27;   // 0x2dd8bb8 float
    ConCmdRangedVar r28;   // 0x2dd8c90 float
    ConCmdRangedVar r29;   // 0x2dd8d68 float
    ConCmdRangedVar r30;   // 0x2dd8e40 float
    ConCmdBoolVar r31;   // 0x2dd8f18 bool
    ConCmdBoolVar r32;   // 0x2dd8fe0 bool
    ConCmdRangedVar r33;   // 0x2dd90a8 float
    ConCmdRangedVar r34;   // 0x2dd9180 float
    ConCmdBoolVar r35;   // 0x2dd9258 bool
    ConCmdBoolVar r36;   // 0x2dd9320 bool
    ConCmdBoolVar r37;   // 0x2dd93e8 bool
    ConCmdBoolVar r38;   // 0x2dd94b0 bool
    std::uint8_t gap39[0x6b0];
    ConCmdCommand r39;   // 0x2dd9c28 cmd
    ConCmdCommand r40;   // 0x2dd9cc8 cmd
    ConCmdCommand r41;   // 0x2dd9d68 cmd
    ConCmdCommand r42;   // 0x2dd9e08 cmd
    DebugMenuItem r43;   // 0x2dd9ea8 cmdi
    const void* external_target_0 = nullptr;   // identity of the object at 0x2dfcbd0 (built by another slot; the host supplies it, the model only stores the pointer)
};

struct Slot571Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot571Result init_slot571_c780(Slot571Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
