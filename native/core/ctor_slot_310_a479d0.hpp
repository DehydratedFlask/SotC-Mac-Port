// CPU-01-STARTUP-SLOT-310-01: native recovery of original constructor-table index 310 / RVA 0xa479d0
// (723 B, [0xa479d0, 0xa47ca3)), ELF sha256 d0723afb...5861d3: the title-info console variables and their
// debug-menu toggles. Every byte below was read from the pinned image with xcrun llvm-objdump and is pinned
// by tools/lift/check_ctor_slot_310.py. Nothing here executes an original RVA; vtable pointers, destructors
// and functor tags are identity tags.
//
// Sequence (.bss addresses):
//   1. titleid  0x32f7648 = string variable (0x257a0; class 0x1594ef0): name "titleid" (0x12c30e0, 7),
//               initial/default value = the empty view {0x12c30e8, 0, 0, 0}; 0xf590 x2 (inert views);
//               __cxa_atexit(0x23c90, obj)                          [core/string_var_257a0]
//   2. circle   0x32f7790 = bool variable `useCircleAsEnter` (0x12c30e9, 0x10), empty description 0x12c30e8,
//               functor tag 0x15d19b0, value 0 / default 0, flags 1; __cxa_atexit(0x20b0)
//                                                                   [shape: init_bool_var_static]
//   3. toggle A 0x32f7858 = 0x26ea0(label "TitleInfo" (0x12c30fa, 9), text "Title ID" (0x12c3104, 8),
//               target = titleid, flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0). The first debug-menu
//               registration initialises the root list and records __cxa_atexit(0x27280).
//   4. toggle B 0x32f7900 = 0x26ea0(label "TitleInfo", text "Circle as Enter" (0x12c310d, 0xf),
//               target = circle, flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0)
//   5. tail     0x32f79a8 = qword 0 (a null owning pointer); __cxa_atexit(0xa46ba0, tail)
// There is NO group item (both toggles share the existing "TitleInfo" label). The string class answers the
// two 0x26ea0 virtual calls with slot 3 = 1 and slot 6 = 4, so the toggle keeps the default range
// {1.0, 10.0}; the bool class answers {1, 0}. What the variables DO at run time (title id reporting, the
// circle/cross enter-button swap) is NOT modelled: no consumer exists in the native tree yet.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"
#include "string_var_257a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot310Rva = 0xa479d0;
inline constexpr std::size_t kSlot310BodySize = 723;
inline constexpr std::uint64_t kSlot310TitleIdRva = 0x32f7648;
inline constexpr std::uint64_t kSlot310CircleRva = 0x32f7790;
inline constexpr std::uint64_t kSlot310ToggleTitleRva = 0x32f7858;
inline constexpr std::uint64_t kSlot310ToggleCircleRva = 0x32f7900;
inline constexpr std::uint64_t kSlot310TailRva = 0x32f79a8;
inline constexpr std::uint64_t kSlot310TailDtorRva = 0xa46ba0;     // identity tag
inline constexpr std::uint64_t kSlot310CircleFnVtbl = 0x15d19b0;   // functor tag (slots 0xc2a0..0xc2f0)
inline constexpr std::uint32_t kSlot310ItemFlags = 0x30;
inline constexpr char kSlot310TitleIdName[] = "titleid";                // 0x12c30e0, 7
inline constexpr char kSlot310CircleName[] = "useCircleAsEnter";        // 0x12c30e9, 0x10
inline constexpr char kSlot310Label[] = "TitleInfo";                    // 0x12c30fa, 9
inline constexpr char kSlot310TitleText[] = "Title ID";                 // 0x12c3104, 8
inline constexpr char kSlot310CircleText[] = "Circle as Enter";         // 0x12c310d, 0xf
static_assert(sizeof kSlot310TitleIdName - 1 == 7 && sizeof kSlot310CircleName - 1 == 0x10);
static_assert(sizeof kSlot310Label - 1 == 9 && sizeof kSlot310TitleText - 1 == 8 &&
              sizeof kSlot310CircleText - 1 == 0xf);

struct Slot310Objects {
    ConCmdStringVar titleid{};      // 0x32f7648 (0x148)
    ConCmdBoolVar circle{};         // 0x32f7790 (0xc8)
    DebugMenuItem toggle_title{};   // 0x32f7858
    DebugMenuItem toggle_circle{};  // 0x32f7900
    std::uint64_t tail = 0;         // 0x32f79a8
};

enum class Slot310Stage : std::uint8_t {
    none,
    titleid_build,
    titleid_view_release,
    titleid_atexit,
    circle_build,
    title_ctor,
    title_registration,
    title_view_release,
    title_atexit,
    circle_toggle_ctor,
    circle_toggle_registration,
    circle_toggle_view_release,
    circle_toggle_atexit,
    tail_atexit,
};
[[nodiscard]] const char* slot310_stage_name(Slot310Stage stage) noexcept;

struct Slot310Result {
    bool ok = false;
    Slot310Stage blocked = Slot310Stage::none;
    StringVarResult titleid{};
    BoolVarResult circle{};
    MenuRegisterResult title_registration{};
    MenuRegisterResult circle_registration{};
};

[[nodiscard]] Slot310Result init_slot310_a479d0(Slot310Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const DebugMenuAssertHooks& hooks = {}) noexcept;

}  // namespace sotc
