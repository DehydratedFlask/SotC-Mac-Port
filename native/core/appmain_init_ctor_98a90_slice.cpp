#include "appmain_init_ctor_98a90_slice.hpp"

#include <cstring>

namespace sotc {

AppmainInitCtor98a90SliceResult run_appmain_init_ctor_98a90_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const Ctor98a90_3be050Hooks& ctor_hooks, const BaseInitialize255130Hooks& base_hooks,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c, const SingletonF08Hooks& singleton_hooks,
    const Init531d60PostBase499f80Hooks& post_base_hooks,
    const Init531d60PostBase56dcb0Hooks& post_base_56dcb0_hooks,
    const Init531d60PostBase5c64c0Hooks& post_base_5c64c0_hooks,
    const Init531d60PostBase1290Hooks& post_base_1290_hooks,
    const Init531d60PostBase56db70Hooks& post_base_56db70_hooks,
    const Init531d60PostBase5b3d00Hooks& post_base_5b3d00_hooks,
    const Init531d60PostBase5b3cf0Hooks& post_base_5b3cf0_hooks,
    const Init531d60PostBaseAvxBssCells& post_base_avx_bss_cells,
    const Init531d60PostBase7f7220Hooks& post_base_7f7220_hooks,
    const Init531d60PostBase56fad0Hooks& post_base_56fad0_hooks,
    const Init531d60PostBaseF90310Hooks& post_base_f90310_hooks,
    const Init531d60PostBaseYmmBssCells& post_base_ymm_bss_cells,
    const Init531d60PostBaseYmmIntercellCells& post_base_ymm_intercell_cells,
    const Init531d60PostBaseXmmSeedCells& post_base_xmm_seed_cells,
    const Init531d60PostBaseYmmReloadCells& post_base_ymm_reload_cells,
    const Init531d60PostBase26800F590Hooks& post_base_26800_f590_hooks,
    const Init531d60PostBaseSbjHooks& post_base_sbj_hooks,
    const Init531d60PostBaseSbojHooks& post_base_sboj_hooks,
    const Init531d60PostBase52eb50Hooks& post_base_52eb50_hooks,
    const Init531d60PostBaseLeaVmovqCells& post_base_lea_vmovq_cells,
    const Init531d60PostBase532ca0Hooks& post_base_532ca0_hooks,
    const Init531d60PostBaseVcall18Hooks& post_base_vcall18_hooks,
    const Init531d60PostBase532ca0U2Hooks& post_base_532ca0_u2_hooks,
    const Init531d60PostBaseVcall18U2Hooks& post_base_vcall18_u2_hooks,
    const Init531d60PostBase532ca0U3Hooks& post_base_532ca0_u3_hooks,
    const Init531d60PostBaseVcall18U3Hooks& post_base_vcall18_u3_hooks,
    const Init531d60PostBase532ca0U4Hooks& post_base_532ca0_u4_hooks,
    const Init531d60PostBaseVcall18U4Hooks& post_base_vcall18_u4_hooks,
    const Init531d60PostBase532ca0U5Hooks& post_base_532ca0_u5_hooks,
    const Init531d60PostBaseVcall18U5Hooks& post_base_vcall18_u5_hooks,
    const Init531d60PostBase532ed0Hooks& post_base_532ed0_hooks,
    const Init531d60PostBaseVcall18532ed0Hooks& post_base_vcall18_532ed0_hooks,
    const Init531d60PostBase532ed0U2Hooks& post_base_532ed0_u2_hooks,
    const Init531d60PostBaseVcall18532ed0U2Hooks& post_base_vcall18_532ed0_u2_hooks,
    const Init531d60PostBase17070Hooks& post_base_17070_hooks,
    const Init531d60PostBase544fe0Hooks& post_base_544fe0_hooks,
    const Init531d60PostBaseCell2eff450Cells& post_base_cell_2eff450_cells,
    const Init531d60PostBaseCell2eff450Hooks& post_base_cell_2eff450_hooks,
    const Init531d60PostBase17070U2Hooks& post_base_17070_u2_hooks,
    const Init531d60PostBase53fd10Hooks& post_base_53fd10_hooks,
    const Init531d60PostBaseCell2eff448Cells& post_base_cell_2eff448_cells,
    const Init531d60PostBaseCell2eff448Hooks& post_base_cell_2eff448_hooks,
    const Init531d60PostBaseVcall38Hooks& post_base_vcall38_hooks,
    const Init531d60PostBaseLea32f4fe0Cells& post_base_lea_32f4fe0_cells,
    const Init531d60PostBaseA2caf0Hooks& post_base_a2caf0_hooks,
    const Init531d60PostBaseA2ed00Hooks& post_base_a2ed00_hooks,
    const Init531d60PostBaseF590A2ed00Hooks& post_base_f590_a2ed00_hooks,
    const Init531d60PostBaseF590A2ed00U2Hooks& post_base_f590_a2ed00_u2_hooks,
    const Init531d60PostBaseA2ed00U2Hooks& post_base_a2ed00_u2_hooks,
    const Init531d60PostBaseF590A2ed00U3Hooks& post_base_f590_a2ed00_u3_hooks,
    const Init531d60PostBaseF590A2ed00U4Hooks& post_base_f590_a2ed00_u4_hooks,
    const Init531d60PostBaseA2ed00U3Hooks& post_base_a2ed00_u3_hooks,
    const Init531d60PostBaseF590A2ed00U5Hooks& post_base_f590_a2ed00_u5_hooks,
    const Init531d60PostBaseF590A2ed00U6Hooks& post_base_f590_a2ed00_u6_hooks,
    const Init531d60PostBaseA2ed00U4Hooks& post_base_a2ed00_u4_hooks,
    const Init531d60PostBaseF590A2ed00U7Hooks& post_base_f590_a2ed00_u7_hooks,
    const Init531d60PostBaseF590A2ed00U8Hooks& post_base_f590_a2ed00_u8_hooks,
    const Init531d60PostBaseA2ed00U5Hooks& post_base_a2ed00_u5_hooks,
    const Init531d60PostBaseF590A2ed00U9Hooks& post_base_f590_a2ed00_u9_hooks,
    const Init531d60PostBaseF590A2ed00U10Hooks& post_base_f590_a2ed00_u10_hooks,
    const Init531d60PostBaseA2ed00U6Hooks& post_base_a2ed00_u6_hooks,
    const Init531d60PostBaseF590A2ed00U11Hooks& post_base_f590_a2ed00_u11_hooks,
    const Init531d60PostBaseF590A2ed00U12Hooks& post_base_f590_a2ed00_u12_hooks,
    const Init531d60PostBaseA2ed00U7Hooks& post_base_a2ed00_u7_hooks,
    const Init531d60PostBaseF590A2ed00U13Hooks& post_base_f590_a2ed00_u13_hooks,
    const Init531d60PostBaseF590A2ed00U14Hooks& post_base_f590_a2ed00_u14_hooks,
    const Init531d60PostBaseA2ea00U1Hooks& post_base_a2ea00_u1_hooks,
    const Init531d60PostBaseA2e5d0U1Hooks& post_base_a2e5d0_u1_hooks,
    const Init531d60PostBaseA543b0U1Hooks& post_base_a543b0_u1_hooks,
    const Init531d60PostBase1fc80U1Hooks& post_base_1fc80_u1_hooks,
    const Init531d60PostBase1e4c0U1Hooks& post_base_1e4c0_u1_hooks,
    const Init531d60PostBase4c4950U1Hooks& post_base_4c4950_u1_hooks,
    const Init531d60PostBaseA52b70U1Hooks& post_base_a52b70_u1_hooks,
    const Init531d60PostBaseA599e0U1Hooks& post_base_a599e0_u1_hooks,
    const Init531d60PostBaseA431e0U1Hooks& post_base_a431e0_u1_hooks,
    const Init531d60PostBaseContinueAHooks& post_base_continue_a_hooks,
    const Init531d60PostBaseContinueBHooks& post_base_continue_b_hooks,
    const Init531d60PostBaseContinueCHooks& post_base_continue_c_hooks,
    const Init531d60PostBaseContinueDHooks& post_base_continue_d_hooks,
    const Init531d60PostBaseContinueEHooks& post_base_continue_e_hooks,
    const Init531d60PostBaseRegistryTailHooks& post_base_registry_tail_hooks,
    const Init531d60PostBaseRegistryBodyHooks& post_base_registry_body_hooks,
    const Init531d60PostBaseAlt5327bdHooks& post_base_alt_5327bd_hooks,
    const Init1290Hooks& init_1290_hooks) {
    AppmainInitCtor98a90SliceResult out{};
    out.early = run_prologue_255180_early(alloc, early_hooks);
    if (!out.early.ok) {
        return out;
    }

    // Tail with lifted 0x3be050 + lifted 0x1ab550 (overrides null).
    Prologue255180TailHooks th = tail_hooks;
    th.ctor_98a90_3be050 = nullptr;
    th.use_lifted_ctor_98a90 = true;
    th.ctor_98a90_detail = ctor_hooks;
    th.ctor_40_1ab550 = nullptr;  // use init_ctor_40_1ab550
    out.tail = run_prologue_255180_tail(alloc, self, th);
    if (!out.tail.ok) {
        return out;
    }
    // Re-read ctor side effects from installed object + list cells.
    if (out.tail.obj_98a90 != nullptr) {
        auto* o = static_cast<std::uint8_t*>(out.tail.obj_98a90);
        std::memcpy(&out.ctor.nested_48, o + 8, sizeof(void*));
        std::memcpy(&out.ctor.buf_80, o + kCtor98a90VecBegin, sizeof(void*));
        out.ctor.list_self_stored = (ctor_98a90_self_cell_2ef63f8() == out.tail.obj_98a90);
        out.ctor.list_head_seen = ctor_98a90_list_head_cell_2ef6400();
        out.ctor.list_walk_skipped_null = (out.ctor.list_head_seen == nullptr);
        std::memcpy(&out.ctor.tsc_value, o + kCtor98a90TscOffset, 8);
        out.ctor.tsc_stored = (out.ctor.tsc_value != 0);
        std::memcpy(&out.ctor.ensure_buf, o + 0x28, sizeof(void*));  // ensure begin
        out.ctor.ensure_allocated = (out.ctor.ensure_buf != nullptr);
        out.ctor.ensure_called = out.ctor.ensure_allocated;
        {
            void* vt = nullptr;
            std::memcpy(&vt, o + kInitSub3be930OffInObj + 0x18, sizeof(void*));
            out.ctor.sub_3be930_ok = (vt == slot_vtable_2dd9f78());
        }
        out.ctor.ok = (out.ctor.nested_48 != nullptr) && (out.ctor.buf_80 != nullptr) &&
                      out.ctor.list_self_stored && out.ctor.tsc_stored &&
                      out.ctor.ensure_allocated && out.ctor.sub_3be930_ok;
        out.ctor.buf_grown = (out.ctor.buf_80 != nullptr);
    }
    if (out.tail.obj_40 != nullptr) {
        auto* s = static_cast<std::uint8_t*>(out.tail.obj_40);
        void* vt = nullptr;
        std::memcpy(&vt, s, sizeof(void*));
        out.ctor_40.derived_vt = (vt == ctor40_derived_vtable_1598ae0());
        out.ctor_40.cell_self = (ctor40_cell_2ee6778() == out.tail.obj_40);
        out.ctor_40.field_0a = (s[0x0a] == 1);
        out.ctor_40.ok =
            out.ctor_40.derived_vt && out.ctor_40.cell_self && out.ctor_40.field_0a;
    }

    // E420: Init 0xf08 singleton install before base Initialize (OBS 0x531d84).
    out.singleton = install_singleton_f08_531d84(alloc, singleton_hooks);
    if (!out.singleton.ok) {
        return out;
    }

    BaseInitialize255130Hooks hooks = base_hooks;
    hooks.prologue_255180 = nullptr;
    // E418/E419/E421: force lifted 0x534130 + 0x255340 + 0x533100 header.
    hooks.vslot_30 = nullptr;
    hooks.use_lifted_vslot_30 = true;
    hooks.mid_255340 = nullptr;
    hooks.use_lifted_mid_255340 = true;
    hooks.vslot_40 = nullptr;
    hooks.use_lifted_vslot_40_header = true;
    hooks.vslot_40_alloc = &alloc;
    hooks.vslot_40_out = &out.vslot_40;
    hooks.vslot_40_post_out = &out.vslot_40_post;
    hooks.vslot_40_miss_out = &out.vslot_40_miss;
    hooks.vslot_40_join_out = &out.vslot_40_join;
    hooks.vslot_40_gfx_out = &out.vslot_40_gfx;
    hooks.vslot_40_shadow_out = &out.vslot_40_shadow;
    hooks.vslot_40_shadow_attach_out = &out.vslot_40_shadow_attach;
    hooks.vslot_40_shadowskels_out = &out.vslot_40_shadowskels;
    hooks.vslot_40_skels_attach_out = &out.vslot_40_skels_attach;
    hooks.vslot_40_shadowanims_out = &out.vslot_40_shadowanims;
    hooks.vslot_40_anims_attach_out = &out.vslot_40_anims_attach;
    hooks.vslot_40_shadowbanks_out = &out.vslot_40_shadowbanks;
    hooks.vslot_40_banks_attach_out = &out.vslot_40_banks_attach;
    hooks.vslot_40_shadowstreams_out = &out.vslot_40_shadowstreams;
    hooks.vslot_40_streams_attach_out = &out.vslot_40_streams_attach;
    hooks.vslot_40_shadowrandomevents_out = &out.vslot_40_shadowrandomevents;
    hooks.vslot_40_randomevents_attach_out = &out.vslot_40_randomevents_attach;
    hooks.vslot_40_shadowbossmusic_out = &out.vslot_40_shadowbossmusic;
    hooks.vslot_40_bossmusic_attach_out = &out.vslot_40_bossmusic_attach;
    hooks.vslot_40_cp3characters_out = &out.vslot_40_cp3characters;
    hooks.vslot_40_cp3characters_attach_out = &out.vslot_40_cp3characters_attach;
    hooks.vslot_40_cp3shadowprecache_out = &out.vslot_40_cp3shadowprecache;
    hooks.vslot_40_cp3shadowprecache_attach_out = &out.vslot_40_cp3shadowprecache_attach;
    hooks.vslot_40_tail_140d90_out = &out.vslot_40_tail_140d90;
    hooks.vslot_40_stackchk_out = &out.vslot_40_stackchk;
    out.base_ok = base_initialize_255130(self, arg, hooks);
    if (out.early.flag_obj != nullptr) {
        std::memcpy(&out.flag_after_base,
                    static_cast<std::uint8_t*>(out.early.flag_obj) + kXchgStore357f0Offset, 4);
    }
    if (!out.base_ok) {
        return out;
    }
    // E446: always-on Init continue after Base Initialize — 0x499f80(self+0x3c70) call-site.
    out.post_base_499f80 = init_531d60_post_base_499f80(self, post_base_hooks);
    if (!out.post_base_499f80.ok) {
        return out;
    }
    // E447: always-on Init continue — 0x56dcb0 call-site; stop before 0x56dca0.
    out.post_base_56dcb0 = init_531d60_post_base_56dcb0(post_base_56dcb0_hooks);
    if (!out.post_base_56dcb0.ok) {
        return out;
    }
    // E448: always-on Init continue — 0x5c64c0 call-site; stop before 0x1290.
    // (0x56dca0 intervening ret-nop remains E406 remainder.)
    out.post_base_5c64c0 = init_531d60_post_base_5c64c0(post_base_5c64c0_hooks);
    if (!out.post_base_5c64c0.ok) {
        return out;
    }
    // E449: always-on Init continue — 0x1290 call-site; stop before 0x56db70.
    // E544 follow-up: if the caller supplies a real 0x1290 configuration (containers and
    // allocator), execute the lifted body so its A/B inserts actually run in the chain. The
    // call-site stub stays as the fallback when no container is provided.
    if (init_1290_hooks.container_a != nullptr || init_1290_hooks.container_b != nullptr) {
        out.init_1290 = init_1290(self, init_1290_hooks);
        out.init_1290_native = out.init_1290.ok;
        if (!out.init_1290.ok) {
            return out;
        }
    } else {
        out.post_base_1290 = init_531d60_post_base_1290(post_base_1290_hooks);
        if (!out.post_base_1290.ok) {
            return out;
        }
    }
    // E450: always-on Init continue — 0x56db70 call-site (ret-nop); stop before 0x5b3d00.
    out.post_base_56db70 = init_531d60_post_base_56db70(post_base_56db70_hooks);
    if (!out.post_base_56db70.ok) {
        return out;
    }
    // E451: always-on Init continue — 0x5b3d00 call-site (ret-nop); stop before 0x5b3cf0.
    out.post_base_5b3d00 = init_531d60_post_base_5b3d00(post_base_5b3d00_hooks);
    if (!out.post_base_5b3d00.ok) {
        return out;
    }
    // E452: always-on Init continue — 0x5b3cf0 call-site (ret-nop); stop before AVX BSS copy.
    out.post_base_5b3cf0 = init_531d60_post_base_5b3cf0(post_base_5b3cf0_hooks);
    if (!out.post_base_5b3cf0.ok) {
        return out;
    }
    // E453: always-on Init continue — AVX BSS xmm copy; stop before 0x7f7220.
    out.post_base_avx_bss = init_531d60_post_base_avx_bss(post_base_avx_bss_cells);
    if (!out.post_base_avx_bss.ok) {
        return out;
    }
    // E454: always-on Init continue — 0x7f7220 call-site; stop before 0x56fad0.
    out.post_base_7f7220 = init_531d60_post_base_7f7220(post_base_7f7220_hooks);
    if (!out.post_base_7f7220.ok) {
        return out;
    }
    // E455: always-on Init continue — 0x56fad0 call-site (ret-nop); stop before 0xf90310.
    out.post_base_56fad0 = init_531d60_post_base_56fad0(post_base_56fad0_hooks);
    if (!out.post_base_56fad0.ok) {
        return out;
    }
    // E456: always-on Init continue — 0xf90310 call-site (splash-hide PLT); stop before ymm.
    out.post_base_f90310 = init_531d60_post_base_f90310(post_base_f90310_hooks);
    if (!out.post_base_f90310.ok) {
        return out;
    }
    // E457: always-on Init continue — ymm BSS seed; stop before inter-cell ymm / 0x26800.
    out.post_base_ymm_bss = init_531d60_post_base_ymm_bss(post_base_ymm_bss_cells);
    if (!out.post_base_ymm_bss.ok) {
        return out;
    }
    // E458: always-on Init continue — ymm inter-cell copy; stop before xmm / 0x26800.
    out.post_base_ymm_intercell =
        init_531d60_post_base_ymm_intercell(post_base_ymm_intercell_cells);
    if (!out.post_base_ymm_intercell.ok) {
        return out;
    }
    // E459: always-on Init continue — xmm BSS seed; stop before ymm reload / 0x26800.
    out.post_base_xmm_seed = init_531d60_post_base_xmm_seed(post_base_xmm_seed_cells);
    if (!out.post_base_xmm_seed.ok) {
        return out;
    }
    // E460: always-on Init continue — ymm reload→0x160433c; stop before 0x26800.
    out.post_base_ymm_reload = init_531d60_post_base_ymm_reload(post_base_ymm_reload_cells);
    if (!out.post_base_ymm_reload.ok) {
        return out;
    }
    // E461: always-on Init continue — 0x26800/0xf590 call-sites; stop before next string.
    out.post_base_26800_f590 = init_531d60_post_base_26800_f590(post_base_26800_f590_hooks);
    if (!out.post_base_26800_f590.ok) {
        return out;
    }
    // E462: always-on Init continue — Shadow Boss Jump call-sites; stop before Outro.
    out.post_base_sbj = init_531d60_post_base_sbj(post_base_sbj_hooks);
    if (!out.post_base_sbj.ok) {
        return out;
    }
    // E463: always-on Init continue — Outro Jump call-sites; stop before 0x52eb50.
    out.post_base_sboj = init_531d60_post_base_sboj(post_base_sboj_hooks);
    if (!out.post_base_sboj.ok) {
        return out;
    }
    // E464: always-on Init continue — 0x52eb50 call-site; stop before lea rbx.
    out.post_base_52eb50 = init_531d60_post_base_52eb50(post_base_52eb50_hooks);
    if (!out.post_base_52eb50.ok) {
        return out;
    }
    // E465: always-on Init continue — lea rbx / vmovq; stop before stack prep.
    out.post_base_lea_vmovq = init_531d60_post_base_lea_vmovq(post_base_lea_vmovq_cells);
    if (!out.post_base_lea_vmovq.ok) {
        return out;
    }
    // E466: always-on Init continue — 0x532ca0 call-site; stack prep NOT modelled.
    out.post_base_532ca0 = init_531d60_post_base_532ca0(post_base_532ca0_hooks);
    if (!out.post_base_532ca0.ok) {
        return out;
    }
    // E467: always-on Init continue — virtual call [rax+0x18]; callee NOT modelled.
    out.post_base_vcall18 = init_531d60_post_base_vcall18(post_base_vcall18_hooks);
    if (!out.post_base_vcall18.ok) {
        return out;
    }
    // E468: always-on Init continue — second 0x532ca0 call-site; stack prep NOT modelled.
    out.post_base_532ca0_u2 = init_531d60_post_base_532ca0_u2(post_base_532ca0_u2_hooks);
    if (!out.post_base_532ca0_u2.ok) {
        return out;
    }
    // E469: always-on Init continue — second virtual call [rax+0x18]; callee NOT modelled.
    out.post_base_vcall18_u2 = init_531d60_post_base_vcall18_u2(post_base_vcall18_u2_hooks);
    if (!out.post_base_vcall18_u2.ok) {
        return out;
    }
    // E470: always-on Init continue — third 0x532ca0 call-site; stack prep NOT modelled.
    out.post_base_532ca0_u3 = init_531d60_post_base_532ca0_u3(post_base_532ca0_u3_hooks);
    if (!out.post_base_532ca0_u3.ok) {
        return out;
    }
    // E471: always-on Init continue — third virtual call [rax+0x18]; callee NOT modelled.
    out.post_base_vcall18_u3 = init_531d60_post_base_vcall18_u3(post_base_vcall18_u3_hooks);
    if (!out.post_base_vcall18_u3.ok) {
        return out;
    }
    // E472: always-on Init continue — fourth 0x532ca0 call-site; stack prep NOT modelled.
    out.post_base_532ca0_u4 = init_531d60_post_base_532ca0_u4(post_base_532ca0_u4_hooks);
    if (!out.post_base_532ca0_u4.ok) {
        return out;
    }
    // E473: always-on Init continue — fourth virtual call [rax+0x18]; callee NOT modelled.
    out.post_base_vcall18_u4 = init_531d60_post_base_vcall18_u4(post_base_vcall18_u4_hooks);
    if (!out.post_base_vcall18_u4.ok) {
        return out;
    }
    // E474: always-on Init continue — fifth 0x532ca0 call-site; stack prep NOT modelled.
    out.post_base_532ca0_u5 = init_531d60_post_base_532ca0_u5(post_base_532ca0_u5_hooks);
    if (!out.post_base_532ca0_u5.ok) {
        return out;
    }
    // E475: always-on Init continue — fifth virtual call [rax+0x18]; callee NOT modelled.
    out.post_base_vcall18_u5 = init_531d60_post_base_vcall18_u5(post_base_vcall18_u5_hooks);
    if (!out.post_base_vcall18_u5.ok) {
        return out;
    }
    // E476: always-on Init continue — first 0x532ed0 call-site; stack prep NOT modelled.
    out.post_base_532ed0 = init_531d60_post_base_532ed0(post_base_532ed0_hooks);
    if (!out.post_base_532ed0.ok) {
        return out;
    }
    // E477: always-on Init continue — first 0x532ed0-series virtual call [rax+0x18].
    out.post_base_vcall18_532ed0 =
        init_531d60_post_base_vcall18_532ed0(post_base_vcall18_532ed0_hooks);
    if (!out.post_base_vcall18_532ed0.ok) {
        return out;
    }
    // E478: always-on Init continue — second 0x532ed0 call-site; stack prep NOT modelled.
    out.post_base_532ed0_u2 = init_531d60_post_base_532ed0_u2(post_base_532ed0_u2_hooks);
    if (!out.post_base_532ed0_u2.ok) {
        return out;
    }
    // E479: always-on Init continue — second 0x532ed0-series virtual call [rax+0x18].
    out.post_base_vcall18_532ed0_u2 =
        init_531d60_post_base_vcall18_532ed0_u2(post_base_vcall18_532ed0_u2_hooks);
    if (!out.post_base_vcall18_532ed0_u2.ok) {
        return out;
    }
    // E480: always-on Init continue — first 0x17070 call-site; arg prep NOT modelled.
    out.post_base_17070 = init_531d60_post_base_17070(post_base_17070_hooks);
    if (!out.post_base_17070.ok) {
        return out;
    }
    // E481: always-on Init continue — first 0x544fe0 call-site; mov-rbx prep NOT modelled.
    out.post_base_544fe0 = init_531d60_post_base_544fe0(post_base_544fe0_hooks);
    if (!out.post_base_544fe0.ok) {
        return out;
    }
    // E482: always-on Init continue — lea r14 / cell-install @ 0x2eff450.
    out.post_base_cell_2eff450 = init_531d60_post_base_cell_2eff450(
        post_base_cell_2eff450_cells, post_base_cell_2eff450_hooks);
    if (!out.post_base_cell_2eff450.ok) {
        return out;
    }
    // E483: always-on Init continue — second 0x17070 call-site; arg prep NOT modelled.
    out.post_base_17070_u2 = init_531d60_post_base_17070_u2(post_base_17070_u2_hooks);
    if (!out.post_base_17070_u2.ok) {
        return out;
    }
    // E484: always-on Init continue — first 0x53fd10 call-site; mov-rbx prep NOT modelled.
    out.post_base_53fd10 = init_531d60_post_base_53fd10(post_base_53fd10_hooks);
    if (!out.post_base_53fd10.ok) {
        return out;
    }
    // E485: always-on Init continue — lea r14 / cell-install @ 0x2eff448.
    out.post_base_cell_2eff448 = init_531d60_post_base_cell_2eff448(
        post_base_cell_2eff448_cells, post_base_cell_2eff448_hooks);
    if (!out.post_base_cell_2eff448.ok) {
        return out;
    }
    // E486: always-on Init continue — virtual call [rax+0x38]; callee NOT modelled.
    out.post_base_vcall38 = init_531d60_post_base_vcall38(post_base_vcall38_hooks);
    if (!out.post_base_vcall38.ok) {
        return out;
    }
    // E487: always-on Init continue — lea r13 pin @ 0x32f4fe0.
    out.post_base_lea_32f4fe0 = init_531d60_post_base_lea_32f4fe0(post_base_lea_32f4fe0_cells);
    if (!out.post_base_lea_32f4fe0.ok) {
        return out;
    }
    // E488: always-on Init continue — first 0xa2caf0 call-site; mov-rdi prep NOT modelled.
    out.post_base_a2caf0 = init_531d60_post_base_a2caf0(post_base_a2caf0_hooks);
    if (!out.post_base_a2caf0.ok) {
        return out;
    }
    // E489: always-on Init continue — first 0xa2ed00 call-site; stack prep NOT modelled.
    out.post_base_a2ed00 = init_531d60_post_base_a2ed00(post_base_a2ed00_hooks);
    if (!out.post_base_a2ed00.ok) {
        return out;
    }
    // E490: always-on Init continue — first post-a2ed00 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00 = init_531d60_post_base_f590_a2ed00(post_base_f590_a2ed00_hooks);
    if (!out.post_base_f590_a2ed00.ok) {
        return out;
    }
    // E491: always-on Init continue — second post-a2ed00 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u2 = init_531d60_post_base_f590_a2ed00_u2(
        post_base_f590_a2ed00_u2_hooks);
    if (!out.post_base_f590_a2ed00_u2.ok) {
        return out;
    }
    // E492: always-on Init continue — second 0xa2ed00 call-site; stack prep NOT modelled.
    out.post_base_a2ed00_u2 = init_531d60_post_base_a2ed00_u2(post_base_a2ed00_u2_hooks);
    if (!out.post_base_a2ed00_u2.ok) {
        return out;
    }
    // E493: always-on Init continue — first post-a2ed00-u2 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u3 = init_531d60_post_base_f590_a2ed00_u3(
        post_base_f590_a2ed00_u3_hooks);
    if (!out.post_base_f590_a2ed00_u3.ok) {
        return out;
    }
    // E494: always-on Init continue — second post-a2ed00-u2 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u4 = init_531d60_post_base_f590_a2ed00_u4(
        post_base_f590_a2ed00_u4_hooks);
    if (!out.post_base_f590_a2ed00_u4.ok) {
        return out;
    }
    // E495: always-on Init continue — third 0xa2ed00 call-site; stack prep NOT modelled.
    out.post_base_a2ed00_u3 = init_531d60_post_base_a2ed00_u3(post_base_a2ed00_u3_hooks);
    if (!out.post_base_a2ed00_u3.ok) {
        return out;
    }
    // E496: always-on Init continue — first post-a2ed00-u3 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u5 = init_531d60_post_base_f590_a2ed00_u5(
        post_base_f590_a2ed00_u5_hooks);
    if (!out.post_base_f590_a2ed00_u5.ok) {
        return out;
    }
    // E497: always-on Init continue — second post-a2ed00-u3 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u6 = init_531d60_post_base_f590_a2ed00_u6(
        post_base_f590_a2ed00_u6_hooks);
    if (!out.post_base_f590_a2ed00_u6.ok) {
        return out;
    }
    // E498: always-on Init continue — fourth 0xa2ed00 call-site; stack prep NOT modelled.
    out.post_base_a2ed00_u4 = init_531d60_post_base_a2ed00_u4(post_base_a2ed00_u4_hooks);
    if (!out.post_base_a2ed00_u4.ok) {
        return out;
    }
    // E499: always-on Init continue — first post-a2ed00-u4 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u7 = init_531d60_post_base_f590_a2ed00_u7(
        post_base_f590_a2ed00_u7_hooks);
    if (!out.post_base_f590_a2ed00_u7.ok) {
        return out;
    }
    // E500: always-on Init continue — second post-a2ed00-u4 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u8 = init_531d60_post_base_f590_a2ed00_u8(
        post_base_f590_a2ed00_u8_hooks);
    if (!out.post_base_f590_a2ed00_u8.ok) {
        return out;
    }
    // E501: always-on Init continue — fifth 0xa2ed00 call-site; stack prep NOT modelled.
    out.post_base_a2ed00_u5 = init_531d60_post_base_a2ed00_u5(post_base_a2ed00_u5_hooks);
    if (!out.post_base_a2ed00_u5.ok) {
        return out;
    }
    // E502: always-on Init continue — first post-a2ed00-u5 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u9 = init_531d60_post_base_f590_a2ed00_u9(
        post_base_f590_a2ed00_u9_hooks);
    if (!out.post_base_f590_a2ed00_u9.ok) {
        return out;
    }
    // E503: always-on Init continue — second post-a2ed00-u5 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u10 = init_531d60_post_base_f590_a2ed00_u10(
        post_base_f590_a2ed00_u10_hooks);
    if (!out.post_base_f590_a2ed00_u10.ok) {
        return out;
    }
    // E504: always-on Init continue — sixth 0xa2ed00 call-site; stack prep NOT modelled.
    out.post_base_a2ed00_u6 = init_531d60_post_base_a2ed00_u6(post_base_a2ed00_u6_hooks);
    if (!out.post_base_a2ed00_u6.ok) {
        return out;
    }
    // E505: always-on Init continue — first post-a2ed00-u6 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u11 = init_531d60_post_base_f590_a2ed00_u11(
        post_base_f590_a2ed00_u11_hooks);
    if (!out.post_base_f590_a2ed00_u11.ok) {
        return out;
    }
    // E506: always-on Init continue — second post-a2ed00-u6 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u12 = init_531d60_post_base_f590_a2ed00_u12(
        post_base_f590_a2ed00_u12_hooks);
    if (!out.post_base_f590_a2ed00_u12.ok) {
        return out;
    }
    // E507: always-on Init continue — seventh 0xa2ed00 call-site; stack prep NOT modelled.
    out.post_base_a2ed00_u7 = init_531d60_post_base_a2ed00_u7(post_base_a2ed00_u7_hooks);
    if (!out.post_base_a2ed00_u7.ok) {
        return out;
    }
    // E508: always-on Init continue — first post-a2ed00-u7 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u13 = init_531d60_post_base_f590_a2ed00_u13(
        post_base_f590_a2ed00_u13_hooks);
    if (!out.post_base_f590_a2ed00_u13.ok) {
        return out;
    }
    // E509: always-on Init continue — second post-a2ed00-u7 0xf590 call-site; mov-rdi prep NOT modelled.
    out.post_base_f590_a2ed00_u14 = init_531d60_post_base_f590_a2ed00_u14(
        post_base_f590_a2ed00_u14_hooks);
    if (!out.post_base_f590_a2ed00_u14.ok) {
        return out;
    }
    // E510: always-on Init continue — first 0xa2ea00 call-site; 9 B prep NOT modelled.
    out.post_base_a2ea00_u1 = init_531d60_post_base_a2ea00_u1(post_base_a2ea00_u1_hooks);
    if (!out.post_base_a2ea00_u1.ok) {
        return out;
    }
    // E511: always-on Init continue — first 0xa2e5d0 call-site; 5 B prep NOT modelled.
    Init531d60PostBaseA2e5d0U1Hooks a2e5d0_hooks = post_base_a2e5d0_u1_hooks;
    if (a2e5d0_hooks.incoming_rbx == nullptr) {
        a2e5d0_hooks.incoming_rbx = post_base_a2ea00_u1_hooks.incoming_rbx;
    }
    a2e5d0_hooks.incoming_esi = out.post_base_a2ea00_u1.eax_return;
    out.post_base_a2e5d0_u1 = init_531d60_post_base_a2e5d0_u1(a2e5d0_hooks);
    if (!out.post_base_a2e5d0_u1.ok) {
        return out;
    }
    // E512: always-on Init continue — first 0xa543b0 call-site; 32 B prep NOT modelled.
    out.post_base_a543b0_u1 = init_531d60_post_base_a543b0_u1(post_base_a543b0_u1_hooks);
    if (!out.post_base_a543b0_u1.ok) {
        return out;
    }
    run_appmain_init_continue_driver_tail(
        out, table, alloc, setter_obj, flag_in, set_a, set_b, set_c,
        post_base_continue_a_hooks, post_base_continue_b_hooks,
        post_base_continue_c_hooks, post_base_continue_d_hooks,
        post_base_continue_e_hooks, post_base_registry_tail_hooks, post_base_registry_body_hooks,
        post_base_alt_5327bd_hooks, post_base_1fc80_u1_hooks, post_base_1e4c0_u1_hooks,
        post_base_4c4950_u1_hooks,
        post_base_a52b70_u1_hooks, post_base_a599e0_u1_hooks, post_base_a431e0_u1_hooks);
    return out;
}


