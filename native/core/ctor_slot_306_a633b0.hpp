// CPU-01-STARTUP-SLOT-306-ASSEMBLE-01: native recovery of original constructor-table index 306 /
// RVA 0xa633b0 (body [0xa633b0, 0xa654bd), 0x210d = 8461 B), ELF sha256 d0723afb...5861d3: the
// `sndx_` sound-engine debug console variables, console commands and the "Sound" debug menu.
//
// This module only ASSEMBLES existing, byte-pinned native pieces in the original order; it adds no new
// reading of the shipped code beyond the 46-object table below (checker: tools/lift/check_ctor_slot_306.py,
// semantics: docs/checkpoints/2026-10-25-slot306-semantics.md). NOTHING here executes, loads or calls
// an original RVA. Vtable pointers, functor tables, the destructor RVAs recorded for `__cxa_atexit` and
// the three command handlers (0xa5bf20 / 0xa5bf50 / 0xa5c020) are IDENTITY TAGS only: the handlers stay
// tagged and are never called, the destructors are recorded and never run.
//
// Construction order is the table order #0..#45 and each object's side effects are contiguous:
//   #0        root pointer cell 0x32f8ff0: qword 0, __cxa_atexit(0xa5be70)
//   #1 #2 #14 ranged int variables (core/intvar_a624a0: wrappers 0xa624a0 / 0xa62860, register themselves)
//   #3 #11 #17 #18 #19  ranged float variables (0xc300; caller functor tag 0x15d2040 -> 0x1594318)
//   #4..#10 #12 #13 #15 #16  bool variables (inline shape, functor tag 0x15d2060, value 0 / default 0)
//   #20 #21 #22  console commands (core/fconcmd_object_slot306: vtable 0x1594dc0, flags 2, dtor 0x4b0)
//   #23..#45  the 23 debug-menu items, flags 0x30, all with a null parent, so they are appended to the
//             root list 0x2dfd4c0 in table order (the root guard and its 0x27280 atexit were already done
//             by slot 299 on the production path; a fresh DebugMenuRegistry records it on the first item)
// Every item is: ctor (0x26c10 / 0x26b60 / 0x26ea0 / 0x27050 / 0x26f90, which tail-calls 0x268b0),
// then `0xf590` on the caption (not for separators; the command item also releases its command name
// first), `0xf590` on the parent path, `__cxa_atexit(0x272d0, item)`.
// Toggle targets #35, #41 and #42 are the int variables; their virtual-slot results come from
// int_var_menu_target (core/intvar_a624a0), the bool ones from debug_menu_target_for.
//
// Registration side effects, in order: the 22 console objects #1..#22 queue into the pending list
// (the manager cell is null on the whole startup path); 46 atexit records are appended (one per
// object); the 23 menu items are appended to the root list.
//
// Hooks / contracts (explicit, fail closed):
//   * `Slot306Hooks::separator_alloc` - the four `MenuSeparator` items allocate "MenuSeparator" through
//     the native 0xf1a0; the allocator behind it (0x12820) is not lifted. With no allocator the fragment
//     reports ok == false at the first separator (#26) with a named blocked site and builds nothing after
//     it. `default_slot306_hooks()` supplies a malloc/free host allocator (documented stand-in, NOT the
//     engine allocator); a host that wants the original allocator replaces it.
//   * `Slot306Hooks::menu_assert` - the engine-state-gated log of 0x268b0; unreachable with flags 0x30.
//   * a non-null ConCmd manager (`ConCmdRegistry::manager`) and a non-null debug-menu parent
//     (`DebugMenuRegistry::parent`) need their insert contracts; without them registration is a named
//     failure.
//
// Known gaps: kSlot306Gaps is empty: every one of the 46 objects is built. What remains outside this
// fragment is listed in kSlot306NotModelled and is not a construction gap.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "intvar_a624a0.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot306Rva = 0xa633b0;
inline constexpr std::uint64_t kSlot306EndRva = 0xa654bd;
inline constexpr std::size_t kSlot306BodySize = 0x210d;
inline constexpr std::size_t kSlot306ObjectCount = 46;
inline constexpr std::size_t kSlot306FirstMenuIndex = 23;
inline constexpr std::size_t kSlot306MenuItemCount = 23;
inline constexpr std::size_t kSlot306IntVarCount = 3;
inline constexpr std::size_t kSlot306FloatVarCount = 5;
inline constexpr std::size_t kSlot306BoolVarCount = 11;
inline constexpr std::size_t kSlot306CommandCount = 3;
inline constexpr std::uint64_t kSlot306RootCellRva = 0x32f8ff0;
inline constexpr std::uint64_t kSlot306RootCellDtorRva = 0xa5be70;   // pinned, never run
inline constexpr std::uint64_t kSlot306BoolFnVtbl = 0x15d2060;       // functor tag of the 11 bools
inline constexpr std::uint32_t kSlot306VarFlags = 1;
inline constexpr std::uint32_t kSlot306ItemFlags = 0x30;
inline constexpr std::uint64_t kSlot306CommandTargetRva = 0x2dfcbd0; // opaque pointer of item #45
inline constexpr char kSlot306Sound[] = "Sound";                     // 0x12c4d1b, length 5

