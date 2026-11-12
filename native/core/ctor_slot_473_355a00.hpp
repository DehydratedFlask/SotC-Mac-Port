// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 473 (RVA 0x355a00, 6487 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 473. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_473.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Debug page (inference from the strings): bool / float / string variables, toggles, a group, separators, one console command and an inline-storage ring header.
// Rows, in constructor order (42 objects at 0x2ef1010..): 5 bool variables (caller functor tag 0x15aa860, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 16: holder, variables, toggles, group, five separators, command and a ring header
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
#include "helper_274730_bool_var.hpp"
#include "helper_1f220_ring_header.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot473Rva = 0x355a00;
inline constexpr std::size_t kSlot473Size = 6487;
inline constexpr std::uint64_t kSlot473BoolFnTag = 0x15aa860;
inline constexpr std::uint64_t kSlot473FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot473ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot473VarFlags = 1;
inline constexpr std::uint64_t kSlot473UFloatFnTag = 0x15aa880;
inline constexpr std::size_t kSlot473UFloatCount = 2;
inline constexpr std::size_t kSlot473BoolCount = 5;
inline constexpr std::size_t kSlot473FloatCount = 0;
inline constexpr std::size_t kSlot473RowCount = 42;
inline constexpr std::size_t kSlot473StringCount = 21;
inline constexpr std::uint64_t kSlot473IntFnTag = 0x0;
inline constexpr std::size_t kSlot473IntCount = 0;
inline constexpr std::size_t kSlot473LocalIntCount = 0;
inline constexpr std::size_t kSlot473SliderCount = 0;
inline constexpr std::uint64_t kSlot473UIntFnTag = 0x0;
inline constexpr std::size_t kSlot473UIntCount = 0;
inline constexpr std::size_t kSlot473CmdCount = 1;
inline constexpr std::size_t kSlot473StrVarCount = 1;
inline constexpr std::uint64_t kSlot473Object0Rva = 0x2ef1010;
inline constexpr std::size_t kSlot473BoolXCount = 9;
inline constexpr std::uint64_t kSlot473HolderVptr = 0x15aa858;
inline constexpr std::uint64_t kSlot473HolderDtorRva = 0x34e820;
inline constexpr std::size_t kSlot473CellCount = 0;
inline constexpr std::size_t kSlot473HolderCount = 1;
inline constexpr std::size_t kSlot473SentinelCount = 0;
inline constexpr std::size_t kSlot473RingCount = 1;
inline constexpr std::uint64_t kSlot473RingDtorRva = 0x354470;
inline constexpr std::uint64_t kSlot473RingHeaderRva = 0x1f220;
inline constexpr char kSlot473BoolNames[kSlot473BoolCount][46] = {"arealoader_displayHeaderZoneInfo", "arealoader_displayZoneInfo", "arealoader_enableMapLoading", "arealoader_enableZoneLoading", "arealoader_hideIdleComponentStates"};
inline constexpr std::size_t kSlot473BoolNameLengths[kSlot473BoolCount] = {0x20, 0x1a, 0x1b, 0x1c, 0x22};
inline constexpr std::uint8_t kSlot473BoolValues[kSlot473BoolCount] = {0, 0, 1, 1, 1};
inline constexpr std::int32_t kSlot473BoolDesc[kSlot473BoolCount] = {-1, -1, -1, -1, -1};
inline constexpr std::uint8_t kSlot473BoolEarly[kSlot473BoolCount] = {1, 1, 1, 1, 1};
// Out-of-line bool variables (0x274730): the call site's name, initial byte, REAL argument-functor class tag (a bool-family table) and argument block {self, q1, q2, q3, payload, aux};
// q[0] is the block's own address (rewritten into the object by the self-reference rule), q[1] is a .text RVA read from a relocated global, q[3] a callback RVA. All identity-only.
inline constexpr BoolVar274730SiteSpec kSlot473BoolXSites[kSlot473BoolXCount] = {{"arealoader_areaStateTint", 0x18, 0, 0x15aa860, {0, 0x274710, 0x0, 0x34e840}, 0x0, 0x0}, {"arealoader_areaMapTint", 0x16, 0, 0x15aa860, {0, 0x274710, 0x0, 0x34e840}, 0x0, 0x0}, {"arealoader_enableZoneRendering", 0x1e, 1, 0x15aa860, {0, 0x274710, 0x0, 0x34ebb0}, 0x0, 0x0}, {"arealoader_createdStatePreservation", 0x23, 1, 0x15aa860, {0, 0x274710, 0x0, 0x34ebb0}, 0x0, 0x0}, {"arealoader_nonlocalarea_force_off", 0x21, 0, 0x15aa860, {0, 0x274710, 0x0, 0x34ebb0}, 0x0, 0x0}, {"arealoader_nonlocalarea_force_background", 0x28, 0, 0x15aa860, {0, 0x274710, 0x0, 0x34ebb0}, 0x0, 0x0}, {"arealoader_localareas_force_high", 0x20, 0, 0x15aa860, {0, 0x274710, 0x0, 0x34ebb0}, 0x0, 0x0}, {"arealoader_allareas_force_high", 0x1e, 0, 0x15aa860, {0, 0x274710, 0x0, 0x34ebb0}, 0x0, 0x0}, {"arealoader_allareas_lowest_available", 0x24, 0, 0x15aa860, {0, 0x274710, 0x0, 0x34ebb0}, 0x0, 0x0}};
inline constexpr char kSlot473UFloatNames[kSlot473UFloatCount][46] = {"areaLoader_FadeTime", "areaLoader_HoldInLoadStateBeforeUnloadingTime"};
inline constexpr std::size_t kSlot473UFloatNameLengths[kSlot473UFloatCount] = {0x13, 0x2d};
inline constexpr std::uint32_t kSlot473UFloatValueBits[kSlot473UFloatCount] = {0x3f800000, 0x00000000};
inline constexpr std::uint8_t kSlot473UFloatEarly[kSlot473UFloatCount] = {1, 1};
inline constexpr std::int32_t kSlot473UFloatDesc[kSlot473UFloatCount] = {-1, -1};
inline constexpr char kSlot473CmdNames[kSlot473CmdCount][46] = {"poiScript"};
inline constexpr std::size_t kSlot473CmdNameLengths[kSlot473CmdCount] = {0x9};
inline constexpr std::int32_t kSlot473CmdDesc[kSlot473CmdCount] = {20};
inline constexpr std::uint64_t kSlot473CmdHandler[kSlot473CmdCount] = {0x3544f0};
inline constexpr char kSlot473StrNames[kSlot473StrVarCount][46] = {"poiPlayerStartingPositions"};
inline constexpr std::size_t kSlot473StrNameLengths[kSlot473StrVarCount] = {0x1a};
inline constexpr std::int32_t kSlot473StrInit[kSlot473StrVarCount] = {19};
inline constexpr char kSlot473MenuStrings[kSlot473StringCount][117] = {"ShaderDebugMode", "", "Area Loader", "Display Zone Info", "Display Extra Zone Info", "Hide Idle Area Loader States", "Enable Location Based Streaming", "Enable Zone Based Streaming", "Enable Static Model Rendering", "Enable Created State Preservation", "Non Local Areas - Force Off", "Non Local Areas - Force Background", "All Areas - Force High", "Local Areas - Force High", "All Areas - Lowest Available", "Show Area State Tint", "Show Area With Map Tint", "Load State Fade Time", "Delay Before Unloading Resources", "$/misc/playerstartpositions.txt", "Add a script point of interest the debug camera can use to jump to. +poi <group> <name> <sgprFile> <opt:description>"};
inline constexpr std::size_t kSlot473MenuStringLengths[kSlot473StringCount] = {0xf, 0x0, 0xb, 0x11, 0x17, 0x1c, 0x1f, 0x1b, 0x1d, 0x21, 0x1b, 0x22, 0x16, 0x18, 0x1c, 0x14, 0x17, 0x14, 0x20, 0x1f, 0x74};

