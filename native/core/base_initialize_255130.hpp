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
#include "vslot_40_533100_gfx_2ee2d00.hpp"
#include "vslot_40_533100_header.hpp"
#include "vslot_40_533100_join.hpp"
#include "vslot_40_533100_miss.hpp"
#include "vslot_40_533100_post.hpp"
#include "vslot_40_533100_shadow_attach.hpp"
#include "vslot_40_533100_shadowmodels.hpp"
#include "vslot_40_533100_shadowskels.hpp"
#include "vslot_40_533100_skels_attach.hpp"
#include "vslot_40_533100_shadowanims.hpp"
#include "vslot_40_533100_anims_attach.hpp"
#include "vslot_40_533100_shadowbanks.hpp"
#include "vslot_40_533100_banks_attach.hpp"
#include "vslot_40_533100_shadowstreams.hpp"
#include "vslot_40_533100_streams_attach.hpp"
#include "vslot_40_533100_shadowrandomevents.hpp"
#include "vslot_40_533100_randomevents_attach.hpp"
#include "vslot_40_533100_shadowbossmusic.hpp"
#include "vslot_40_533100_bossmusic_attach.hpp"
#include "vslot_40_533100_cp3characters.hpp"
#include "vslot_40_533100_cp3characters_attach.hpp"
#include "vslot_40_533100_cp3shadowprecache.hpp"
#include "vslot_40_533100_cp3shadowprecache_attach.hpp"
#include "vslot_40_533100_tail_140d90.hpp"
#include "vslot_40_533100_stackchk.hpp"
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
    // (+ alloc), run E421 header + E422 post fragment. Else null → no-op.
    void (*vslot_40)(void* self, void* arg) = nullptr;
    bool use_lifted_vslot_40_header = false;
    AllocatorFront* vslot_40_alloc = nullptr;  // required when use_lifted_vslot_40_header
    Vslot40_533100HeaderHooks vslot_40_detail{};
    Vslot40_533100PostHooks vslot_40_post_detail{};
    Vslot40_533100MissHooks vslot_40_miss_detail{};
    Vslot40_533100JoinHooks vslot_40_join_detail{};
    Vslot40_533100GfxHooks vslot_40_gfx_detail{};
    Vslot40_533100ShadowHooks vslot_40_shadow_detail{};
    // The nine attach fragments build their 0x14c2f0 key and release it with 0xf590. A host must
    // set `key_alloc.alloc.allocate` and `platform_tag` in each attach detail the lifted path
    // reaches; with the null defaults that fragment returns ok == false (no key is faked).
    Vslot40_533100ShadowAttachHooks vslot_40_shadow_attach_detail{};
    Vslot40_533100ShadowskelsHooks vslot_40_shadowskels_detail{};
    Vslot40_533100SkelsAttachHooks vslot_40_skels_attach_detail{};
    Vslot40_533100ShadowanimsHooks vslot_40_shadowanims_detail{};
    Vslot40_533100AnimsAttachHooks vslot_40_anims_attach_detail{};
    Vslot40_533100ShadowbanksHooks vslot_40_shadowbanks_detail{};
    Vslot40_533100BanksAttachHooks vslot_40_banks_attach_detail{};
    Vslot40_533100ShadowstreamsHooks vslot_40_shadowstreams_detail{};
    Vslot40_533100StreamsAttachHooks vslot_40_streams_attach_detail{};
    Vslot40_533100ShadowrandomeventsHooks vslot_40_shadowrandomevents_detail{};
    Vslot40_533100RandomeventsAttachHooks vslot_40_randomevents_attach_detail{};
    Vslot40_533100ShadowbossmusicHooks vslot_40_shadowbossmusic_detail{};
    Vslot40_533100BossmusicAttachHooks vslot_40_bossmusic_attach_detail{};
    Vslot40_533100Cp3charactersHooks vslot_40_cp3characters_detail{};
    Vslot40_533100Cp3charactersAttachHooks vslot_40_cp3characters_attach_detail{};
    Vslot40_533100Cp3shadowprecacheHooks vslot_40_cp3shadowprecache_detail{};
    Vslot40_533100Cp3shadowprecacheAttachHooks vslot_40_cp3shadowprecache_attach_detail{};
    Vslot40_533100Tail140d90Hooks vslot_40_tail_140d90_detail{};
    Vslot40_533100StackchkHooks vslot_40_stackchk_detail{};
    // Last header/post/…/tail-140d90/stackchk results.
    Vslot40_533100HeaderResult* vslot_40_out = nullptr;
    Vslot40_533100PostResult* vslot_40_post_out = nullptr;
    Vslot40_533100MissResult* vslot_40_miss_out = nullptr;
    Vslot40_533100JoinResult* vslot_40_join_out = nullptr;
    Vslot40_533100GfxResult* vslot_40_gfx_out = nullptr;
    Vslot40_533100ShadowResult* vslot_40_shadow_out = nullptr;
    Vslot40_533100ShadowAttachResult* vslot_40_shadow_attach_out = nullptr;
    Vslot40_533100ShadowskelsResult* vslot_40_shadowskels_out = nullptr;
    Vslot40_533100SkelsAttachResult* vslot_40_skels_attach_out = nullptr;
    Vslot40_533100ShadowanimsResult* vslot_40_shadowanims_out = nullptr;
    Vslot40_533100AnimsAttachResult* vslot_40_anims_attach_out = nullptr;
    Vslot40_533100ShadowbanksResult* vslot_40_shadowbanks_out = nullptr;
    Vslot40_533100BanksAttachResult* vslot_40_banks_attach_out = nullptr;
    Vslot40_533100ShadowstreamsResult* vslot_40_shadowstreams_out = nullptr;
    Vslot40_533100StreamsAttachResult* vslot_40_streams_attach_out = nullptr;
    Vslot40_533100ShadowrandomeventsResult* vslot_40_shadowrandomevents_out = nullptr;
    Vslot40_533100RandomeventsAttachResult* vslot_40_randomevents_attach_out = nullptr;
    Vslot40_533100ShadowbossmusicResult* vslot_40_shadowbossmusic_out = nullptr;
    Vslot40_533100BossmusicAttachResult* vslot_40_bossmusic_attach_out = nullptr;
    Vslot40_533100Cp3charactersResult* vslot_40_cp3characters_out = nullptr;
    Vslot40_533100Cp3charactersAttachResult* vslot_40_cp3characters_attach_out = nullptr;
    Vslot40_533100Cp3shadowprecacheResult* vslot_40_cp3shadowprecache_out = nullptr;
    Vslot40_533100Cp3shadowprecacheAttachResult* vslot_40_cp3shadowprecache_attach_out =
        nullptr;
    Vslot40_533100Tail140d90Result* vslot_40_tail_140d90_out = nullptr;
    Vslot40_533100StackchkResult* vslot_40_stackchk_out = nullptr;
};

// Process-wide cell modelling original qword @ 0x2ea4718 (points at flag object).
[[nodiscard]] void*& base_init_flag_cell_2ea4718() noexcept;

// Orchestrator. On vslot_30 failure returns false without calling mid/vslot_40/0x357f0.
// On success: hooks as above, then xchg_store_u32_357f0(*cell, 0); returns true.
// Fail-visible if cell is null on the success path (returns false after hooks).
[[nodiscard]] bool base_initialize_255130(void* self, void* arg,
                                          const BaseInitialize255130Hooks& hooks) noexcept;

}  // namespace sotc
