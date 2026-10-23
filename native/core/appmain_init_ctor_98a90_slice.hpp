// Measured Init path: prologue early + tail using lifted 0x3be050 (header+0x3be930+ensure+
// 0x80 grow+null-head list+tsc) + lifted 0x1ab550 (0x40 ctor) + debug gates 0x2552d4
// → 0xf08 singleton install → base (lifted vslot 0x534130 + mid 0x255340 +
// bounded 0x533100 header+post+miss+join+gfx+shadowmodels+attach+shadowskels+
// skels-attach+shadowanims+anims-attach+shadowbanks+banks-attach+shadowstreams+
// streams-attach+shadowrandomevents+randomevents-attach+shadowbossmusic+
// bossmusic-attach+cp3characters+cp3characters-attach+cp3shadowprecache+
// cp3shadowprecache-attach+tail-140d90+stackchk) → post-base 0x499f80 call-site
// → post-base 0x56dcb0 call-site → post-base 0x5c64c0 call-site →
// post-base 0x1290 call-site → post-base 0x56db70 / 0x5b3d00 / 0x5b3cf0
// call-sites (ret-nops) → post-base AVX BSS copy (0x1b1dd40→0x1b1dd00/0x1b1dd20)
// → post-base 0x7f7220 call-site → post-base 0x56fad0 call-site (ret-nop)
// → post-base 0xf90310 call-site (splash-hide PLT) → registry.
// Tip AllocatorFront+FakeHeap.
// PR #5 cite-only.
// NOT modelled: ensure migrate / heavy menu helpers / 0xf8f860 PLT / non-null list;
// PLT bodies 0xf901a0/0xf901c0; ba40 listener invoke; 0x533100 hit /
// success atexit; 0x499f80/0x56dcb0/0x5c64c0/0x1290 bodies; 0x7f7220 body;
// 0xf90310 PLT/Sony body; later ymm Init continue; 0x14c2f0/0x6a5370/0x6abcc0/
// 0x21620/0x2779a0/0x42390 bodies; bodies of 0x523bd0 / 0x525b40 / 0x493230 /
// 0x525080 / 0x2569d0.
#pragma once

#include "allocator_front.hpp"
#include "appmain_init_remainder_slice.hpp"
#include "base_initialize_255130.hpp"
#include "ctor_40_1ab550.hpp"
#include "ctor_98a90_3be050.hpp"
#include "init_531d60_post_base_499f80.hpp"
#include "init_531d60_post_base_56dcb0.hpp"
#include "init_531d60_post_base_5c64c0.hpp"
#include "init_531d60_post_base_1290.hpp"
#include "init_531d60_post_base_56db70.hpp"
#include "init_531d60_post_base_5b3d00.hpp"
#include "init_531d60_post_base_5b3cf0.hpp"
#include "init_531d60_post_base_avx_bss.hpp"
#include "init_531d60_post_base_7f7220.hpp"
#include "init_531d60_post_base_56fad0.hpp"
#include "init_531d60_post_base_f90310.hpp"
#include "init_531d60_post_base_ymm_bss.hpp"
#include "init_531d60_post_base_ymm_intercell.hpp"
#include "init_531d60_post_base_xmm_seed.hpp"
#include "init_531d60_post_base_ymm_reload.hpp"
#include "init_531d60_post_base_26800_f590.hpp"
#include "init_531d60_post_base_sbj.hpp"
#include "init_531d60_post_base_sboj.hpp"
#include "init_531d60_post_base_52eb50.hpp"
#include "init_531d60_post_base_lea_vmovq.hpp"
#include "init_531d60_post_base_532ca0.hpp"
#include "init_531d60_post_base_vcall18.hpp"
#include "init_531d60_post_base_532ca0_u2.hpp"
#include "init_531d60_post_base_vcall18_u2.hpp"
#include "init_531d60_post_base_532ca0_u3.hpp"
#include "init_531d60_post_base_vcall18_u3.hpp"
#include "init_531d60_post_base_532ca0_u4.hpp"
#include "init_531d60_post_base_vcall18_u4.hpp"
#include "init_531d60_post_base_532ca0_u5.hpp"
#include "init_531d60_post_base_vcall18_u5.hpp"
#include "init_531d60_post_base_532ed0.hpp"
#include "init_531d60_post_base_vcall18_532ed0.hpp"
#include "init_531d60_post_base_532ed0_u2.hpp"
#include "init_531d60_post_base_vcall18_532ed0_u2.hpp"
#include "init_531d60_post_base_17070.hpp"
#include "init_531d60_post_base_544fe0.hpp"
#include "init_531d60_post_base_cell_2eff450.hpp"
#include "init_531d60_post_base_17070_u2.hpp"
#include "init_531d60_post_base_53fd10.hpp"
#include "init_531d60_post_base_cell_2eff448.hpp"
#include "init_531d60_post_base_vcall38.hpp"
#include "init_531d60_post_base_lea_32f4fe0.hpp"
#include "init_531d60_post_base_a2caf0.hpp"
#include "init_531d60_post_base_a2ed00.hpp"
#include "init_531d60_post_base_a2ed00_u2.hpp"
#include "init_531d60_post_base_a2ed00_u3.hpp"
#include "init_531d60_post_base_a2ed00_u4.hpp"
#include "init_531d60_post_base_a2ed00_u5.hpp"
#include "init_531d60_post_base_f590_a2ed00.hpp"
#include "init_531d60_post_base_f590_a2ed00_u2.hpp"
#include "init_531d60_post_base_f590_a2ed00_u3.hpp"
#include "init_531d60_post_base_f590_a2ed00_u4.hpp"
#include "init_531d60_post_base_f590_a2ed00_u5.hpp"
#include "init_531d60_post_base_f590_a2ed00_u6.hpp"
#include "init_531d60_post_base_f590_a2ed00_u7.hpp"
#include "init_531d60_post_base_f590_a2ed00_u8.hpp"
#include "init_531d60_post_base_f590_a2ed00_u9.hpp"
#include "init_531d60_post_base_f590_a2ed00_u10.hpp"
#include "init_531d60_post_base_a2ed00_u6.hpp"
#include "init_531d60_post_base_f590_a2ed00_u11.hpp"
#include "init_531d60_post_base_f590_a2ed00_u12.hpp"
#include "init_531d60_post_base_a2ed00_u7.hpp"
#include "init_531d60_post_base_f590_a2ed00_u13.hpp"
#include "init_531d60_post_base_continue_a.hpp"
#include "init_531d60_post_base_continue_b.hpp"
#include "init_531d60_post_base_continue_c.hpp"
#include "input_table_551140.hpp"
#include "prologue_255180.hpp"
#include "singleton_f08_531d84.hpp"
#include "vslot_40_533100_header.hpp"