enum class Slot306Kind : std::uint8_t {
    root_cell,
    int_var,
    float_var,
    bool_var,
    command,
    menu_group,
    menu_slider,
    menu_toggle,
    menu_separator,
    menu_command,
};

// One row of the construction table (checker-pinned and independently re-asserted by the test).
struct Slot306Entry {
    Slot306Kind kind;
    std::uint8_t slot;            // index inside the kind's array of Slot306Objects (items: index - 23)
    std::uint64_t global_rva;     // original .bss address of the object (identity only)
    const char* name;             // console name; caption for menu items ("" for separators/root)
    std::uint64_t atexit_dtor_rva;
    std::uint32_t lo_bits;        // float min / slider range min (IEEE bits)
    std::uint32_t hi_bits;        // float max / slider range max
    std::uint32_t default_bits;   // float default and initial value
    std::int8_t target_index;     // table index of the target object for sliders/toggles, -1 otherwise
    std::uint64_t target_rva;     // opaque target (item #45), 0 otherwise
    const char* parent;           // parent path view ("Sound"; "" for the top group)
    const char* command;          // command name of item #45, "" otherwise
    std::uint64_t handler_rva;    // handler identity tag of the console commands
};

inline constexpr std::array<Slot306Entry, kSlot306ObjectCount> kSlot306Table{{
    {Slot306Kind::root_cell, 0, 0x32f8ff0, "", 0xa5be70, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #0
    {Slot306Kind::int_var, 0, 0x32f8ff8, "sndx_listenerOrientation", 0xa5bea0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #1
    {Slot306Kind::int_var, 1, 0x32f90d0, "sndx_listenerPosition", 0xa5bea0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #2
    {Slot306Kind::float_var, 0, 0x32f91a8, "sndx_listenerPlayerOffset", 0x20f0, 0x0, 0x42c80000, 0x0, -1, 0x0, "", "", 0x0},  // #3
    {Slot306Kind::bool_var, 0, 0x32f9280, "sndx_drawListenerPosition", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #4
    {Slot306Kind::bool_var, 1, 0x32f9348, "sndx_showEnvironmentInfo", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #5
    {Slot306Kind::bool_var, 2, 0x32f9410, "sndx_showWorldAmbientInfo", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #6
    {Slot306Kind::bool_var, 3, 0x32f94d8, "sndx_showTouchVolumes", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #7
    {Slot306Kind::bool_var, 4, 0x32f95a0, "sndx_showDuckingInfo", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #8
    {Slot306Kind::bool_var, 5, 0x32f9668, "sndx_showGlobalInstVars", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #9
    {Slot306Kind::bool_var, 6, 0x32f9730, "sndx_onlyUseLastCore", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #10
    {Slot306Kind::float_var, 1, 0x32f9810, "sndx_masterVolume", 0x20f0, 0x0, 0x3f800000, 0x3f800000, -1, 0x0, "", "", 0x0},  // #11
    {Slot306Kind::bool_var, 7, 0x32f98e8, "sndx_attachSndxDebugger", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #12
    {Slot306Kind::bool_var, 8, 0x32f99b0, "sndx_showBanks", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #13
    {Slot306Kind::int_var, 2, 0x32f9a78, "sndx_showActive", 0xa5bee0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #14
    {Slot306Kind::bool_var, 9, 0x32f9b50, "sndx_showActive3d", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #15
    {Slot306Kind::bool_var, 10, 0x32f9c18, "sndx_showSystemInfo", 0x20b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #16
    {Slot306Kind::float_var, 2, 0x32f9ce0, "sndx_debugGlobalAlpha", 0x20f0, 0x0, 0x3f800000, 0x3f800000, -1, 0x0, "", "", 0x0},  // #17
    {Slot306Kind::float_var, 3, 0x32f9db8, "sndx_debugLingerTime", 0x20f0, 0x0, 0x41200000, 0x0, -1, 0x0, "", "", 0x0},  // #18
    {Slot306Kind::float_var, 4, 0x32f9e90, "sndx_debugLingerFadeTime", 0x20f0, 0x0, 0x41200000, 0x3f000000, -1, 0x0, "", "", 0x0},  // #19
    {Slot306Kind::command, 0, 0x32f9f68, "sndx_reloadData", 0x4b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0xa5bf20},  // #20
    {Slot306Kind::command, 1, 0x32fa008, "sndx_categoryMuteAll", 0x4b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0xa5bf50},  // #21
    {Slot306Kind::command, 2, 0x32fa0a8, "sndx_categoryFilterAll", 0x4b0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0xa5c020},  // #22
    {Slot306Kind::menu_group, 0, 0x32fa148, "Sound", 0x272d0, 0x0, 0x0, 0x0, -1, 0x0, "", "", 0x0},  // #23
    {Slot306Kind::menu_slider, 1, 0x32fa1f0, "Master Volume", 0x272d0, 0x3c23d70a, 0x3dcccccd, 0x0, 11, 0x0, "Sound", "", 0x0},  // #24
    {Slot306Kind::menu_group, 2, 0x32fa298, "Category Mute", 0x272d0, 0x0, 0x0, 0x0, -1, 0x0, "Sound", "", 0x0},  // #25
    {Slot306Kind::menu_separator, 3, 0x32fa340, "", 0x272d0, 0x0, 0x0, 0x0, -1, 0x0, "Sound", "", 0x0},  // #26
    {Slot306Kind::menu_toggle, 4, 0x32fa3e8, "Show System Info", 0x272d0, 0x0, 0x0, 0x0, 16, 0x0, "Sound", "", 0x0},  // #27
    {Slot306Kind::menu_toggle, 5, 0x32fa490, "Show Banks", 0x272d0, 0x0, 0x0, 0x0, 13, 0x0, "Sound", "", 0x0},  // #28
    {Slot306Kind::menu_toggle, 6, 0x32fa538, "Show Environment Info", 0x272d0, 0x0, 0x0, 0x0, 5, 0x0, "Sound", "", 0x0},  // #29
    {Slot306Kind::menu_toggle, 7, 0x32fa5e0, "Show World Ambient Info", 0x272d0, 0x0, 0x0, 0x0, 6, 0x0, "Sound", "", 0x0},  // #30
    {Slot306Kind::menu_toggle, 8, 0x32fa688, "Draw Touch Volumes", 0x272d0, 0x0, 0x0, 0x0, 7, 0x0, "Sound", "", 0x0},  // #31
    {Slot306Kind::menu_toggle, 9, 0x32fa730, "Show Ducking Info", 0x272d0, 0x0, 0x0, 0x0, 8, 0x0, "Sound", "", 0x0},  // #32
    {Slot306Kind::menu_toggle, 10, 0x32fa7d8, "Show Global Inst Vars", 0x272d0, 0x0, 0x0, 0x0, 9, 0x0, "Sound", "", 0x0},  // #33
    {Slot306Kind::menu_separator, 11, 0x32fa880, "", 0x272d0, 0x0, 0x0, 0x0, -1, 0x0, "Sound", "", 0x0},  // #34
    {Slot306Kind::menu_toggle, 12, 0x32fa928, "Show Active", 0x272d0, 0x0, 0x0, 0x0, 14, 0x0, "Sound", "", 0x0},  // #35
    {Slot306Kind::menu_toggle, 13, 0x32fa9d0, "Show Active 3d", 0x272d0, 0x0, 0x0, 0x0, 15, 0x0, "Sound", "", 0x0},  // #36
    {Slot306Kind::menu_group, 14, 0x32faa78, "Category Filters", 0x272d0, 0x0, 0x0, 0x0, -1, 0x0, "Sound", "", 0x0},  // #37
    {Slot306Kind::menu_slider, 15, 0x32fab20, "Inactive Linger Time", 0x272d0, 0x3dcccccd, 0x3f000000, 0x0, 18, 0x0, "Sound", "", 0x0},  // #38
    {Slot306Kind::menu_separator, 16, 0x32fabc8, "", 0x272d0, 0x0, 0x0, 0x0, -1, 0x0, "Sound", "", 0x0},  // #39
    {Slot306Kind::menu_toggle, 17, 0x32fac70, "Draw Listener Position", 0x272d0, 0x0, 0x0, 0x0, 4, 0x0, "Sound", "", 0x0},  // #40
    {Slot306Kind::menu_toggle, 18, 0x32fad18, "Listener Orientation", 0x272d0, 0x0, 0x0, 0x0, 1, 0x0, "Sound", "", 0x0},  // #41
    {Slot306Kind::menu_toggle, 19, 0x32fadc0, "Listener Position", 0x272d0, 0x0, 0x0, 0x0, 2, 0x0, "Sound", "", 0x0},  // #42
    {Slot306Kind::menu_slider, 20, 0x32fae68, "Player Position Offset", 0x272d0, 0x3dcccccd, 0x3f800000, 0x0, 3, 0x0, "Sound", "", 0x0},  // #43
    {Slot306Kind::menu_separator, 21, 0x32faf10, "", 0x272d0, 0x0, 0x0, 0x0, -1, 0x0, "Sound", "", 0x0},  // #44
    {Slot306Kind::menu_command, 22, 0x32fafb8, "Reload All Sound Data", 0x272d0, 0x0, 0x0, 0x0, -1, 0x2dfcbd0, "Sound", "sndx_reloadData", 0x0},  // #45
}};

// Construction gaps: objects that could not be built from existing pieces. Empty - all 46 are built.
inline constexpr std::array<std::size_t, 0> kSlot306Gaps{};

// What this fragment deliberately does not model (neither is an object of the table).
inline constexpr std::array<const char*, 6> kSlot306NotModelled{{
    "handler bodies 0xa5bf20/0xa5bf50/0xa5c020 (identity tags, never called)",
    "class destructors 0xa5be70/0xa5bea0/0xa5bee0/0x20f0/0x20b0/0x4b0/0x272d0 (recorded, never run)",
    "functor table slot bodies beyond the lifted copy/clone/move/bare-ret destroy",
    "vtable slot 4/5 of the console command (execute path) and the argument-record vector fill",
    "the non-null ConCmd manager's real map layout and the non-null debug-menu parent insert 0x27470",
    "all consumers of the sndx_ values (sound engine, listener, debug draw)",
}};

// Table-ordered storage. Menu items are items[index - 23].
struct Slot306Objects {
    std::uint64_t root_cell = 0;                                        // #0 0x32f8ff0
    std::array<ConCmdRangedVar, kSlot306IntVarCount> int_vars{};        // #1 #2 #14
    std::array<ConCmdRangedVar, kSlot306FloatVarCount> floats{};        // #3 #11 #17 #18 #19
    std::array<ConCmdBoolVar, kSlot306BoolVarCount> bools{};            // #4..#10 #12 #13 #15 #16
    std::array<ConCmdCommand, kSlot306CommandCount> commands{};         // #20 #21 #22
    std::array<DebugMenuItem, kSlot306MenuItemCount> items{};           // #23..#45
    std::uint64_t command_target_cell = 0;                              // native stand-in for 0x2dfcbd0
    // The allocator the four MenuSeparator texts came from; their storage is returned to it by the
    // destructor. Set by init_slot306_a633b0 only after a separator was built.
    AllocDispatchContract owned_alloc{};

    Slot306Objects() = default;
    Slot306Objects(const Slot306Objects&) = delete;
    Slot306Objects& operator=(const Slot306Objects&) = delete;
    ~Slot306Objects();
};

struct Slot306Hooks {
    SharedStrCtorHooks separator_alloc{};   // allocator behind 0xf1a0 ("MenuSeparator")
    DebugMenuAssertHooks menu_assert{};     // gated log of 0x268b0 (unreachable with flags 0x30)
};

// Host allocator stand-in (malloc/free) for the "MenuSeparator" strings. NOT the engine allocator.
[[nodiscard]] Slot306Hooks default_slot306_hooks() noexcept;

enum class Slot306Stage : std::uint8_t {
    none,
    root_cell_atexit,
    int_var_site,
    float_ctor,
    float_view_release,
    float_functor_destroy,
    float_registration,
    float_atexit,
    bool_var,
    command_build,
    menu_target_unknown,
    menu_ctor,
    menu_separator_alloc,
    menu_registration,
    menu_view_release,
    menu_atexit,
    bad_table,
};
[[nodiscard]] const char* slot306_stage_name(Slot306Stage stage) noexcept;

struct Slot306Blocked {
    std::size_t index = kSlot306ObjectCount;   // table index of the object that stopped the fragment
    Slot306Stage stage = Slot306Stage::none;
};

struct Slot306Result {
    bool ok = false;                          // every one of the 46 objects built, no gap
    std::size_t built_count = 0;              // objects completed before the first failure
    std::array<bool, kSlot306ObjectCount> built{};
    Slot306Blocked blocked{};
    std::size_t gap_count = 0;                // == kSlot306Gaps.size(): objects not built from existing pieces
    std::size_t menu_asserts_emitted = 0;
};

// Native address of table object `index` inside `objects` (the object a menu item targets).
[[nodiscard]] const void* slot306_object_address(const Slot306Objects& objects, std::size_t index) noexcept;

// Runs 0xa633b0 in table order against the shared registries. Stops at the first failing object (the
// original is unconditional, so a partial run is never reported as success): `blocked` names it.
[[nodiscard]] Slot306Result init_slot306_a633b0(Slot306Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu, const Slot306Hooks& hooks) noexcept;

}  // namespace sotc
