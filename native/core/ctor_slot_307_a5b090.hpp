// CPU-01-STARTUP-SLOT-307-01: native recovery of original constructor-table index 307 / RVA 0xa5b090
// (1288 B, [0xa5b090, 0xa5b598)), ELF sha256 d0723afb...5861d3: the "Trophy System" block of the
// "Platform Services" debug menu: the `trophyProcessingEnabled` bool variable, the `unlockTrophy` and
// `unlockAllTrophies` console commands, a "Trophy System" group and two toggle items. Every byte below
// was read from the pinned image with xcrun llvm-objdump and is pinned by tools/lift/check_ctor_slot_307.py.
// Nothing here executes an original RVA; vtable pointers, destructors and handlers are identity tags.
//
// Sequence (.bss addresses):
//   1. cell      0x32f8b80 = 0;                                   __cxa_atexit(0xa59370, cell)
//   2. variable  0x32f8b90 = trophyProcessingEnabled (bool shape: description is an empty view, flags 1,
//                functor tag 0x15d1dc0, value 0, default 0; core/concmd_vars init_bool_var_static;
//                __cxa_atexit(0x20b0))
//   3. caption   0x32f8c58 = SharedStr{"Trophy System" (0x12c47e8), 0xd, 0, 0};  __cxa_atexit(0xf7b0)
//   4. group     0x32f8c78 = 0x26c10(parent "Platform Services" (0x12c47f6, 0x11), caption = the global,
//                flags 0x30); 0xf590(parent); __cxa_atexit(0x272d0)
//   5. toggle 1  0x32f8d20 = 0x26ea0(label = caption, text "Processing Enabled" (0x12c4808, 0x12),
//                target = the variable, flags 0x30); 0xf590(text); __cxa_atexit(0x272d0). The bool class
//                gives slots {1, 0} so the default range {1.0, 10.0} is kept.
//   6. tails     0x32f8dc8 and 0x32f8de8: 24 zero bytes each; __cxa_atexit(0xa593a0) / (0xa593b0)
//   7. command   0x32f8e08 = unlockTrophy ("Unlocks a trophy. +unlockTrophy <trophy_name>", 0x2d),
//                handler tag 0xa59410, console-command shape of core/fconcmd_object_slot306
//   8. command   0x32f8ea8 = unlockAllTrophies ("Unlocks all trophies.", 0x15), handler tag 0xa595c0
//   9. toggle 2  0x32f8f48 = 0x26ea0(label = caption, text "Unlock All Trophies" (0x12c487e, 0x13),
//                target = command 2, flags 0x30); 0xf590(text); __cxa_atexit(0x272d0). A command target
//                has slot 3 == 0 (see l2_slot_common.hpp) so the default range is kept.
// The destructors 0xa59370 / 0xa593a0 / 0xa593b0 and the handlers 0xa59410 / 0xa595c0 are identity tags
// (not lifted, never called). What the handlers do when a command runs is NOT modelled, and the value
// of `trophyProcessingEnabled` has no native consumer yet: no trophy behaviour is claimed.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot307Rva = 0xa5b090;
inline constexpr std::size_t kSlot307BodySize = 1288;
inline constexpr std::uint64_t kSlot307CellRva = 0x32f8b80;
inline constexpr std::uint64_t kSlot307CellDtorRva = 0xa59370;      // identity tag
inline constexpr std::uint64_t kSlot307VarRva = 0x32f8b90;
inline constexpr std::uint64_t kSlot307FnVtbl = 0x15d1dc0;          // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot307Value = 0;
inline constexpr std::uint8_t kSlot307Default = 0;
inline constexpr std::uint64_t kSlot307CaptionRva = 0x32f8c58;
inline constexpr std::uint64_t kSlot307CaptionDtorRva = 0xf7b0;     // identity tag
inline constexpr std::uint64_t kSlot307GroupRva = 0x32f8c78;
inline constexpr std::uint64_t kSlot307Toggle1Rva = 0x32f8d20;
inline constexpr std::uint64_t kSlot307Tail1Rva = 0x32f8dc8;
inline constexpr std::uint64_t kSlot307Tail1DtorRva = 0xa593a0;     // identity tag
inline constexpr std::uint64_t kSlot307Tail2Rva = 0x32f8de8;
inline constexpr std::uint64_t kSlot307Tail2DtorRva = 0xa593b0;     // identity tag
inline constexpr std::uint64_t kSlot307Command1Rva = 0x32f8e08;
inline constexpr std::uint64_t kSlot307Handler1Rva = 0xa59410;      // identity tag
inline constexpr std::uint64_t kSlot307Command2Rva = 0x32f8ea8;
inline constexpr std::uint64_t kSlot307Handler2Rva = 0xa595c0;      // identity tag
inline constexpr std::uint64_t kSlot307Toggle2Rva = 0x32f8f48;
inline constexpr std::uint32_t kSlot307ItemFlags = 0x30;
inline constexpr char kSlot307VarName[] = "trophyProcessingEnabled";            // 0x12c47d0, 0x17
inline constexpr char kSlot307Caption[] = "Trophy System";                      // 0x12c47e8, 0xd
inline constexpr char kSlot307Parent[] = "Platform Services";                   // 0x12c47f6, 0x11
inline constexpr char kSlot307Toggle1Text[] = "Processing Enabled";             // 0x12c4808, 0x12
inline constexpr char kSlot307Command1Name[] = "unlockTrophy";                  // 0x12c481b
inline constexpr char kSlot307Command1Desc[] = "Unlocks a trophy. +unlockTrophy <trophy_name>";  // 0x12c4828, 0x2d
inline constexpr char kSlot307Command2Name[] = "unlockAllTrophies";             // 0x12c4856
inline constexpr char kSlot307Command2Desc[] = "Unlocks all trophies.";         // 0x12c4868, 0x15
inline constexpr char kSlot307Toggle2Text[] = "Unlock All Trophies";            // 0x12c487e, 0x13
static_assert(sizeof kSlot307VarName - 1 == 0x17 && sizeof kSlot307Caption - 1 == 0xd);
static_assert(sizeof kSlot307Parent - 1 == 0x11 && sizeof kSlot307Toggle1Text - 1 == 0x12);
static_assert(sizeof kSlot307Command1Desc - 1 == 0x2d && sizeof kSlot307Command2Desc - 1 == 0x15);
static_assert(sizeof kSlot307Toggle2Text - 1 == 0x13);