#include <cstdint>

namespace sotc {

// E524 decision point: the 0x532904 merge block is entered only when the
// `je 0x5328e0` outcome is unknown or not taken. A modelled taken branch stops
// before the 36 B 0x5328e0 preamble, which is NOT modelled.
[[nodiscard]] constexpr bool enters_merge_after_continue_b(
    const Init531d60PostBaseContinueBResult& b) noexcept {
    return !(b.branch_outcome_modelled && b.branch_taken);
}

struct AppmainInitCtor40SliceSide {
    bool ok = false;
    bool derived_vt = false;
    bool cell_self = false;
    bool field_0a = false;
};

struct AppmainInitCtor98a90SliceResult {
    Prologue255180Result early{};
    Prologue255180TailResult tail{};
    Ctor98a90_3be050Result ctor{};
    AppmainInitCtor40SliceSide ctor_40{};
    SingletonF08Result singleton{};
    Vslot40_533100HeaderResult vslot_40{};
    Vslot40_533100PostResult vslot_40_post{};
    Vslot40_533100MissResult vslot_40_miss{};
    Vslot40_533100JoinResult vslot_40_join{};
    Vslot40_533100GfxResult vslot_40_gfx{};
    Vslot40_533100ShadowResult vslot_40_shadow{};
    Vslot40_533100ShadowAttachResult vslot_40_shadow_attach{};
    Vslot40_533100ShadowskelsResult vslot_40_shadowskels{};
    Vslot40_533100SkelsAttachResult vslot_40_skels_attach{};
    Vslot40_533100ShadowanimsResult vslot_40_shadowanims{};
    Vslot40_533100AnimsAttachResult vslot_40_anims_attach{};
    Vslot40_533100ShadowbanksResult vslot_40_shadowbanks{};
    Vslot40_533100BanksAttachResult vslot_40_banks_attach{};
    Vslot40_533100ShadowstreamsResult vslot_40_shadowstreams{};
    Vslot40_533100StreamsAttachResult vslot_40_streams_attach{};
    Vslot40_533100ShadowrandomeventsResult vslot_40_shadowrandomevents{};
    Vslot40_533100RandomeventsAttachResult vslot_40_randomevents_attach{};
    Vslot40_533100ShadowbossmusicResult vslot_40_shadowbossmusic{};
    Vslot40_533100BossmusicAttachResult vslot_40_bossmusic_attach{};
    Vslot40_533100Cp3charactersResult vslot_40_cp3characters{};
    Vslot40_533100Cp3charactersAttachResult vslot_40_cp3characters_attach{};
    Vslot40_533100Cp3shadowprecacheResult vslot_40_cp3shadowprecache{};
    Vslot40_533100Cp3shadowprecacheAttachResult vslot_40_cp3shadowprecache_attach{};
    Vslot40_533100Tail140d90Result vslot_40_tail_140d90{};
    Vslot40_533100StackchkResult vslot_40_stackchk{};
    bool base_ok = false;
    std::uint32_t flag_after_base = 0;
    Init531d60PostBase499f80Result post_base_499f80{};
    Init531d60PostBase56dcb0Result post_base_56dcb0{};
    Init531d60PostBase5c64c0Result post_base_5c64c0{};
    Init531d60PostBase1290Result post_base_1290{};
    Init531d60PostBase56db70Result post_base_56db70{};
    Init531d60PostBase5b3d00Result post_base_5b3d00{};
    Init531d60PostBase5b3cf0Result post_base_5b3cf0{};
    Init531d60PostBaseAvxBssResult post_base_avx_bss{};
    Init531d60PostBase7f7220Result post_base_7f7220{};
    Init531d60PostBase56fad0Result post_base_56fad0{};
    Init531d60PostBaseF90310Result post_base_f90310{};
    Init531d60PostBaseYmmBssResult post_base_ymm_bss{};
    Init531d60PostBaseYmmIntercellResult post_base_ymm_intercell{};
    Init531d60PostBaseXmmSeedResult post_base_xmm_seed{};
    Init531d60PostBaseYmmReloadResult post_base_ymm_reload{};
    Init531d60PostBase26800F590Result post_base_26800_f590{};
    Init531d60PostBaseSbjResult post_base_sbj{};
    Init531d60PostBaseSbojResult post_base_sboj{};
    Init531d60PostBase52eb50Result post_base_52eb50{};
    Init531d60PostBaseLeaVmovqResult post_base_lea_vmovq{};
    Init531d60PostBase532ca0Result post_base_532ca0{};
    Init531d60PostBaseVcall18Result post_base_vcall18{};
    Init531d60PostBase532ca0U2Result post_base_532ca0_u2{};
    Init531d60PostBaseVcall18U2Result post_base_vcall18_u2{};
    Init531d60PostBase532ca0U3Result post_base_532ca0_u3{};
    Init531d60PostBaseVcall18U3Result post_base_vcall18_u3{};
    Init531d60PostBase532ca0U4Result post_base_532ca0_u4{};
    Init531d60PostBaseVcall18U4Result post_base_vcall18_u4{};
    Init531d60PostBase532ca0U5Result post_base_532ca0_u5{};
    Init531d60PostBaseVcall18U5Result post_base_vcall18_u5{};
    Init531d60PostBase532ed0Result post_base_532ed0{};
    Init531d60PostBaseVcall18532ed0Result post_base_vcall18_532ed0{};
    Init531d60PostBase532ed0U2Result post_base_532ed0_u2{};
    Init531d60PostBaseVcall18532ed0U2Result post_base_vcall18_532ed0_u2{};
    Init531d60PostBase17070Result post_base_17070{};
    Init531d60PostBase544fe0Result post_base_544fe0{};
    Init531d60PostBaseCell2eff450Result post_base_cell_2eff450{};
    Init531d60PostBase17070U2Result post_base_17070_u2{};
    Init531d60PostBase53fd10Result post_base_53fd10{};
    Init531d60PostBaseCell2eff448Result post_base_cell_2eff448{};
    Init531d60PostBaseVcall38Result post_base_vcall38{};
    Init531d60PostBaseLea32f4fe0Result post_base_lea_32f4fe0{};
    Init531d60PostBaseA2caf0Result post_base_a2caf0{};
    Init531d60PostBaseA2ed00Result post_base_a2ed00{};
    Init531d60PostBaseF590A2ed00Result post_base_f590_a2ed00{};
    Init531d60PostBaseF590A2ed00U2Result post_base_f590_a2ed00_u2{};
    Init531d60PostBaseA2ed00U2Result post_base_a2ed00_u2{};
    Init531d60PostBaseF590A2ed00U3Result post_base_f590_a2ed00_u3{};
    Init531d60PostBaseF590A2ed00U4Result post_base_f590_a2ed00_u4{};
    Init531d60PostBaseA2ed00U3Result post_base_a2ed00_u3{};
    Init531d60PostBaseF590A2ed00U5Result post_base_f590_a2ed00_u5{};
    Init531d60PostBaseF590A2ed00U6Result post_base_f590_a2ed00_u6{};
    Init531d60PostBaseA2ed00U4Result post_base_a2ed00_u4{};
    Init531d60PostBaseF590A2ed00U7Result post_base_f590_a2ed00_u7{};
    Init531d60PostBaseF590A2ed00U8Result post_base_f590_a2ed00_u8{};
    Init531d60PostBaseA2ed00U5Result post_base_a2ed00_u5{};
    Init531d60PostBaseF590A2ed00U9Result post_base_f590_a2ed00_u9{};
    Init531d60PostBaseF590A2ed00U10Result post_base_f590_a2ed00_u10{};
    Init531d60PostBaseA2ed00U6Result post_base_a2ed00_u6{};
    Init531d60PostBaseF590A2ed00U11Result post_base_f590_a2ed00_u11{};
    Init531d60PostBaseF590A2ed00U12Result post_base_f590_a2ed00_u12{};
    Init531d60PostBaseA2ed00U7Result post_base_a2ed00_u7{};
    Init531d60PostBaseF590A2ed00U13Result post_base_f590_a2ed00_u13{};
    Init531d60PostBaseContinueAResult post_base_continue_a{};
    Init531d60PostBaseContinueBResult post_base_continue_b{};
    Init531d60PostBaseContinueCResult post_base_continue_c{};
    // E524: when the 0x1e4c0 hook returns a proven boolean and the je is taken,
    // the 36 B 0x5328e0 preamble is NOT modelled, so the chain stops there
    // instead of pretending the merge block was entered through it.
    bool init_branch_5328e0_taken = false;
    bool init_stopped_at_5328e0 = false;
    AppmainInitRemainderSliceResult remainder{};
};

[[nodiscard]] AppmainInitCtor98a90SliceResult run_appmain_init_ctor_98a90_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const Ctor98a90_3be050Hooks& ctor_hooks, const BaseInitialize255130Hooks& base_hooks,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c, const SingletonF08Hooks& singleton_hooks = {},
    const Init531d60PostBase499f80Hooks& post_base_hooks = {},
    const Init531d60PostBase56dcb0Hooks& post_base_56dcb0_hooks = {},
    const Init531d60PostBase5c64c0Hooks& post_base_5c64c0_hooks = {},
    const Init531d60PostBase1290Hooks& post_base_1290_hooks = {},
    const Init531d60PostBase56db70Hooks& post_base_56db70_hooks = {},
    const Init531d60PostBase5b3d00Hooks& post_base_5b3d00_hooks = {},
    const Init531d60PostBase5b3cf0Hooks& post_base_5b3cf0_hooks = {},
    const Init531d60PostBaseAvxBssCells& post_base_avx_bss_cells = {},
    const Init531d60PostBase7f7220Hooks& post_base_7f7220_hooks = {},
    const Init531d60PostBase56fad0Hooks& post_base_56fad0_hooks = {},
    const Init531d60PostBaseF90310Hooks& post_base_f90310_hooks = {},
    const Init531d60PostBaseYmmBssCells& post_base_ymm_bss_cells = {},
    const Init531d60PostBaseYmmIntercellCells& post_base_ymm_intercell_cells = {},
    const Init531d60PostBaseXmmSeedCells& post_base_xmm_seed_cells = {},
    const Init531d60PostBaseYmmReloadCells& post_base_ymm_reload_cells = {},
    const Init531d60PostBase26800F590Hooks& post_base_26800_f590_hooks = {},
    const Init531d60PostBaseSbjHooks& post_base_sbj_hooks = {},
    const Init531d60PostBaseSbojHooks& post_base_sboj_hooks = {},
    const Init531d60PostBase52eb50Hooks& post_base_52eb50_hooks = {},
    const Init531d60PostBaseLeaVmovqCells& post_base_lea_vmovq_cells = {},
    const Init531d60PostBase532ca0Hooks& post_base_532ca0_hooks = {},
    const Init531d60PostBaseVcall18Hooks& post_base_vcall18_hooks = {},
    const Init531d60PostBase532ca0U2Hooks& post_base_532ca0_u2_hooks = {},
    const Init531d60PostBaseVcall18U2Hooks& post_base_vcall18_u2_hooks = {},
    const Init531d60PostBase532ca0U3Hooks& post_base_532ca0_u3_hooks = {},
    const Init531d60PostBaseVcall18U3Hooks& post_base_vcall18_u3_hooks = {},
    const Init531d60PostBase532ca0U4Hooks& post_base_532ca0_u4_hooks = {},
    const Init531d60PostBaseVcall18U4Hooks& post_base_vcall18_u4_hooks = {},
    const Init531d60PostBase532ca0U5Hooks& post_base_532ca0_u5_hooks = {},
    const Init531d60PostBaseVcall18U5Hooks& post_base_vcall18_u5_hooks = {},
    const Init531d60PostBase532ed0Hooks& post_base_532ed0_hooks = {},
    const Init531d60PostBaseVcall18532ed0Hooks& post_base_vcall18_532ed0_hooks = {},
    const Init531d60PostBase532ed0U2Hooks& post_base_532ed0_u2_hooks = {},
    const Init531d60PostBaseVcall18532ed0U2Hooks& post_base_vcall18_532ed0_u2_hooks = {},
    const Init531d60PostBase17070Hooks& post_base_17070_hooks = {},
    const Init531d60PostBase544fe0Hooks& post_base_544fe0_hooks = {},
    const Init531d60PostBaseCell2eff450Cells& post_base_cell_2eff450_cells = {},
    const Init531d60PostBaseCell2eff450Hooks& post_base_cell_2eff450_hooks = {},
    const Init531d60PostBase17070U2Hooks& post_base_17070_u2_hooks = {},
    const Init531d60PostBase53fd10Hooks& post_base_53fd10_hooks = {},
    const Init531d60PostBaseCell2eff448Cells& post_base_cell_2eff448_cells = {},
    const Init531d60PostBaseCell2eff448Hooks& post_base_cell_2eff448_hooks = {},
    const Init531d60PostBaseVcall38Hooks& post_base_vcall38_hooks = {},
    const Init531d60PostBaseLea32f4fe0Cells& post_base_lea_32f4fe0_cells = {},
    const Init531d60PostBaseA2caf0Hooks& post_base_a2caf0_hooks = {},
    const Init531d60PostBaseA2ed00Hooks& post_base_a2ed00_hooks = {},
    const Init531d60PostBaseF590A2ed00Hooks& post_base_f590_a2ed00_hooks = {},
    const Init531d60PostBaseF590A2ed00U2Hooks& post_base_f590_a2ed00_u2_hooks = {},
    const Init531d60PostBaseA2ed00U2Hooks& post_base_a2ed00_u2_hooks = {},
    const Init531d60PostBaseF590A2ed00U3Hooks& post_base_f590_a2ed00_u3_hooks = {},
    const Init531d60PostBaseF590A2ed00U4Hooks& post_base_f590_a2ed00_u4_hooks = {},
    const Init531d60PostBaseA2ed00U3Hooks& post_base_a2ed00_u3_hooks = {},
    const Init531d60PostBaseF590A2ed00U5Hooks& post_base_f590_a2ed00_u5_hooks = {},
    const Init531d60PostBaseF590A2ed00U6Hooks& post_base_f590_a2ed00_u6_hooks = {},
    const Init531d60PostBaseA2ed00U4Hooks& post_base_a2ed00_u4_hooks = {},
    const Init531d60PostBaseF590A2ed00U7Hooks& post_base_f590_a2ed00_u7_hooks = {},
    const Init531d60PostBaseF590A2ed00U8Hooks& post_base_f590_a2ed00_u8_hooks = {},
    const Init531d60PostBaseA2ed00U5Hooks& post_base_a2ed00_u5_hooks = {},
    const Init531d60PostBaseF590A2ed00U9Hooks& post_base_f590_a2ed00_u9_hooks = {},
    const Init531d60PostBaseF590A2ed00U10Hooks& post_base_f590_a2ed00_u10_hooks = {},
    const Init531d60PostBaseA2ed00U6Hooks& post_base_a2ed00_u6_hooks = {},
    const Init531d60PostBaseF590A2ed00U11Hooks& post_base_f590_a2ed00_u11_hooks = {},
    const Init531d60PostBaseF590A2ed00U12Hooks& post_base_f590_a2ed00_u12_hooks = {},
    const Init531d60PostBaseA2ed00U7Hooks& post_base_a2ed00_u7_hooks = {},
    const Init531d60PostBaseF590A2ed00U13Hooks& post_base_f590_a2ed00_u13_hooks = {},
    const Init531d60PostBaseContinueAHooks& post_base_continue_a_hooks = {},
    const Init531d60PostBaseContinueBHooks& post_base_continue_b_hooks = {},
    const Init531d60PostBaseContinueCHooks& post_base_continue_c_hooks = {});

}  // namespace sotc
