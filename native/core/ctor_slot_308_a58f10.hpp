// CPU-01-STARTUP-SLOT-308-01: native recovery of original constructor-table index 308 / RVA 0xa58f10
// (1119 B, [0xa58f10, 0xa5936f)), ELF sha256 d0723afb...5861d3: the "Save System" block of the
// "Platform Services" debug menu: the `sSaveManagerShowInfoLevel` ranged int variable (0..3), the
// `sSaveManagerSaveEnabled` bool variable, a "Save System" group and two toggle items. Every byte below
// was read from the pinned image with xcrun llvm-objdump and is pinned by tools/lift/check_ctor_slot_308.py.
// Nothing here executes an original RVA; vtable pointers, destructors and functor tags are identity tags.
//
// Sequence (.bss addresses):
//   1. cell      0x32f84f8 = 0;                                   __cxa_atexit(0xa50d50, cell)
//   2. level     0x32f8508 = sSaveManagerShowInfoLevel: 0xc580 (core/ranged_int_var_c580) with name view
//                (0x12c40c0, 0x19), description view (0x12c40da, 0x28), value cell = this+0xd0, min 0,
//                max 3, flags 1, argument block all-zero with functor tag 0x15d1d80; the caller then
//                destroys that functor (slot 3 = 0xc770, bare ret), stores the final vptr 0x1594c08,
//                value = 0, default = 0, calls 0x20d60 (register), 0xf590(description), 0xf590(name),
//                __cxa_atexit(0x18a60, level)
//   3. enabled   0x32f85e0 = sSaveManagerSaveEnabled (bool shape: empty description view, flags 1,
//                functor tag 0x15d1da0, value 1, default 1; core/concmd_vars init_bool_var_static;
//                __cxa_atexit(0x20b0))
//   4. group     0x32f86a8 = 0x26c10(parent "Platform Services" (0x12c411b, 0x11), caption "Save System"
//                (0x12c412d, 0xb), flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0). The caption is a STACK
//                view here (slot 307 uses a global one).
//   5. toggle 1  0x32f8750 = 0x26ea0(label "Save System", text "Show Info Level" (0x12c4139, 0xf),
//                target = level, flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0). The int class 0x1594c08
//                answers slot 3 = 1 and slot 6 = 2 so the default range {1.0, 10.0} is kept.
//   6. toggle 2  0x32f87f8 = 0x26ea0(label "Save System", text "Enable Save System" (0x12c4149, 0x12),
//                target = enabled, flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0). Bool slots {1, 0}.
//   7. tail      0x32f88a8: bytes +0x08..+0x1f zeroed (+0x00 is not written); __cxa_atexit(0xa55f30)
// The destructors 0xa50d50 / 0xa55f30 are identity tags (not lifted, never called). The consumers of the
// two variables (the save manager) are NOT modelled: no save behaviour is claimed by this module.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"
#include "ranged_int_var_c580.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot308Rva = 0xa58f10;
inline constexpr std::size_t kSlot308BodySize = 1119;
inline constexpr std::uint64_t kSlot308CellRva = 0x32f84f8;
inline constexpr std::uint64_t kSlot308CellDtorRva = 0xa50d50;      // identity tag
inline constexpr std::uint64_t kSlot308LevelRva = 0x32f8508;
inline constexpr std::uint64_t kSlot308EnabledRva = 0x32f85e0;
inline constexpr std::uint64_t kSlot308GroupRva = 0x32f86a8;
inline constexpr std::uint64_t kSlot308Toggle1Rva = 0x32f8750;
inline constexpr std::uint64_t kSlot308Toggle2Rva = 0x32f87f8;
inline constexpr std::uint64_t kSlot308TailRva = 0x32f88a8;
inline constexpr std::uint64_t kSlot308TailDtorRva = 0xa55f30;      // identity tag
inline constexpr std::uint64_t kSlot308ArgFnVtbl = 0x15d1d80;       // caller's argument functor tag
inline constexpr std::uint64_t kSlot308BoolFnVtbl = 0x15d1da0;      // bool functor tag
inline constexpr std::int32_t kSlot308LevelMin = 0;
inline constexpr std::int32_t kSlot308LevelMax = 3;
inline constexpr std::uint32_t kSlot308LevelFlags = 1;
inline constexpr std::uint8_t kSlot308EnabledValue = 1;
inline constexpr std::uint8_t kSlot308EnabledDefault = 1;
inline constexpr std::uint32_t kSlot308ItemFlags = 0x30;
inline constexpr char kSlot308LevelName[] = "sSaveManagerShowInfoLevel";                   // 0x12c40c0, 0x19
inline constexpr char kSlot308LevelDesc[] = "Save System Debug Info Level from 0 to 3";   // 0x12c40da, 0x28
inline constexpr char kSlot308EnabledName[] = "sSaveManagerSaveEnabled";                  // 0x12c4103, 0x17
inline constexpr char kSlot308Parent[] = "Platform Services";                             // 0x12c411b, 0x11
inline constexpr char kSlot308Caption[] = "Save System";                                  // 0x12c412d, 0xb
inline constexpr char kSlot308Toggle1Text[] = "Show Info Level";                          // 0x12c4139, 0xf
inline constexpr char kSlot308Toggle2Text[] = "Enable Save System";                       // 0x12c4149, 0x12
static_assert(sizeof kSlot308LevelName - 1 == 0x19 && sizeof kSlot308LevelDesc - 1 == 0x28);
static_assert(sizeof kSlot308EnabledName - 1 == 0x17 && sizeof kSlot308Parent - 1 == 0x11);
static_assert(sizeof kSlot308Caption - 1 == 0xb && sizeof kSlot308Toggle1Text - 1 == 0xf);
static_assert(sizeof kSlot308Toggle2Text - 1 == 0x12);

struct Slot308Tail {
    std::uint64_t untouched = 0;             // +0x00 never written by the constructor
    std::array<std::uint64_t, 3> zero{};     // +0x08..+0x1f
};

struct Slot308Objects {
    std::uint64_t cell = 0;                  // 0x32f84f8
    ConCmdRangedVar level{};                 // 0x32f8508
    ConCmdBoolVar enabled{};                 // 0x32f85e0
    DebugMenuItem group{};                   // 0x32f86a8
    DebugMenuItem toggle1{};                 // 0x32f8750
    DebugMenuItem toggle2{};                 // 0x32f87f8
    Slot308Tail tail{};                      // 0x32f88a8
};

enum class Slot308Stage : std::uint8_t {
    none,
    cell_atexit,
    level_build,
    level_registration,
    level_view_release,
    level_atexit,
    enabled_build,
    group_ctor,
    group_registration,
    group_view_release,
    group_atexit,
    toggle1_ctor,
    toggle1_registration,
    toggle1_view_release,
    toggle1_atexit,
    toggle2_ctor,
    toggle2_registration,
    toggle2_view_release,
    toggle2_atexit,
    tail_atexit,
};
[[nodiscard]] const char* slot308_stage_name(Slot308Stage stage) noexcept;

struct Slot308Result {
    bool ok = false;
    Slot308Stage blocked = Slot308Stage::none;
    ConCmdRegisterResult level_registration{};
    BoolVarResult enabled{};
    MenuRegisterResult group_registration{};
    MenuRegisterResult toggle1_registration{};
    MenuRegisterResult toggle2_registration{};
};

[[nodiscard]] Slot308Result init_slot308_a58f10(Slot308Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const DebugMenuAssertHooks& hooks = {}) noexcept;

}  // namespace sotc
