// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 508 (RVA 0x2241e0, 3252 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 508. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_508.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Physics debug variables (inference from the strings): drawRigidBodies / drawCollision / physics::drawTouch ranged ints, drawCollisionMeshMin/MaxTris unbounded ints, drawModifiers / drawConstraints bools and five physics* unbounded floats whose initial values are RUN-TIME LOADS of globals (four .data globals at 0x1604918..0x1604924 and the .bss global 0x2ee8820): modelled as explicit INPUT fields (Objects::source_bits), default = the pinned ELF initial image.
// Rows, in constructor order (12 objects at 0x2ee88c0..): 2 bool variables (caller functor tag 0x159b488, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// No menu rows (variables only).
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
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

inline constexpr std::uint64_t kSlot508Rva = 0x2241e0;
inline constexpr std::size_t kSlot508Size = 3252;
inline constexpr std::uint64_t kSlot508BoolFnTag = 0x159b488;
inline constexpr std::uint64_t kSlot508FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot508ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot508VarFlags = 1;
inline constexpr std::uint64_t kSlot508UFloatFnTag = 0x159b4a8;
inline constexpr std::size_t kSlot508UFloatCount = 5;
inline constexpr std::size_t kSlot508BoolCount = 2;
inline constexpr std::size_t kSlot508FloatCount = 0;
inline constexpr std::size_t kSlot508RowCount = 12;
inline constexpr std::size_t kSlot508StringCount = 0;
inline constexpr std::uint64_t kSlot508IntFnTag = 0x159b468;
inline constexpr std::size_t kSlot508IntCount = 3;
inline constexpr std::size_t kSlot508LocalIntCount = 0;
inline constexpr std::size_t kSlot508SliderCount = 0;
inline constexpr std::uint64_t kSlot508UIntFnTag = 0x159b468;
inline constexpr std::size_t kSlot508UIntCount = 2;
inline constexpr std::size_t kSlot508CmdCount = 0;
inline constexpr std::size_t kSlot508StrVarCount = 0;
inline constexpr std::uint64_t kSlot508Object0Rva = 0x2ee88c0;
inline constexpr std::size_t kSlot508CellCount = 0;
inline constexpr std::size_t kSlot508HolderCount = 0;
inline constexpr std::size_t kSlot508SentinelCount = 0;
inline constexpr std::size_t kSlot508UFloatSourceCount = 5;
inline constexpr char kSlot508BoolNames[kSlot508BoolCount][26] = {"drawModifiers", "drawConstraints"};
inline constexpr std::size_t kSlot508BoolNameLengths[kSlot508BoolCount] = {0xd, 0xf};
inline constexpr std::uint8_t kSlot508BoolValues[kSlot508BoolCount] = {0, 0};
inline constexpr char kSlot508UFloatNames[kSlot508UFloatCount][26] = {"physicsERP", "physicsCFM", "physicsGravity", "physicsMaxCollisionMargin", "physicsMaxTouchMargin"};
inline constexpr std::size_t kSlot508UFloatNameLengths[kSlot508UFloatCount] = {0xa, 0xa, 0xe, 0x19, 0x15};
inline constexpr std::uint32_t kSlot508UFloatValueBits[kSlot508UFloatCount] = {0x3dcccccd, 0x3727c5ac, 0xc11ccccd, 0x3ca3d70a, 0x00000000};
// Run-time loaded initial values: the original executes `mov eax, [global]` and stores it to the variable (+0xd0 and +0x68). The global is NOT a constant of the body, so the model takes it as an INPUT
// (Objects::source_bits[k]); the default is the pinned ELF initial image of that global (a .bss global has no image: zero). It is the host's job to supply the value in effect when the original constructor runs.
inline constexpr std::int32_t kSlot508UFloatSource[kSlot508UFloatCount] = {0, 1, 2, 3, 4};
inline constexpr std::uint64_t kSlot508SourceAddr[kSlot508UFloatSourceCount] = {0x160491c, 0x1604920, 0x1604924, 0x1604918, 0x2ee8820};
inline constexpr std::uint32_t kSlot508SourceImageBits[kSlot508UFloatSourceCount] = {0x3dcccccd, 0x3727c5ac, 0xc11ccccd, 0x3ca3d70a, 0x00000000};
inline constexpr std::uint8_t kSlot508SourceFileBacked[kSlot508UFloatSourceCount] = {1, 1, 1, 1, 0};
inline constexpr std::uint8_t kSlot508UFloatEarly[kSlot508UFloatCount] = {1, 1, 1, 1, 1};
inline constexpr std::int32_t kSlot508UFloatDesc[kSlot508UFloatCount] = {-1, -1, -1, -1, -1};
inline constexpr char kSlot508IntNames[kSlot508IntCount][26] = {"drawRigidBodies", "drawCollision", "physics::drawTouch"};
inline constexpr std::size_t kSlot508IntNameLengths[kSlot508IntCount] = {0xf, 0xd, 0x12};
inline constexpr std::int32_t kSlot508IntMin[kSlot508IntCount] = {0, 0, 0};
inline constexpr std::int32_t kSlot508IntMax[kSlot508IntCount] = {2, 2, 2};
inline constexpr std::int32_t kSlot508IntValue[kSlot508IntCount] = {0, 0, 0};
inline constexpr char kSlot508UIntNames[kSlot508UIntCount][26] = {"drawCollisionMeshMinTris", "drawCollisionMeshMaxTris"};
inline constexpr std::size_t kSlot508UIntNameLengths[kSlot508UIntCount] = {0x18, 0x18};
inline constexpr std::int32_t kSlot508UIntValue[kSlot508UIntCount] = {0, 10000000};
inline constexpr char kSlot508MenuStrings[1][1] = {""};   // no strings in this slot
inline constexpr std::size_t kSlot508MenuStringLengths[1] = {0};

struct Slot508Objects {
    ConCmdRangedVar r00;   // 0x2ee88c0 int
    ConCmdRangedVar r01;   // 0x2ee8998 int
    ConCmdRangedVar r02;   // 0x2ee8a70 uint
    ConCmdRangedVar r03;   // 0x2ee8b48 uint
    ConCmdRangedVar r04;   // 0x2ee8c20 int
    ConCmdBoolVar r05;   // 0x2ee8cf8 bool
    ConCmdBoolVar r06;   // 0x2ee8dc0 bool
    ConCmdRangedVar r07;   // 0x2ee8e88 ufloat
    ConCmdRangedVar r08;   // 0x2ee8f60 ufloat
    ConCmdRangedVar r09;   // 0x2ee9038 ufloat
    ConCmdRangedVar r10;   // 0x2ee9110 ufloat
    ConCmdRangedVar r11;   // 0x2ee91e8 ufloat
    // INPUT: the run-time value of each global the original loads as an initial value (see kSlot508SourceAddr); the default is the ELF initial image (zero for .bss).
    std::uint32_t source_bits[kSlot508UFloatSourceCount] = {0x3dcccccd, 0x3727c5ac, 0xc11ccccd, 0x3ca3d70a, 0x00000000};
};

struct Slot508Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot508Result init_slot508_2241e0(Slot508Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
