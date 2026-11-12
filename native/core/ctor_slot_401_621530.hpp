// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 401 (RVA 0x621530, 3799 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 401. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_401.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// CP10 character animation debug page: bool toggles and two local ranged-int variables (phase blend), plus the Shadow IK toggle (inference from the strings).
// Rows, in constructor order (23 objects at 0x300ce68..): 10 bool variables (caller functor tag 0x15cc2f8, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: CP10-Anim and Shadow IK toggles.
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

inline constexpr std::uint64_t kSlot401Rva = 0x621530;
inline constexpr std::size_t kSlot401Size = 3799;
inline constexpr std::uint64_t kSlot401BoolFnTag = 0x15cc2f8;
inline constexpr std::uint64_t kSlot401FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot401ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot401VarFlags = 1;
inline constexpr std::uint64_t kSlot401UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot401UFloatCount = 0;
inline constexpr std::size_t kSlot401BoolCount = 10;
inline constexpr std::size_t kSlot401FloatCount = 0;
inline constexpr std::size_t kSlot401RowCount = 23;
inline constexpr std::size_t kSlot401StringCount = 2;
inline constexpr std::uint64_t kSlot401IntFnTag = 0x0;
inline constexpr std::size_t kSlot401IntCount = 0;
inline constexpr std::size_t kSlot401LocalIntCount = 2;
inline constexpr std::size_t kSlot401SliderCount = 0;
inline constexpr std::uint64_t kSlot401Object0Rva = 0x300ce68;
inline constexpr std::size_t kSlot401CellCount = 0;
inline constexpr std::size_t kSlot401HolderCount = 0;
inline constexpr char kSlot401BoolNames[kSlot401BoolCount][42] = {"cp10_enableInterp", "cp10_allowNeuterCharacterGeomety", "cp10_enablePhaseBlend", "cp10_demoMotionRootFix", "shadow_ikEnableSkipDefaultRecomputeGlobal", "cp10_enableAddSyncAnims", "cp10_enableAddSubAnims", "cp10_enableAddCustomAnims", "cp10_enableIK", "cp10_enableExpression"};
inline constexpr std::size_t kSlot401BoolNameLengths[kSlot401BoolCount] = {0x11, 0x20, 0x15, 0x16, 0x29, 0x17, 0x16, 0x19, 0xd, 0x15};
inline constexpr std::uint8_t kSlot401BoolValues[kSlot401BoolCount] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
inline constexpr LocalIntSiteSpec kSlot401LocalIntSites[kSlot401LocalIntCount] = {{0x620e10, "cp10_phaseBlendUse", 18, 0, 0, 2, 1, 0x15cc318, false}, {0x6211d0, "cp10_phaseBlendTypeOverride", 27, 0, 1, 4, 1, 0x15cc3b0, false}};
inline constexpr char kSlot401MenuStrings[kSlot401StringCount][10] = {"CP10-Anim", "Shadow IK"};
inline constexpr std::size_t kSlot401MenuStringLengths[kSlot401StringCount] = {0x9, 0x9};

struct Slot401Objects {
    ConCmdBoolVar r00;   // 0x300ce68 bool
    ConCmdBoolVar r01;   // 0x300cf30 bool
    ConCmdBoolVar r02;   // 0x300cff8 bool
    ConCmdRangedVar r03;   // 0x300d0c0 lint
    ConCmdRangedVar r04;   // 0x300d198 lint
    ConCmdBoolVar r05;   // 0x300d270 bool
    ConCmdBoolVar r06;   // 0x300d338 bool
    DebugMenuItem r07;   // 0x300d400 tog
    DebugMenuItem r08;   // 0x300d4a8 tog
    DebugMenuItem r09;   // 0x300d550 tog
    DebugMenuItem r10;   // 0x300d5f8 tog
    DebugMenuItem r11;   // 0x300d6a0 tog
    DebugMenuItem r12;   // 0x300d748 tog
    std::uint8_t gap13[0x18];
    ConCmdBoolVar r13;   // 0x300d808 bool
    DebugMenuItem r14;   // 0x300d8d0 tog
    ConCmdBoolVar r15;   // 0x300d978 bool
    DebugMenuItem r16;   // 0x300da40 tog
    ConCmdBoolVar r17;   // 0x300dae8 bool
    DebugMenuItem r18;   // 0x300dbb0 tog
    ConCmdBoolVar r19;   // 0x300dc58 bool
    DebugMenuItem r20;   // 0x300dd20 tog
    ConCmdBoolVar r21;   // 0x300ddc8 bool
    DebugMenuItem r22;   // 0x300de90 tog
};

struct Slot401Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot401Result init_slot401_621530(Slot401Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
