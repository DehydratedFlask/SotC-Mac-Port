// Base Initialize orchestrator `0x255130` (79 B; called from Init 0x531d60 @ 0x531dda).
//
// OBS control flow:
//   call 0x255180          // prologue (448 B) — early fragment in prologue_255180.*;
//                          //   injectable hook here if not pre-run
//   vcall [vptr+0x30]       // app vtable slot → 0x534130 (OBS @ 0x15c8d08); bool — E418 lift
//   jz → return false
//   call 0x255340          // mid debug menus — E419 lift
//   vcall [vptr+0x40]       // app vtable slot → 0x533100 (OBS @ 0x15c8d18); (self, arg)
//                          //   3956 B — NOT modelled; injectable hook
//   rdi=*0x2ea4718; sil=0; call 0x357f0  // leaf modelled (xchg_store_u32_357f0)
//   return true
//
// Appmain triage: vslots 6/8 = 0x534130 / 0x533100. E418 lifts 0x534130; E419 lifts
// 0x255340; E421 lifts bounded 0x533100 header (install only). No leaf-vec; not idx 95;
// not in recovered_constructors(). PR #5 heap cite-only.
#pragma once

#include "allocator_front.hpp"
#include "mid_255340.hpp"
#include "vslot_30_534130.hpp"
#include "vslot_40_533100_header.hpp"
#include "xchg_store_u32_357f0.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kBaseInitialize255130Rva = 0x255130;
inline constexpr std::size_t kBaseInitialize255130Size = 79;
inline constexpr std::uint64_t kAppVtableSlot30 = 0x15c8d08;  // → 0x534130
inline constexpr std::uint64_t kAppVtableSlot40 = 0x15c8d18;  // → 0x533100
inline constexpr std::uint64_t kAppVslot534130 = 0x534130;
inline constexpr std::uint64_t kAppVslot533100 = 0x533100;
inline constexpr std::uint64_t kBaseInitFlagCellRva = 0x2ea4718;  // *cell → obj for 0x357f0

struct BaseInitialize255130Hooks {
    // Models call 0x255180(self). Required for success path; null → treat as no-op.
    void (*prologue_255180)(void* self) = nullptr;
    // Models vcall [vptr+0x30](self). If null and use_lifted_vslot_30, run E418 lift.
    // If both unset → false (Init early-out).
    bool (*vslot_30)(void* self) = nullptr;
    bool use_lifted_vslot_30 = false;
    Vslot30_534130Hooks vslot_30_detail{};
    // Models call 0x255340(self). If null and use_lifted_mid_255340, run E419 lift.
    void (*mid_255340)(void* self) = nullptr;
    bool use_lifted_mid_255340 = false;
    Mid255340Hooks mid_255340_detail{};
    // Models vcall [vptr+0x40](self, arg) → 0x533100. If null and use_lifted_vslot_40_header
    // (+ alloc), run E421 header install only. Else null → no-op.
    void (*vslot_40)(void* self, void* arg) = nullptr;
    bool use_lifted_vslot_40_header = false;
    AllocatorFront* vslot_40_alloc = nullptr;  // required when use_lifted_vslot_40_header
    Vslot40_533100HeaderHooks vslot_40_detail{};
    // Last header result when lifted (for measured slices).
    Vslot40_533100HeaderResult* vslot_40_out = nullptr;
};

// Process-wide cell modelling original qword @ 0x2ea4718 (points at flag object).
[[nodiscard]] void*& base_init_flag_cell_2ea4718() noexcept;

// Orchestrator. On vslot_30 failure returns false without calling mid/vslot_40/0x357f0.
// On success: hooks as above, then xchg_store_u32_357f0(*cell, 0); returns true.
// Fail-visible if cell is null on the success path (returns false after hooks).
[[nodiscard]] bool base_initialize_255130(void* self, void* arg,
                                          const BaseInitialize255130Hooks& hooks) noexcept;

}  // namespace sotc
