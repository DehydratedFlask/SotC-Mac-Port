// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 501 (RVA 0x24db80, 3244 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 501. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_501.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Animation debug page (inference from the strings): skeleton / bounds / joint-name display variables, a highlighted-joint string variable with an empty default, one group, one label item and eight toggles.
// Rows, in constructor order (18 objects at 0x2eeaed8..): 5 bool variables (caller functor tag 0x159bee0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 20: animation display variables, a string variable with an empty default, a group, a label item and toggles
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

inline constexpr std::uint64_t kSlot501Rva = 0x24db80;
inline constexpr std::size_t kSlot501Size = 3244;
inline constexpr std::uint64_t kSlot501BoolFnTag = 0x159bee0;
inline constexpr std::uint64_t kSlot501FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot501ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot501VarFlags = 1;
inline constexpr std::uint64_t kSlot501UFloatFnTag = 0x159bf00;
inline constexpr std::size_t kSlot501UFloatCount = 1;
inline constexpr std::size_t kSlot501BoolCount = 5;
inline constexpr std::size_t kSlot501FloatCount = 0;
inline constexpr std::size_t kSlot501RowCount = 18;
inline constexpr std::size_t kSlot501StringCount = 10;
inline constexpr std::uint64_t kSlot501IntFnTag = 0x0;
inline constexpr std::size_t kSlot501IntCount = 0;
inline constexpr std::size_t kSlot501LocalIntCount = 0;
inline constexpr std::size_t kSlot501SliderCount = 0;
inline constexpr std::uint64_t kSlot501UIntFnTag = 0x159bf40;
inline constexpr std::size_t kSlot501UIntCount = 1;
inline constexpr std::size_t kSlot501CmdCount = 0;
inline constexpr std::size_t kSlot501StrVarCount = 1;
inline constexpr std::uint64_t kSlot501Object0Rva = 0x2eeaed8;
inline constexpr std::size_t kSlot501CellCount = 0;
inline constexpr std::size_t kSlot501HolderCount = 0;
inline constexpr std::size_t kSlot501SentinelCount = 0;
inline constexpr char kSlot501BoolNames[kSlot501BoolCount][30] = {"r_showBounds", "r_showSkeleton", "r_showJointNames", "r_showSkeletonWorldSpace", "r_enableAnimations"};
inline constexpr std::size_t kSlot501BoolNameLengths[kSlot501BoolCount] = {0xc, 0xe, 0x10, 0x18, 0x12};
inline constexpr std::uint8_t kSlot501BoolValues[kSlot501BoolCount] = {0, 0, 0, 1, 1};
inline constexpr std::int32_t kSlot501BoolDesc[kSlot501BoolCount] = {-1, -1, -1, -1, -1};
inline constexpr std::uint8_t kSlot501BoolEarly[kSlot501BoolCount] = {1, 1, 1, 1, 1};
inline constexpr char kSlot501UFloatNames[kSlot501UFloatCount][30] = {"r_showSkeletonScale"};
inline constexpr std::size_t kSlot501UFloatNameLengths[kSlot501UFloatCount] = {0x13};
inline constexpr std::uint32_t kSlot501UFloatValueBits[kSlot501UFloatCount] = {0x00000000};
inline constexpr std::uint8_t kSlot501UFloatEarly[kSlot501UFloatCount] = {1};
inline constexpr std::int32_t kSlot501UFloatDesc[kSlot501UFloatCount] = {-1};
inline constexpr char kSlot501UIntNames[kSlot501UIntCount][30] = {"r_highlightSkeletonJointIndex"};
inline constexpr std::size_t kSlot501UIntNameLengths[kSlot501UIntCount] = {0x1d};
inline constexpr std::int32_t kSlot501UIntValue[kSlot501UIntCount] = {-1};
inline constexpr std::uint8_t kSlot501UIntEarly[kSlot501UIntCount] = {1};
inline constexpr char kSlot501StrNames[kSlot501StrVarCount][30] = {"r_highlightSkeletonJoint"};
inline constexpr std::size_t kSlot501StrNameLengths[kSlot501StrVarCount] = {0x18};
inline constexpr std::int32_t kSlot501StrInit[kSlot501StrVarCount] = {-1};
inline constexpr char kSlot501MenuStrings[kSlot501StringCount][28] = {"", "Animation", "Enable Animations", "Show Skeletons", "Skeleton Scale", "Show Skeleton in WorldSpace", "Show Joint Names", "Show Bounds", "HighLight Joint Name", "HighLight Joint Index"};
inline constexpr std::size_t kSlot501MenuStringLengths[kSlot501StringCount] = {0x0, 0x9, 0x11, 0xe, 0xe, 0x1b, 0x10, 0xb, 0x14, 0x15};

struct Slot501Objects {
    ConCmdBoolVar r00;   // 0x2eeaed8 bool
    ConCmdBoolVar r01;   // 0x2eeafa0 bool
    ConCmdBoolVar r02;   // 0x2eeb068 bool
    ConCmdRangedVar r03;   // 0x2eeb130 ufloat
    ConCmdBoolVar r04;   // 0x2eeb208 bool
    ConCmdStringVar r05;   // 0x2eeb2d0 str
    ConCmdRangedVar r06;   // 0x2eeb418 uint
    ConCmdBoolVar r07;   // 0x2eeb4f0 bool
    DebugMenuItem r08;   // 0x2eeb5b8 group
    DebugMenuItem r09;   // 0x2eeb660 sep
    DebugMenuItem r10;   // 0x2eeb708 toge
    DebugMenuItem r11;   // 0x2eeb7b0 toge
    DebugMenuItem r12;   // 0x2eeb858 toge
    DebugMenuItem r13;   // 0x2eeb900 toge
    DebugMenuItem r14;   // 0x2eeb9a8 toge
    DebugMenuItem r15;   // 0x2eeba50 toge
    DebugMenuItem r16;   // 0x2eebaf8 toge
    DebugMenuItem r17;   // 0x2eebba0 toge
    SharedStrCtorHooks separator_alloc{};   // HOST INPUT: the allocator behind 0xf1a0 ("MenuSeparator"; 0x12820 / 0x12af0 are not lifted); empty refuses the separator rows. See default_family_separator_alloc().
};

struct Slot501Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot501Result init_slot501_24db80(Slot501Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
