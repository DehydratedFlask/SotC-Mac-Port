// CPU-01-STARTUP-SLOT-313-01: native recovery of original constructor-table index 313 / RVA 0xa447a0
// (661 B, [0xa447a0, 0xa44a35)), ELF sha256 d0723afb...5861d3: the `update_entitlement_status` console
// command with its "Entitlement System" debug-menu group and "Update Entitlement State" item, in the
// "Platform Services" menu. Every byte below was read from the pinned image with xcrun llvm-objdump
// and is pinned by tools/lift/check_ctor_slot_313.py. Nothing here executes an original RVA; vtable
// pointers, destructors and the handler are identity tags only.
//
// Sequence (all addresses are .bss unless noted):
//   1. cell      0x32f6d18 = 0;                 __cxa_atexit(0xa42900, cell)
//   2. caption   0x32f6d28 = SharedStr{"Entitlement System" (0x12c2ce0), 0x12, 0, 0};
//                                               __cxa_atexit(0xf7b0, caption)      ; SharedStr dtor tag
//   3. command   0x32f6d48 = update_entitlement_status ("Updates the entitlement status", 0x1e);
//                handler tag 0xa42930; the shared console-command shape (0xf2f0, 0x20af0 flags 2, vptr
//                0x1594dd0, delegate {self, 0x1990, 0, handler}, zero vector, register via vtable slot
//                2 = 0x20d60, 0xf590 x2, __cxa_atexit(0x4b0)) is core/fconcmd_object_slot306.
//   4. group     0x32f6de8 = 0x26c10(parent "Platform Services" (0x12c2d2c, 0x11), caption = the
//                global caption of step 2, flags 0x30); 0xf590(parent view); __cxa_atexit(0x272d0)
//   5. toggle    0x32f6e90 = 0x26ea0(label = the global caption, text "Update Entitlement State"
//                (0x12c2d3e, 0x18), target = the command of step 3, flags 0x30); 0xf590(text view);
//                __cxa_atexit(0x272d0). The class of the target is the command class, whose slot 3
//                returns 0, so the default range {1.0, 10.0} is kept (see l2_slot_common.hpp).
//   6. tail      0x32f6f38..0x32f6f4f = 24 zero bytes;  __cxa_atexit(0xa42950, tail)
// Both menu items are appended to the debug-menu root list (the parent cell is null at startup).
// The handler 0xa42930 and the destructors 0xa42900 / 0xa42950 are identity tags: not lifted, never
// called. What the handler does when the command runs is NOT modelled (no consumer exists yet).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot313Rva = 0xa447a0;
inline constexpr std::size_t kSlot313BodySize = 661;
inline constexpr std::uint64_t kSlot313CellRva = 0x32f6d18;
inline constexpr std::uint64_t kSlot313CellDtorRva = 0xa42900;      // identity tag
inline constexpr std::uint64_t kSlot313CaptionRva = 0x32f6d28;
inline constexpr std::uint64_t kSlot313CaptionDtorRva = 0xf7b0;     // identity tag
inline constexpr std::uint64_t kSlot313CommandRva = 0x32f6d48;
inline constexpr std::uint64_t kSlot313HandlerRva = 0xa42930;       // identity tag
inline constexpr std::uint64_t kSlot313GroupRva = 0x32f6de8;
inline constexpr std::uint64_t kSlot313ToggleRva = 0x32f6e90;
inline constexpr std::uint64_t kSlot313TailRva = 0x32f6f38;
inline constexpr std::uint64_t kSlot313TailDtorRva = 0xa42950;      // identity tag
inline constexpr std::uint32_t kSlot313ItemFlags = 0x30;
inline constexpr std::size_t kSlot313CaptionLength = 0x12;
inline constexpr std::size_t kSlot313DescriptionLength = 0x1e;
inline constexpr std::size_t kSlot313ParentLength = 0x11;
inline constexpr std::size_t kSlot313TextLength = 0x18;
inline constexpr char kSlot313Caption[] = "Entitlement System";            // 0x12c2ce0
inline constexpr char kSlot313CommandName[] = "update_entitlement_status";  // 0x12c2cf3
inline constexpr char kSlot313Description[] = "Updates the entitlement status";  // 0x12c2d0d
inline constexpr char kSlot313Parent[] = "Platform Services";              // 0x12c2d2c
inline constexpr char kSlot313Text[] = "Update Entitlement State";         // 0x12c2d3e
static_assert(sizeof kSlot313Caption - 1 == kSlot313CaptionLength);
static_assert(sizeof kSlot313Description - 1 == kSlot313DescriptionLength);
static_assert(sizeof kSlot313Parent - 1 == kSlot313ParentLength);
static_assert(sizeof kSlot313Text - 1 == kSlot313TextLength);

struct Slot313Objects {
    std::uint64_t cell = 0;                  // 0x32f6d18
    SharedStr caption{};                     // 0x32f6d28
    ConCmdCommand command{};                 // 0x32f6d48 (0xa0)
    DebugMenuItem group{};                   // 0x32f6de8
    DebugMenuItem toggle{};                  // 0x32f6e90
    std::array<std::uint64_t, 3> tail{};     // 0x32f6f38 (24 zero bytes)
};

enum class Slot313Stage : std::uint8_t {
    none,
    cell_atexit,
    caption_atexit,
    command_build,
    group_ctor,
    group_registration,
    group_view_release,
    group_atexit,
    toggle_ctor,
    toggle_registration,
    toggle_view_release,
    toggle_atexit,
    tail_atexit,
};
[[nodiscard]] const char* slot313_stage_name(Slot313Stage stage) noexcept;

struct Slot313Result {
    bool ok = false;
    Slot313Stage blocked = Slot313Stage::none;   // first failing stage; none when ok
    FConCmdBuildResult command{};
    MenuRegisterResult group_registration{};
    MenuRegisterResult toggle_registration{};
};

// Models 0xa447a0 against the shared ConCmd registry and the debug-menu registry. `ok == false` names
// the stage that stopped the fragment (the original is unconditional, so a partial run is never success).
[[nodiscard]] Slot313Result init_slot313_a447a0(Slot313Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const DebugMenuAssertHooks& hooks = {}) noexcept;

}  // namespace sotc
