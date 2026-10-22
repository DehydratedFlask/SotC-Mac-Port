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
    const Init531d60PostBase17070U2Hooks& post_base_17070_u2_hooks) {
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
    out.post_base_1290 = init_531d60_post_base_1290(post_base_1290_hooks);
    if (!out.post_base_1290.ok) {
        return out;
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
    out.remainder = run_appmain_init_remainder_slice(table, alloc, setter_obj, flag_in, set_a,
                                                     set_b, set_c);
    return out;
}

}  // namespace sotc