struct Slot307Objects {
    std::uint64_t cell = 0;                  // 0x32f8b80
    ConCmdBoolVar var{};                     // 0x32f8b90
    SharedStr caption{};                     // 0x32f8c58
    DebugMenuItem group{};                   // 0x32f8c78
    DebugMenuItem toggle1{};                 // 0x32f8d20
    std::array<std::uint64_t, 3> tail1{};    // 0x32f8dc8
    std::array<std::uint64_t, 3> tail2{};    // 0x32f8de8
    ConCmdCommand command1{};                // 0x32f8e08
    ConCmdCommand command2{};                // 0x32f8ea8
    DebugMenuItem toggle2{};                 // 0x32f8f48
};

enum class Slot307Stage : std::uint8_t {
    none,
    cell_atexit,
    var_build,
    caption_atexit,
    group_ctor,
    group_registration,
    group_view_release,
    group_atexit,
    toggle1_ctor,
    toggle1_registration,
    toggle1_view_release,
    toggle1_atexit,
    tail1_atexit,
    tail2_atexit,
    command1_build,
    command2_build,
    toggle2_ctor,
    toggle2_registration,
    toggle2_view_release,
    toggle2_atexit,
};
[[nodiscard]] const char* slot307_stage_name(Slot307Stage stage) noexcept;

struct Slot307Result {
    bool ok = false;
    Slot307Stage blocked = Slot307Stage::none;
    BoolVarResult var{};
    FConCmdBuildResult command1{};
    FConCmdBuildResult command2{};
    MenuRegisterResult group_registration{};
    MenuRegisterResult toggle1_registration{};
    MenuRegisterResult toggle2_registration{};
};

[[nodiscard]] Slot307Result init_slot307_a5b090(Slot307Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const DebugMenuAssertHooks& hooks = {}) noexcept;

}  // namespace sotc
