// CPU-01-STARTUP-BATCH-10: native model of startup constructor slot 349 (RVA 0x7bde50, 11443 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 349. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_349.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shadow camera / photo-mode debug page: nine bool variables, 28 bounded float variables, two 0x26db0 toggles, three 0x27120 sliders and seven name holders (inference from the strings).
// Rows, in constructor order (49 objects at 0x325b7b0..): 9 bool variables (caller functor tag 0x15ce2d8, dtor 0x20b0), 28 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15ce2b8, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
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

inline constexpr std::uint64_t kSlot349Rva = 0x7bde50;
inline constexpr std::size_t kSlot349Size = 11443;
inline constexpr std::uint64_t kSlot349BoolFnTag = 0x15ce2d8;
inline constexpr std::uint64_t kSlot349FloatFnTag = 0x15ce2b8;
inline constexpr std::uint32_t kSlot349ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot349VarFlags = 1;
inline constexpr std::uint64_t kSlot349UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot349UFloatCount = 0;
inline constexpr std::size_t kSlot349BoolCount = 9;
inline constexpr std::size_t kSlot349FloatCount = 28;
inline constexpr std::size_t kSlot349RowCount = 49;
inline constexpr std::size_t kSlot349StringCount = 9;
inline constexpr std::size_t kSlot349SliderCount = 3;
inline constexpr std::uint64_t kSlot349Object0Rva = 0x325b7b0;
inline constexpr std::uint64_t kSlot349HolderVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot349HolderDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot349HolderVptrB = 0x159e1a0;
inline constexpr std::uint64_t kSlot349HolderDtorRvaB = 0x2fced0;
inline constexpr std::size_t kSlot349CellCount = 0;
inline constexpr std::size_t kSlot349HolderCount = 7;
inline constexpr char kSlot349BoolNames[kSlot349BoolCount][46] = {"nico_PhotoModeUpdateLookAtAfterGameCameraMode", "nico_PhotoModeForceEnabled", "nico_PhotoModeReorientCameraDataByCollision", "nico_PhotoModeEmergencyTeleportRewind", "shadow_drawPlayerInterestTarget", "shadow_drawCameraLookAtPos", "shadow_printCameraVisibility", "shadow_printAutoRot", "shadow_weapons_disableLockOnWhenAiming"};
inline constexpr std::size_t kSlot349BoolNameLengths[kSlot349BoolCount] = {0x2d, 0x1a, 0x2b, 0x25, 0x1f, 0x1a, 0x1c, 0x13, 0x26};
inline constexpr std::uint8_t kSlot349BoolValues[kSlot349BoolCount] = {1, 0, 1, 1, 0, 0, 0, 0, 1};
inline constexpr char kSlot349FloatNames[kSlot349FloatCount][46] = {"shadow_cameraYawPitchControlAccelScale", "shadow_cameraYawPitchControlMaxSpeedScale", "nico_PhotoModeMinViewAngle", "nico_PhotoModeMaxViewAngle", "nico_PhotoModeViewAngleControlSpeed", "nico_PhotoModeYawPitchControlAccelScale", "nico_PhotoModeYawPitchControlMaxSpeedScale", "nico_PhotoModeMaxBank", "nico_PhotoModeBankDampenRange", "nico_PhotoModeBankControlSpeed", "nico_PhotoModeBankControlAccel", "nico_PhotoModeMinDistToTarget", "nico_PhotoModeMaxDistToTarget", "nico_PhotoModeDollyControlSpeed", "nico_PhotoModeMaxOffsetX", "nico_PhotoModeMaxOffsetY", "nico_PhotoModeOffsetDampenRange", "nico_PhotoModeOffsetControlSpeed", "nico_PhotoModeOffsetControlAccel", "nico_PhotoModeLwsLookDampenRange", "nico_PhotoModeLwsLookControlSpeed", "nico_PhotoModeLwsLookControlAccel", "shadow_deadZoneYawPitchCtrl", "shadow_CameraLookAtRadiusForColliding", "shadow_CameraLookAtRadiusForAvoiding", "shadow_CameraAutoRotDelayTime", "shadow_CameraAutoRotBlendInTime", "shadow_CameraFloorAvoidDist"};
inline constexpr std::size_t kSlot349FloatNameLengths[kSlot349FloatCount] = {0x26, 0x29, 0x1a, 0x1a, 0x23, 0x27, 0x2a, 0x15, 0x1d, 0x1e, 0x1e, 0x1d, 0x1d, 0x1f, 0x18, 0x18, 0x1f, 0x20, 0x20, 0x20, 0x21, 0x21, 0x1b, 0x25, 0x24, 0x1d, 0x1f, 0x1b};
inline constexpr std::uint32_t kSlot349FloatLoBits[kSlot349FloatCount] = {0x3dcccccd, 0x3dcccccd, 0x3f800000, 0x3f800000, 0x3f800000, 0x3dcccccd, 0x3dcccccd, 0x3f800000, 0x3dcccccd, 0x3f800000, 0x3f800000, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3c23d70a, 0x3dcccccd, 0x3f800000, 0x3c23d70a, 0x3dcccccd, 0x3dcccccd, 0x00000000, 0x3d4ccccd, 0x3d4ccccd, 0x00000000, 0x00000000, 0x00000000};
inline constexpr std::uint32_t kSlot349FloatHiBits[kSlot349FloatCount] = {0x41200000, 0x41200000, 0x42b40000, 0x43340000, 0x41f00000, 0x41200000, 0x41200000, 0x42b40000, 0x41200000, 0x41f00000, 0x43340000, 0x41a00000, 0x42200000, 0x41200000, 0x40800000, 0x40800000, 0x41200000, 0x41f00000, 0x43340000, 0x3f800000, 0x40800000, 0x41200000, 0x3f800000, 0x3f800000, 0x40000000, 0x41200000, 0x41200000, 0x3ecccccd};
inline constexpr std::uint32_t kSlot349FloatValueBits[kSlot349FloatCount] = {0x3f800000, 0x3f800000, 0x41700000, 0x42a00000, 0x41700000, 0x40800000, 0x3fd9999a, 0x41f00000, 0x40800000, 0x41a00000, 0x42a00000, 0x3fc00000, 0x41900000, 0x40400000, 0x40000000, 0x3fc00000, 0x3e23d70a, 0x3fcccccd, 0x40cccccd, 0x3e4ccccd, 0x3f000000, 0x40800000, 0x3e3851ec, 0x3e4ccccd, 0x3ecccccd, 0x3f000000, 0x3fc00000, 0x3ecccccd};
inline constexpr char kSlot349MenuStrings[kSlot349StringCount][20] = {"Shadow Camera", "Shadow Photo Mode", "r_dofOverrideEnable", "r_dofNearAmount", "r_dofFarAmount", "r_dofNearStart", "r_dofNearRange", "r_dofFarStart", "r_dofFarRange"};
inline constexpr std::size_t kSlot349MenuStringLengths[kSlot349StringCount] = {0xd, 0x11, 0x13, 0xf, 0xe, 0xe, 0xe, 0xd, 0xd};
inline constexpr std::uint32_t kSlot349SliderLoBits[kSlot349SliderCount] = {0x3dcccccd, 0x3dcccccd, 0x3c23d70a};
inline constexpr std::uint32_t kSlot349SliderHiBits[kSlot349SliderCount] = {0x3f800000, 0x3f800000, 0x3dcccccd};

