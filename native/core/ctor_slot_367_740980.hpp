// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 367 (RVA 0x740980, 4701 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 367. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_367.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Menu-less debug page of unbounded and bounded float variables and bools (inference from the strings).
// Rows, in constructor order (19 objects at 0x3251120..): 6 bool variables (caller functor tag 0x15cd1c0, dtor 0x20b0), 1 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cd1e0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// No menu rows.
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

inline constexpr std::uint64_t kSlot367Rva = 0x740980;
inline constexpr std::size_t kSlot367Size = 4701;
inline constexpr std::uint64_t kSlot367BoolFnTag = 0x15cd1c0;
inline constexpr std::uint64_t kSlot367FloatFnTag = 0x15cd1e0;
inline constexpr std::uint32_t kSlot367ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot367VarFlags = 1;
inline constexpr std::uint64_t kSlot367UFloatFnTag = 0x15cd1e0;
inline constexpr std::size_t kSlot367UFloatCount = 12;
inline constexpr std::size_t kSlot367BoolCount = 6;
inline constexpr std::size_t kSlot367FloatCount = 1;
inline constexpr std::size_t kSlot367RowCount = 19;
inline constexpr std::size_t kSlot367StringCount = 0;
inline constexpr std::uint64_t kSlot367IntFnTag = 0x0;
inline constexpr std::size_t kSlot367IntCount = 0;
inline constexpr std::size_t kSlot367LocalIntCount = 0;
inline constexpr std::size_t kSlot367SliderCount = 0;
inline constexpr std::uint64_t kSlot367Object0Rva = 0x3251120;
inline constexpr std::size_t kSlot367CellCount = 0;
inline constexpr std::size_t kSlot367HolderCount = 0;
inline constexpr char kSlot367BoolNames[kSlot367BoolCount][51] = {"shadow_ledge_debug", "shadow_ledge_useHiResCollision", "shadow_ledge_chooseClosestGripPoint", "shadow_ledge_thicknessSanityCheck", "shadow_ledge_raytcastScale_scaleBasedOnPlayerSpeed", "shadow_ledge_chargedJump_useRaycastDelay"};
inline constexpr std::size_t kSlot367BoolNameLengths[kSlot367BoolCount] = {0x12, 0x1e, 0x23, 0x21, 0x32, 0x28};
inline constexpr std::uint8_t kSlot367BoolValues[kSlot367BoolCount] = {0, 0, 1, 1, 1, 1};
inline constexpr char kSlot367FloatNames[kSlot367FloatCount][51] = {"shadow_ledge_chargedJump_raycastDelay"};
inline constexpr std::size_t kSlot367FloatNameLengths[kSlot367FloatCount] = {0x25};
inline constexpr std::uint32_t kSlot367FloatLoBits[kSlot367FloatCount] = {0x00000000};
inline constexpr std::uint32_t kSlot367FloatHiBits[kSlot367FloatCount] = {0x7f7fffff};
inline constexpr std::uint32_t kSlot367FloatValueBits[kSlot367FloatCount] = {0x3ecccccd};
inline constexpr char kSlot367UFloatNames[kSlot367UFloatCount][51] = {"shadow_ledge_thicknessThreshold", "shadow_ledge_wallSearchDist", "shadow_ledge_hangCheckDist", "shadow_ledge_hangCheckHeight", "shadow_ledge_raycastScale_srcX", "shadow_ledge_raycastScale_srcY", "shadow_ledge_raycastScale_srcZ", "shadow_ledge_raycastScale_dstX", "shadow_ledge_raycastScale_dstY", "shadow_ledge_raycastScale_dstZ", "shadow_ledge_raytcastScale_playerMaxSpeed", "shadow_ledge_raytcastScale_minScaleForPlayerSpeed"};
inline constexpr std::size_t kSlot367UFloatNameLengths[kSlot367UFloatCount] = {0x1f, 0x1b, 0x1a, 0x1c, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x29, 0x31};
inline constexpr std::uint32_t kSlot367UFloatValueBits[kSlot367UFloatCount] = {0x3f000000, 0x40000000, 0x3fa66666, 0x3f0ccccd, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3fc00000, 0x3f000000};
inline constexpr std::uint8_t kSlot367UFloatEarly[kSlot367UFloatCount] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
inline constexpr std::int32_t kSlot367UFloatDesc[kSlot367UFloatCount] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
inline constexpr char kSlot367MenuStrings[1][1] = {""};   // no strings in this slot
inline constexpr std::size_t kSlot367MenuStringLengths[1] = {0};

struct Slot367Objects {
    ConCmdBoolVar r00;   // 0x3251120 bool
    ConCmdBoolVar r01;   // 0x32511e8 bool
    ConCmdBoolVar r02;   // 0x32512b0 bool
    ConCmdBoolVar r03;   // 0x3251378 bool
    ConCmdRangedVar r04;   // 0x3251440 ufloat
    ConCmdRangedVar r05;   // 0x3251518 ufloat
    ConCmdRangedVar r06;   // 0x32515f0 ufloat
    ConCmdRangedVar r07;   // 0x32516c8 ufloat
    ConCmdRangedVar r08;   // 0x32517a0 ufloat
    ConCmdRangedVar r09;   // 0x3251878 ufloat
    ConCmdRangedVar r10;   // 0x3251950 ufloat
    ConCmdRangedVar r11;   // 0x3251a28 ufloat
    ConCmdRangedVar r12;   // 0x3251b00 ufloat
    ConCmdRangedVar r13;   // 0x3251bd8 ufloat
    ConCmdBoolVar r14;   // 0x3251cb0 bool
    ConCmdRangedVar r15;   // 0x3251d78 ufloat
    ConCmdRangedVar r16;   // 0x3251e50 ufloat
    ConCmdBoolVar r17;   // 0x3251f28 bool
    ConCmdRangedVar r18;   // 0x3251ff0 float
};

struct Slot367Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot367Result init_slot367_740980(Slot367Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