// Ring / vector header object (0x354470 is its atexit destructor, an IDENTITY TAG; helper_1f220_ring_header writes exactly four qwords: [+0x00] = [+0x08] = [+0x20] = this + 0x28 and [+0x10] = this + 0x2628).
// The 0x2600 B inline storage area, +0x18 and everything else are never written (the element type and the ring semantics are INFERENCE).
struct Slot473Ring {
    std::uintptr_t q00;
    std::uintptr_t q08;
    std::uintptr_t q10;
    std::uintptr_t q18;   // never written
    std::uintptr_t q20;
    std::uint8_t area[0x2600];   // never written
};
static_assert(sizeof(Slot473Ring) == 0x2628);

struct Slot473Objects {
    NameHolder30 r00;   // 0x2ef1010 holder
    ConCmdBoolVar r01;   // 0x2ef1040 bool
    ConCmdBoolVar r02;   // 0x2ef1108 bool
    ConCmdBoolVar r03;   // 0x2ef11d0 boolx
    ConCmdBoolVar r04;   // 0x2ef1298 boolx
    ConCmdBoolVar r05;   // 0x2ef1360 bool
    ConCmdBoolVar r06;   // 0x2ef1428 bool
    ConCmdBoolVar r07;   // 0x2ef14f0 boolx
    ConCmdBoolVar r08;   // 0x2ef15b8 boolx
    ConCmdBoolVar r09;   // 0x2ef1680 boolx
    ConCmdBoolVar r10;   // 0x2ef1748 boolx
    ConCmdBoolVar r11;   // 0x2ef1810 boolx
    ConCmdBoolVar r12;   // 0x2ef18d8 boolx
    ConCmdBoolVar r13;   // 0x2ef19a0 boolx
    ConCmdBoolVar r14;   // 0x2ef1a68 bool
    ConCmdRangedVar r15;   // 0x2ef1b30 ufloat
    ConCmdRangedVar r16;   // 0x2ef1c08 ufloat
    DebugMenuItem r17;   // 0x2ef1ce0 group
    DebugMenuItem r18;   // 0x2ef1d88 toge
    DebugMenuItem r19;   // 0x2ef1e30 toge
    DebugMenuItem r20;   // 0x2ef1ed8 toge
    DebugMenuItem r21;   // 0x2ef1f80 sep
    DebugMenuItem r22;   // 0x2ef2028 toge
    DebugMenuItem r23;   // 0x2ef20d0 toge
    DebugMenuItem r24;   // 0x2ef2178 toge
    DebugMenuItem r25;   // 0x2ef2220 toge
    DebugMenuItem r26;   // 0x2ef22c8 sep
    DebugMenuItem r27;   // 0x2ef2370 toge
    DebugMenuItem r28;   // 0x2ef2418 toge
    DebugMenuItem r29;   // 0x2ef24c0 toge
    DebugMenuItem r30;   // 0x2ef2568 toge
    DebugMenuItem r31;   // 0x2ef2610 toge
    DebugMenuItem r32;   // 0x2ef26b8 sep
    DebugMenuItem r33;   // 0x2ef2760 toge
    DebugMenuItem r34;   // 0x2ef2808 toge
    DebugMenuItem r35;   // 0x2ef28b0 sep
    DebugMenuItem r36;   // 0x2ef2958 toge
    DebugMenuItem r37;   // 0x2ef2a00 toge
    DebugMenuItem r38;   // 0x2ef2aa8 sep
    std::uint8_t gap39[0x58];
    Slot473Ring r39;   // 0x2ef2ba8 ringhdr
    ConCmdStringVar r40;   // 0x2ef51d0 str
    ConCmdCommand r41;   // 0x2ef5318 cmd
    SharedStrCtorHooks separator_alloc{};   // HOST INPUT: the allocator behind 0xf1a0 ("MenuSeparator"; 0x12820 / 0x12af0 are not lifted); empty refuses the separator rows. See default_family_separator_alloc().
};

struct Slot473Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot473Result init_slot473_355a00(Slot473Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