struct Slot349Objects {
    ConCmdRangedVar r00;   // 0x325b7b0 float
    ConCmdRangedVar r01;   // 0x325b888 float
    ConCmdRangedVar r02;   // 0x325b960 float
    ConCmdRangedVar r03;   // 0x325ba38 float
    ConCmdRangedVar r04;   // 0x325bb10 float
    ConCmdRangedVar r05;   // 0x325bbe8 float
    ConCmdRangedVar r06;   // 0x325bcc0 float
    ConCmdBoolVar r07;   // 0x325bd98 bool
    ConCmdRangedVar r08;   // 0x325be60 float
    ConCmdRangedVar r09;   // 0x325bf38 float
    ConCmdRangedVar r10;   // 0x325c010 float
    ConCmdRangedVar r11;   // 0x325c0e8 float
    ConCmdRangedVar r12;   // 0x325c1c0 float
    ConCmdRangedVar r13;   // 0x325c298 float
    ConCmdRangedVar r14;   // 0x325c370 float
    ConCmdRangedVar r15;   // 0x325c448 float
    ConCmdRangedVar r16;   // 0x325c520 float
    ConCmdRangedVar r17;   // 0x325c5f8 float
    ConCmdRangedVar r18;   // 0x325c6d0 float
    ConCmdRangedVar r19;   // 0x325c7a8 float
    ConCmdRangedVar r20;   // 0x325c880 float
    ConCmdRangedVar r21;   // 0x325c958 float
    ConCmdRangedVar r22;   // 0x325ca30 float
    ConCmdBoolVar r23;   // 0x325cb08 bool
    ConCmdBoolVar r24;   // 0x325cbd0 bool
    ConCmdBoolVar r25;   // 0x325cc98 bool
    ConCmdRangedVar r26;   // 0x325cd60 float
    ConCmdBoolVar r27;   // 0x325ce38 bool
    ConCmdBoolVar r28;   // 0x325cf00 bool
    ConCmdBoolVar r29;   // 0x325cfc8 bool
    ConCmdBoolVar r30;   // 0x325d090 bool
    ConCmdRangedVar r31;   // 0x325d158 float
    ConCmdRangedVar r32;   // 0x325d230 float
    ConCmdRangedVar r33;   // 0x325d308 float
    ConCmdRangedVar r34;   // 0x325d3e0 float
    ConCmdRangedVar r35;   // 0x325d4b8 float
    ConCmdBoolVar r36;   // 0x325d590 bool
    DebugMenuItem r37;   // 0x325d658 slider
    DebugMenuItem r38;   // 0x325d700 slider
    DebugMenuItem r39;   // 0x325d7a8 slider
    DebugMenuItem r40;   // 0x325d850 tog
    DebugMenuItem r41;   // 0x325d8f8 tog
    std::uint8_t gap42[0x48];
    NameHolder30 r42;   // 0x325d9e8 holder
    NameHolder30 r43;   // 0x325da18 holder
    NameHolder30 r44;   // 0x325da48 holder
    NameHolder30 r45;   // 0x325da78 holder
    NameHolder30 r46;   // 0x325daa8 holder
    NameHolder30 r47;   // 0x325dad8 holder
    NameHolder30 r48;   // 0x325db08 holder
};

struct Slot349Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot349Result init_slot349_7bde50(Slot349Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
