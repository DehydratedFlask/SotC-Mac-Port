// CPU-01-STARTUP-SLOT-448 (lane m6): native model of startup constructor slot 448 (RVA 0x4b8680, 501 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 448. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_448.py. NOTHING here
// executes, loads or calls an original RVA: the vtables, the functor tag 0x15c0c30 and the destructor 0x20b0 are
// IDENTITY TAGS, recorded and never run.
//
// Observed shape (stack canary present): two BOOL console variables back to back, each built exactly like the
// shared init_bool_var_static sequence (base constructor 0x20af0, mid vptr, value_ref = var + 0xc0, zero fields,
// caller functor tag 0x15c0c30, final vptr, value, default, registration 0x20d60, two 0xf590 view releases,
// __cxa_atexit(0x20b0, var)):
//   * 0x2efa6e0  `shadow_DisableWorldZoomOut`      (name length 0x1a)
//                description `Disables the world zooming out effect when transitioning to the pause menu.`
//                (length 0x4b), value 1 / default 1
//   * 0x2efa7a8  `shadow_mapShowBossCloudIndices`  (name length 0x1e, = first variable + 0xc8)
//                description `If true it displays the boss cloud index over the first icon in the group.`
//                (length 0x4a), value 0 / default 0
// Name and description are stack views {literal, immediate length, 0, 0} (control == nullptr), so both 0xf590
// releases are exact no-ops. Observation: the original releases the DESCRIPTION view before the NAME view only
// after registration, whereas init_bool_var_static releases the description before registration; with borrowed
// views both are inert and unobservable, so the shared model is used unchanged (inference, not an observed
// difference in effects). The tag 0x15c0c30's four relocation slots are the bool functor bodies (checker).
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot448Rva = 0x4b8680;
inline constexpr std::size_t kSlot448Size = 501;
inline constexpr std::uint64_t kSlot448BoolFnTag = 0x15c0c30;
inline constexpr char kSlot448Var1Name[] = "shadow_DisableWorldZoomOut";
inline constexpr std::size_t kSlot448Var1NameLength = 0x1a;
inline constexpr char kSlot448Var1Description[] =
    "Disables the world zooming out effect when transitioning to the pause menu.";
inline constexpr std::size_t kSlot448Var1DescriptionLength = 0x4b;
inline constexpr char kSlot448Var2Name[] = "shadow_mapShowBossCloudIndices";
inline constexpr std::size_t kSlot448Var2NameLength = 0x1e;
inline constexpr char kSlot448Var2Description[] =
    "If true it displays the boss cloud index over the first icon in the group.";
inline constexpr std::size_t kSlot448Var2DescriptionLength = 0x4a;

struct Slot448Objects {
    ConCmdBoolVar disable_world_zoom_out;    // 0x2efa6e0
    ConCmdBoolVar map_show_boss_cloud_idx;   // 0x2efa7a8
};

struct Slot448Result {
    bool ok = false;
    BoolVarResult first{};
    BoolVarResult second{};
};

[[nodiscard]] Slot448Result init_slot448_4b8680(Slot448Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
