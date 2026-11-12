// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 402 (RVA 0x610430, 2663 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 402. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_402.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// CP10 animation debug page (inference from the strings): two shadow_gridBlendDefaultBoundary floats, the CP10_AnimMode ranged enum (range 0..2, inline-built), CP10_Anim* bools and floats, the CP10_NewAnimationFilter string variable, toggles, a slider and the r_enableAnimations name holder
// Rows, in constructor order (14 objects at 0x300c388..): 3 bool variables (caller functor tag 0x15cc1c8, dtor 0x20b0), 3 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cc110, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 14: inline-built ranged enum variable (identity class pair) + floats, bools, a string variable, toggles and a slider
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

inline constexpr std::uint64_t kSlot402Rva = 0x610430;
inline constexpr std::size_t kSlot402Size = 2663;
inline constexpr std::uint64_t kSlot402BoolFnTag = 0x15cc1c8;
inline constexpr std::uint64_t kSlot402FloatFnTag = 0x15cc110;
inline constexpr std::uint32_t kSlot402ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot402VarFlags = 1;
inline constexpr std::uint64_t kSlot402UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot402UFloatCount = 0;
inline constexpr std::size_t kSlot402BoolCount = 3;
inline constexpr std::size_t kSlot402FloatCount = 3;
inline constexpr std::size_t kSlot402RowCount = 14;
inline constexpr std::size_t kSlot402StringCount = 3;
inline constexpr std::uint64_t kSlot402IntFnTag = 0x0;
inline constexpr std::size_t kSlot402IntCount = 0;
inline constexpr std::size_t kSlot402LocalIntCount = 0;
inline constexpr std::size_t kSlot402SliderCount = 1;
inline constexpr std::uint64_t kSlot402UIntFnTag = 0x0;
inline constexpr std::size_t kSlot402UIntCount = 0;
inline constexpr std::size_t kSlot402CmdCount = 0;
inline constexpr std::size_t kSlot402StrVarCount = 1;
inline constexpr std::uint64_t kSlot402Object0Rva = 0x300c388;
inline constexpr std::uint64_t kSlot402HolderVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot402HolderDtorRva = 0x8ef00;
inline constexpr std::size_t kSlot402CellCount = 0;
inline constexpr std::size_t kSlot402HolderCount = 1;
inline constexpr std::size_t kSlot402SentinelCount = 0;
inline constexpr std::size_t kSlot402EnumCount = 1;
inline constexpr std::uint64_t kSlot402EnumMidVptr = 0x15cc0a8;
inline constexpr std::uint64_t kSlot402EnumFinalVptr = 0x15cc140;
inline constexpr std::uint64_t kSlot402EnumFnTag = 0x15cc1a8;
inline constexpr std::uint64_t kSlot402EnumDtorRva = 0x60d5d0;
inline constexpr std::uint32_t kSlot402EnumSlot3Result = 1;
inline constexpr std::uint32_t kSlot402EnumSlot6Result = 1;
inline constexpr char kSlot402BoolNames[kSlot402BoolCount][33] = {"CP10_AnimEnableTranslation", "CP10_AnimUseOrigFrameCount", "CP10_AnimEnablePartialBodyBlend"};
inline constexpr std::size_t kSlot402BoolNameLengths[kSlot402BoolCount] = {0x1a, 0x1a, 0x1f};
inline constexpr std::uint8_t kSlot402BoolValues[kSlot402BoolCount] = {1, 0, 1};
inline constexpr std::int32_t kSlot402BoolDesc[kSlot402BoolCount] = {-1, -1, -1};
inline constexpr char kSlot402FloatNames[kSlot402FloatCount][33] = {"shadow_gridBlendDefaultBoundary", "shadow_gridBlendDefaultBoundary2", "CP10_AnimPlaybackSpeed"};
inline constexpr std::size_t kSlot402FloatNameLengths[kSlot402FloatCount] = {0x1f, 0x20, 0x16};
inline constexpr std::uint32_t kSlot402FloatLoBits[kSlot402FloatCount] = {0x00000000, 0x00000000, 0x00000000};
inline constexpr std::uint32_t kSlot402FloatHiBits[kSlot402FloatCount] = {0x3f800000, 0x40000000, 0x40a00000};
inline constexpr std::uint32_t kSlot402FloatValueBits[kSlot402FloatCount] = {0x3f800000, 0x40000000, 0x3f800000};
inline constexpr char kSlot402EnumNames[kSlot402EnumCount][33] = {"CP10_AnimMode"};
inline constexpr std::size_t kSlot402EnumNameLengths[kSlot402EnumCount] = {0xd};
inline constexpr std::int32_t kSlot402EnumMin[kSlot402EnumCount] = {0};
inline constexpr std::int32_t kSlot402EnumMax[kSlot402EnumCount] = {2};
inline constexpr std::int32_t kSlot402EnumValue[kSlot402EnumCount] = {2};
inline constexpr char kSlot402StrNames[kSlot402StrVarCount][33] = {"CP10_NewAnimationFilter"};
inline constexpr std::size_t kSlot402StrNameLengths[kSlot402StrVarCount] = {0x17};
inline constexpr std::int32_t kSlot402StrInit[kSlot402StrVarCount] = {0};
inline constexpr char kSlot402MenuStrings[kSlot402StringCount][19] = {"", "CP10-Anim", "r_enableAnimations"};
inline constexpr std::size_t kSlot402MenuStringLengths[kSlot402StringCount] = {0x0, 0x9, 0x12};
inline constexpr std::uint32_t kSlot402SliderLoBits[kSlot402SliderCount] = {0x3dcccccd};
inline constexpr std::uint32_t kSlot402SliderHiBits[kSlot402SliderCount] = {0x3f000000};

struct Slot402Objects {
    ConCmdRangedVar r00;   // 0x300c388 float
    ConCmdRangedVar r01;   // 0x300c460 float
    ConCmdRangedVar r02;   // 0x300c538 ienum
    ConCmdBoolVar r03;   // 0x300c610 bool
    ConCmdBoolVar r04;   // 0x300c6d8 bool
    ConCmdStringVar r05;   // 0x300c7a0 str
    ConCmdRangedVar r06;   // 0x300c8e8 float
    ConCmdBoolVar r07;   // 0x300c9c0 bool
    DebugMenuItem r08;   // 0x300ca88 tog
    DebugMenuItem r09;   // 0x300cb30 tog
    DebugMenuItem r10;   // 0x300cbd8 slider
    DebugMenuItem r11;   // 0x300cc80 tog
    DebugMenuItem r12;   // 0x300cd28 tog
    NameHolder30 r13;   // 0x300cdd0 holder
};

struct Slot402Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot402Result init_slot402_610430(Slot402Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
