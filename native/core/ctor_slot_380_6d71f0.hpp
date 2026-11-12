// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 380 (RVA 0x6d71f0, 4668 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 380. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_380.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Foot-partner IK / hi-res IK debug page (inference from the strings).
// Rows, in constructor order (27 objects at 0x323aa28..): 11 bool variables (caller functor tag 0x15cc858, dtor 0x20b0), 3 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cc878, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
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

inline constexpr std::uint64_t kSlot380Rva = 0x6d71f0;
inline constexpr std::size_t kSlot380Size = 4668;
inline constexpr std::uint64_t kSlot380BoolFnTag = 0x15cc858;
inline constexpr std::uint64_t kSlot380FloatFnTag = 0x15cc878;
inline constexpr std::uint32_t kSlot380ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot380VarFlags = 1;
inline constexpr std::uint64_t kSlot380UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot380UFloatCount = 0;
inline constexpr std::size_t kSlot380BoolCount = 11;
inline constexpr std::size_t kSlot380FloatCount = 3;
inline constexpr std::size_t kSlot380RowCount = 27;
inline constexpr std::size_t kSlot380StringCount = 2;
inline constexpr std::size_t kSlot380SliderCount = 1;
inline constexpr std::uint64_t kSlot380Object0Rva = 0x323aa28;
inline constexpr std::size_t kSlot380CellCount = 0;
inline constexpr std::size_t kSlot380HolderCount = 0;
inline constexpr char kSlot380BoolNames[kSlot380BoolCount][40] = {"shadowEnableFootPartnerIk", "cp10_hiResIKEnable", "cp10_hiResIKDeferred", "cp10_hiResIKLower", "cp10_drawIKRayTest", "cp10_enableScaleRootHeightAdjustDiff", "shadowEnableFootIkStairsInterpolate", "shadow_ikEnableLimitFootOrient", "shadow_ikDisableLegSetLowerFit", "shadow_ikDisableLegVibration", "shadow_ikEnableSelectiveRecomputeGlobal"};
inline constexpr std::size_t kSlot380BoolNameLengths[kSlot380BoolCount] = {0x19, 0x12, 0x14, 0x11, 0x12, 0x24, 0x23, 0x1e, 0x1e, 0x1c, 0x27};
inline constexpr std::uint8_t kSlot380BoolValues[kSlot380BoolCount] = {1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1};
inline constexpr char kSlot380FloatNames[kSlot380FloatCount][40] = {"shadow_footPartnerIkMinHeightDiff", "shadow_ikLimitFootOrientAngle", "shadow_ik3BlendInFrames"};
inline constexpr std::size_t kSlot380FloatNameLengths[kSlot380FloatCount] = {0x21, 0x1d, 0x17};
inline constexpr std::uint32_t kSlot380FloatLoBits[kSlot380FloatCount] = {0x3c23d70a, 0x00000000, 0x00000000};
inline constexpr std::uint32_t kSlot380FloatHiBits[kSlot380FloatCount] = {0x3f000000, 0x42b40000, 0x41f00000};
inline constexpr std::uint32_t kSlot380FloatValueBits[kSlot380FloatCount] = {0x3df5c28f, 0x42340000, 0x40a00000};
inline constexpr char kSlot380MenuStrings[kSlot380StringCount][10] = {"CP10-Anim", "Shadow IK"};
inline constexpr std::size_t kSlot380MenuStringLengths[kSlot380StringCount] = {0x9, 0x9};
inline constexpr std::uint32_t kSlot380SliderLoBits[kSlot380SliderCount] = {0x3c23d70a};
inline constexpr std::uint32_t kSlot380SliderHiBits[kSlot380SliderCount] = {0x3dcccccd};

struct Slot380Objects {
    ConCmdBoolVar r00;   // 0x323aa28 bool
    ConCmdBoolVar r01;   // 0x323aaf0 bool
    DebugMenuItem r02;   // 0x323abb8 tog
    ConCmdBoolVar r03;   // 0x323ac60 bool
    DebugMenuItem r04;   // 0x323ad28 tog
    ConCmdBoolVar r05;   // 0x323add0 bool
    DebugMenuItem r06;   // 0x323ae98 tog
    ConCmdBoolVar r07;   // 0x323af40 bool
    DebugMenuItem r08;   // 0x323b008 tog
    ConCmdBoolVar r09;   // 0x323b0b0 bool
    DebugMenuItem r10;   // 0x323b178 tog
    DebugMenuItem r11;   // 0x323b220 tog
    ConCmdRangedVar r12;   // 0x323b2c8 float
    DebugMenuItem r13;   // 0x323b3a0 slider
    ConCmdBoolVar r14;   // 0x323b448 bool
    DebugMenuItem r15;   // 0x323b510 tog
    ConCmdRangedVar r16;   // 0x323b5b8 float
    DebugMenuItem r17;   // 0x323b690 tog
    ConCmdBoolVar r18;   // 0x323b738 bool
    DebugMenuItem r19;   // 0x323b800 tog
    ConCmdRangedVar r20;   // 0x323b8a8 float
    ConCmdBoolVar r21;   // 0x323b980 bool
    DebugMenuItem r22;   // 0x323ba48 tog
    ConCmdBoolVar r23;   // 0x323baf0 bool
    DebugMenuItem r24;   // 0x323bbb8 tog
    ConCmdBoolVar r25;   // 0x323bc60 bool
    DebugMenuItem r26;   // 0x323bd28 tog
};

struct Slot380Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot380Result init_slot380_6d71f0(Slot380Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