namespace {

void run_remainder_after_registry_tail(
    AppmainInitCtor98a90SliceResult& out, InputTable551140& table, AllocatorFront& alloc,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c) {
    RegistryInit531d60Hooks registry_hooks{};
    if (out.post_base_registry_tail.mov_edi_count == 1) {
        registry_hooks.merge_mov_edi_98_532b0c_u1_ran = true;
    }
    if (out.post_base_registry_tail.call_17070_count == 1) {
        registry_hooks.merge_call_17070_532b11_u1_ran = true;
    }
    if (out.post_base_registry_tail.mov_r14_rax_count == 1) {
        registry_hooks.merge_mov_r14_rax_532b16_u1_ran = true;
    }
    if (out.post_base_registry_body.call_54dc10_count == 1) {
        registry_hooks.merge_call_54dc10_532b19_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_rbx_rax_count == 1) {
        registry_hooks.merge_mov_rbx_rax_532b1e_u1_ran = true;
    }
    if (out.post_base_registry_body.call_54dc20_count == 1) {
        registry_hooks.merge_call_54dc20_532b21_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r15_rax_count == 1) {
        registry_hooks.merge_mov_r15_rax_532b26_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_rsi_rbx_count == 1) {
        registry_hooks.merge_mov_rsi_rbx_532b29_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_edx_eax_count == 1) {
        registry_hooks.merge_mov_edx_eax_532b2c_u1_ran = true;
    }
    if (out.post_base_registry_body.call_52a220_count == 1) {
        registry_hooks.merge_call_52a220_532b2e_u1_ran = true;
    }
    if (out.post_base_registry_body.lea_rbx_2efd160_count == 1) {
        registry_hooks.merge_lea_rbx_2efd160_532b33_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r13_15fe518_count == 1) {
        registry_hooks.merge_mov_r13_15fe518_532b3a_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r12_rbx_count == 1) {
        registry_hooks.merge_mov_r12_rbx_532b41_u1_ran = true;
    }
    if (out.post_base_registry_body.cmp_r12_r14_count == 1) {
        registry_hooks.merge_cmp_r12_r14_532b44_u1_ran = true;
    }
    if (out.post_base_registry_body.je_532bfb_count == 1) {
        registry_hooks.merge_je_532bfb_532b47_u1_ran = true;
    }
    if (out.post_base_registry_body.test_r12_r12_count == 1) {
        registry_hooks.merge_test_r12_r12_532b4d_u1_ran = true;
    }
    if (out.post_base_registry_body.je_532bf8_count == 1) {
        registry_hooks.merge_je_532bf8_532b50_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r14_mem_rsp10_count == 1) {
        registry_hooks.merge_mov_r14_mem_rsp10_532b56_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r13_mem_rsp40_count == 1) {
        registry_hooks.merge_mov_r13_mem_rsp40_532b5b_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r14_mem_rsp48_count == 1) {
        registry_hooks.merge_mov_r14_mem_rsp48_532b60_u1_ran = true;
    }
    if (out.post_base_registry_body.cmp_r13_r14_count == 1) {
        registry_hooks.merge_cmp_r13_r14_532b65_u1_ran = true;
    }
    if (out.post_base_registry_body.je_532bba_count == 1) {
        registry_hooks.merge_je_532bba_532b68_u1_ran = true;
    }
    if (out.post_base_registry_body.lea_rsi_rsp10_count == 1) {
        registry_hooks.merge_lea_rsi_rsp10_532b6a_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_rbx_r13_10_count == 1) {
        registry_hooks.merge_mov_rbx_r13_10_532b70_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r15_r13_18_count == 1) {
        registry_hooks.merge_mov_r15_r13_18_532b74_u1_ran = true;
    }
    if (out.post_base_registry_body.cmp_r15_r11_count == 1) {
        registry_hooks.merge_cmp_r15_r11_532b78_u1_ran = true;
    }
    if (out.post_base_registry_body.je_532b98_count == 1) {
        registry_hooks.merge_je_532b98_532b7b_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice(table, alloc, setter_obj, flag_in, set_a,
                                                     set_b, set_c, registry_hooks);
}

}  // namespace

void run_appmain_init_continue_driver_tail(
    AppmainInitCtor98a90SliceResult& out, InputTable551140& table,
    AllocatorFront& alloc, void* setter_obj, std::uint8_t flag_in,
    std::uint8_t set_a, std::uint8_t set_b, std::uint8_t set_c,
    const Init531d60PostBaseContinueAHooks& post_base_continue_a_hooks,
    const Init531d60PostBaseContinueBHooks& post_base_continue_b_hooks,
    const Init531d60PostBaseContinueCHooks& post_base_continue_c_hooks,
    const Init531d60PostBaseContinueDHooks& post_base_continue_d_hooks,
    const Init531d60PostBaseContinueEHooks& post_base_continue_e_hooks,
    const Init531d60PostBaseRegistryTailHooks& post_base_registry_tail_hooks,
    const Init531d60PostBaseRegistryBodyHooks& post_base_registry_body_hooks,
    const Init531d60PostBaseAlt5327bdHooks& post_base_alt_5327bd_hooks,
    const Init531d60PostBase1fc80U1Hooks& post_base_1fc80_u1_hooks,
    const Init531d60PostBase1e4c0U1Hooks& post_base_1e4c0_u1_hooks,
    const Init531d60PostBase4c4950U1Hooks& post_base_4c4950_u1_hooks,
    const Init531d60PostBaseA52b70U1Hooks& post_base_a52b70_u1_hooks,
    const Init531d60PostBaseA599e0U1Hooks& post_base_a599e0_u1_hooks,
    const Init531d60PostBaseA431e0U1Hooks& post_base_a431e0_u1_hooks) {
    // E513: always-on Init continue — first 0x1fc80 call-site; 16 B prep NOT modelled.
    out.post_base_1fc80_u1 = init_531d60_post_base_1fc80_u1(post_base_1fc80_u1_hooks);
    if (!out.post_base_1fc80_u1.ok) {
        return;
    }
    Init531d60PostBaseContinueAHooks continue_a_hooks = post_base_continue_a_hooks;
    continue_a_hooks.incoming_rax_from_1fc80 = out.post_base_1fc80_u1.rax_from_1fc80;
    continue_a_hooks.incoming_rax_from_1fc80_known =
        out.post_base_1fc80_u1.rax_from_1fc80_known;
    // E519: always-on Init continue — 2-unit branch boundary 0x53277a..0x53277f.
    out.post_base_continue_a = init_531d60_post_base_continue_a(continue_a_hooks);
    if (!out.post_base_continue_a.ok) {
        return;
    }

    // A supplied alt gate selects the 0x5327bd route (the AB-native selector is
    // a separate task). Its error path already executes 0x5328e0..0x532904;
    // its success path jumps directly to 0x532904.
    Init531d60PostBaseContinueCHooks c_hooks = post_base_continue_c_hooks;
    std::uint8_t route_r12b = 0;
    bool alt_route = false;
    const void* route_r13 = nullptr;
    if (post_base_alt_5327bd_hooks.gate_check_supplied ||
        post_base_alt_5327bd_hooks.gate_2dfce70 != nullptr) {
        alt_route = true;
        route_r13 = post_base_alt_5327bd_hooks.incoming_r13;
        out.init_branch_5327bd_taken = true;
        Init531d60PostBaseAlt5327bdHooks alt_hooks = post_base_alt_5327bd_hooks;
        if (alt_hooks.vcall_slot0 == nullptr) {
            alt_hooks.vcall_slot0 = c_hooks.vcall_slot0;
        }
        if (alt_hooks.cell_2efe318 == nullptr && c_hooks.cell_2efe318 != nullptr) {
            alt_hooks.cell_2efe318 = static_cast<const void*>(c_hooks.cell_2efe318);
        }
        if (!alt_hooks.use_native_lazy_bind_91210 && c_hooks.use_native_lazy_bind_91210) {
            alt_hooks.use_native_lazy_bind_91210 = true;
            alt_hooks.demo_object = c_hooks.demo_object;
            alt_hooks.lazy_bind = c_hooks.lazy_bind;
        }
        out.post_base_alt_5327bd = init_531d60_post_base_alt_5327bd(alt_hooks);
        if (!out.post_base_alt_5327bd.ok) {
            return;
        }
        if (!out.post_base_alt_5327bd.r14b_known ||
            !out.post_base_alt_5327bd.r12b_known) {
            out.init_stopped_at_532904 = true;
            return;
        }
        c_hooks.incoming_r14b = out.post_base_alt_5327bd.r14b_out;
        c_hooks.enter_via_5328e0 = false;  // alt has already joined at 0x532904
        c_hooks.incoming_r15 = const_cast<void*>(route_r13);
        route_r12b = out.post_base_alt_5327bd.r12b_out;
        out.init_r12b_known = true;
    } else {
        // E514: first B-path 0x1e4c0 call-site; 15 B prep NOT modelled.
        Init531d60PostBase1e4c0U1Hooks e4_hooks = post_base_1e4c0_u1_hooks;
        if (e4_hooks.incoming_rax_standin == nullptr &&
            out.post_base_continue_a.rax_from_1fc80_known) {
            e4_hooks.incoming_rax_standin = out.post_base_continue_a.rax_from_1fc80;
        }
        if (e4_hooks.call_1e4c0 == nullptr) {
            e4_hooks.call_1e4c0 = post_base_continue_b_hooks.call_1e4c0;
        }
        out.post_base_1e4c0_u1 = init_531d60_post_base_1e4c0_u1(e4_hooks);
        if (!out.post_base_1e4c0_u1.ok) {
            return;
        }
        Init531d60PostBase4c4950U1Hooks c49_hooks = post_base_4c4950_u1_hooks;
        c49_hooks.incoming_eax_from_1e4c0_u1 = out.post_base_1e4c0_u1.eax_return;
        c49_hooks.incoming_eax_known = out.post_base_1e4c0_u1.eax_known;
        if (c49_hooks.call_4c4950 == nullptr) {
            c49_hooks.call_4c4950 = post_base_continue_b_hooks.call_4c4950;
        }
        out.post_base_4c4950_u1 = init_531d60_post_base_4c4950_u1(c49_hooks);
        if (!out.post_base_4c4950_u1.ok) {
            return;
        }
        Init531d60PostBaseContinueBHooks b_hooks = post_base_continue_b_hooks;
        b_hooks.incoming_eax_from_1e4c0_u1 = out.post_base_1e4c0_u1.eax_return;
        b_hooks.incoming_eax_known = out.post_base_1e4c0_u1.eax_known;
        out.post_base_continue_b = init_531d60_post_base_continue_b(b_hooks);
        if (!out.post_base_continue_b.ok) {
            return;
        }
        out.init_branch_5328e0_taken = out.post_base_continue_b.branch_taken;
        if (out.post_base_continue_b.eax_known) {
            route_r12b = static_cast<std::uint8_t>(out.post_base_continue_b.bl ^ 1U);
            out.init_r12b_known = true;
        }
        if (out.post_base_continue_b.branch_outcome_modelled &&
            out.post_base_continue_b.branch_taken) {
            c_hooks.enter_via_5328e0 = true;
        }
    }
    out.init_r12b_out = route_r12b;

    if (!c_hooks.enter_via_5328e0) {
        Init531d60PostBaseA52b70U1Hooks a52_hooks = post_base_a52b70_u1_hooks;
        a52_hooks.incoming_r14b = c_hooks.incoming_r14b;
        a52_hooks.incoming_r15 = c_hooks.incoming_r15;
        if (a52_hooks.call_a52b70 == nullptr) {
            a52_hooks.call_a52b70 = c_hooks.call_a52b70;
        }
        out.post_base_a52b70_u1 = init_531d60_post_base_a52b70_u1(a52_hooks);
        if (!out.post_base_a52b70_u1.ok) {
            return;
        }
        c_hooks.merge_a52b70_u1_ran = true;

        Init531d60PostBaseA599e0U1Hooks a59_hooks = post_base_a599e0_u1_hooks;
        a59_hooks.cell_32f8b80 = c_hooks.cell_32f8b80;
        a59_hooks.cell_2efe318 = c_hooks.cell_2efe318;
        a59_hooks.flag_byte_iter1 = c_hooks.flag_byte_iter1;
        if (a59_hooks.vcall_slot0 == nullptr) {
            a59_hooks.vcall_slot0 = c_hooks.vcall_slot0;
        }
        if (a59_hooks.call_a599e0 == nullptr) {
            a59_hooks.call_a599e0 = c_hooks.call_a599e0;
        }
        a59_hooks.use_native_lazy_bind_91210 = c_hooks.use_native_lazy_bind_91210;
        a59_hooks.demo_object = c_hooks.demo_object;
        a59_hooks.lazy_bind = c_hooks.lazy_bind;
        out.post_base_a599e0_u1 = init_531d60_post_base_a599e0_u1(a59_hooks);
        if (!out.post_base_a599e0_u1.ok) {
            return;
        }
        c_hooks.merge_a599e0_u1_ran = true;

        Init531d60PostBaseA431e0U1Hooks a43_hooks = post_base_a431e0_u1_hooks;
        a43_hooks.cell_32f6d18 = c_hooks.cell_32f6d18;
        a43_hooks.cell_2efe318 = c_hooks.cell_2efe318;
        a43_hooks.flag_byte_iter2 = c_hooks.flag_byte_iter2;
        if (a43_hooks.vcall_slot0 == nullptr) {
            a43_hooks.vcall_slot0 = c_hooks.vcall_slot0;
        }
        if (a43_hooks.call_a431e0 == nullptr) {
            a43_hooks.call_a431e0 = c_hooks.call_a431e0;
        }
        a43_hooks.use_native_lazy_bind_91210 = c_hooks.use_native_lazy_bind_91210;
        a43_hooks.demo_object = c_hooks.demo_object;
        a43_hooks.lazy_bind = c_hooks.lazy_bind;
        out.post_base_a431e0_u1 = init_531d60_post_base_a431e0_u1(a43_hooks);
        if (!out.post_base_a431e0_u1.ok) {
            return;
        }
        c_hooks.merge_a431e0_u1_ran = true;
    }

    // Continue C merge body starts at 0x53297f after E518 second direct call-site.
    out.post_base_continue_c = init_531d60_post_base_continue_c(c_hooks);
    if (!out.post_base_continue_c.ok) {
        return;
    }
    if (out.post_base_a599e0_u1.ok) {
        out.post_base_continue_c.flag1_known = out.post_base_a599e0_u1.flag1_known;
        out.post_base_continue_c.sil_iter1 = out.post_base_a599e0_u1.sil_iter1;
        out.post_base_continue_c.lazy_bind_calls += out.post_base_a599e0_u1.lazy_bind_calls;
        if (out.post_base_a599e0_u1.lazy_bind_calls != 0) {
            out.post_base_continue_c.lazy_bind_ok = out.post_base_a599e0_u1.lazy_bind_ok;
        }
    }
    if (out.post_base_a431e0_u1.ok) {
        out.post_base_continue_c.flag2_known = out.post_base_a431e0_u1.flag2_known;
        out.post_base_continue_c.sil_iter2 = out.post_base_a431e0_u1.sil_iter2;
        out.post_base_continue_c.lazy_bind_calls += out.post_base_a431e0_u1.lazy_bind_calls;
        if (out.post_base_a431e0_u1.lazy_bind_calls != 0 && !out.post_base_continue_c.lazy_bind_ok) {
            out.post_base_continue_c.lazy_bind_ok = out.post_base_a431e0_u1.lazy_bind_ok;
        }
    }

    Init531d60PostBaseContinueEHooks e_hooks = post_base_continue_e_hooks;
    e_hooks.incoming_r12b = route_r12b;
    if (alt_route) {
        e_hooks.incoming_r13 = route_r13;
    }
    out.init_branch_5329ae_taken = out.post_base_continue_c.merge_branch_taken;
    if (out.post_base_continue_c.merge_branch_outcome_modelled &&
        out.post_base_continue_c.merge_branch_taken) {
        if (!out.init_r12b_known) {
            out.init_stopped_at_5329ae = true;
            return;
        }
        e_hooks.incoming_r12b = route_r12b;
        out.post_base_continue_e = init_531d60_post_base_continue_e(e_hooks);
        if (!out.post_base_continue_e.ok) {
            out.init_stopped_at_5329ae = true;
            return;
        }
        out.post_base_registry_tail =
            init_531d60_post_base_registry_tail(post_base_registry_tail_hooks);
        if (!out.post_base_registry_tail.ok) {
            out.init_stopped_at_5329ae = true;
            return;
        }
        {
            Init531d60PostBaseRegistryBodyHooks body_hooks = post_base_registry_body_hooks;
            body_hooks.table_in = &table;
            out.post_base_registry_body = init_531d60_post_base_registry_body(body_hooks);
            if (!out.post_base_registry_body.ok) {
                out.init_stopped_at_5329ae = true;
                return;
            }
        }
        run_remainder_after_registry_tail(out, table, alloc, setter_obj, flag_in, set_a, set_b,
                                        set_c);
        return;
    }

    // 0x53299b not taken (or unmodelled): continue D, fall through to E, then
    // reach the registry/remainder. Stop at 0x5329ae if B did not supply the
    // r12b bit; the E hook's default byte is not register provenance.
    out.post_base_continue_d = init_531d60_post_base_continue_d(
        post_base_continue_d_hooks);
    if (!out.post_base_continue_d.ok) {
        return;
    }
    if (!out.init_r12b_known) {
        out.init_stopped_at_5329ae = true;
        return;
    }
    e_hooks.incoming_r12b = route_r12b;
    out.post_base_continue_e = init_531d60_post_base_continue_e(e_hooks);
    if (!out.post_base_continue_e.ok) {
        out.init_stopped_at_5329ae = true;
        return;
    }
        out.post_base_registry_tail =
            init_531d60_post_base_registry_tail(post_base_registry_tail_hooks);
        if (!out.post_base_registry_tail.ok) {
            out.init_stopped_at_5329ae = true;
            return;
        }
        {
            Init531d60PostBaseRegistryBodyHooks body_hooks = post_base_registry_body_hooks;
            body_hooks.table_in = &table;
            out.post_base_registry_body = init_531d60_post_base_registry_body(body_hooks);
            if (!out.post_base_registry_body.ok) {
                out.init_stopped_at_5329ae = true;
                return;
            }
        }
        run_remainder_after_registry_tail(out, table, alloc, setter_obj, flag_in, set_a, set_b,
                                        set_c);
}

}  // namespace sotc
