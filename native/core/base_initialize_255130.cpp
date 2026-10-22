#include "base_initialize_255130.hpp"

namespace sotc {
namespace {

void* g_flag_cell_2ea4718 = nullptr;

}  // namespace

void*& base_init_flag_cell_2ea4718() noexcept { return g_flag_cell_2ea4718; }

bool base_initialize_255130(void* self, void* arg,
                            const BaseInitialize255130Hooks& hooks) noexcept {
    // OBS 0x25513d: call 0x255180
    if (hooks.prologue_255180 != nullptr) {
        hooks.prologue_255180(self);
    }

    // OBS 0x255148: call [rax+0x30] → 0x534130; test al,al; jz fail
    bool ok30 = false;
    if (hooks.vslot_30 != nullptr) {
        ok30 = hooks.vslot_30(self);
    } else if (hooks.use_lifted_vslot_30) {
        ok30 = vslot_30_534130(self, hooks.vslot_30_detail).ok;
    }
    if (!ok30) {
        return false;
    }

    // OBS 0x255152: call 0x255340 (E419 lift when flagged)
    if (hooks.mid_255340 != nullptr) {
        hooks.mid_255340(self);
    } else if (hooks.use_lifted_mid_255340) {
        (void)mid_255340(self, hooks.mid_255340_detail);
    }

    // OBS 0x255160: call [rax+0x40] with rsi=arg
    if (hooks.vslot_40 != nullptr) {
        hooks.vslot_40(self, arg);
    } else if (hooks.use_lifted_vslot_40_header) {
        if (hooks.vslot_40_alloc == nullptr) {
            return false;
        }
        const Vslot40_533100HeaderResult hr =
            vslot_40_533100_header(*hooks.vslot_40_alloc, self, arg, hooks.vslot_40_detail);
        if (hooks.vslot_40_out != nullptr) {
            *hooks.vslot_40_out = hr;
        }
        if (!hr.ok) {
            return false;
        }
        // E422: always-on post-header fragment (stops at show_areaInfo branch).
        const Vslot40_533100PostResult pr =
            vslot_40_533100_post(self, arg, hooks.vslot_40_post_detail);
        if (hooks.vslot_40_post_out != nullptr) {
            *hooks.vslot_40_post_out = pr;
        }
        if (!pr.ok) {
            return false;
        }
        // E423: measured empty-table takes miss path (hit side effects before join NOT modelled).
        if (!pr.lookup_hit) {
            const Vslot40_533100MissResult mr =
                vslot_40_533100_miss(hooks.vslot_40_miss_detail);
            if (hooks.vslot_40_miss_out != nullptr) {
                *hooks.vslot_40_miss_out = mr;
            }
            if (!mr.ok) {
                return false;
            }
        }
        // E424: always-on join+ (miss and hit converge @ 0x533305).
        const Vslot40_533100JoinResult jr =
            vslot_40_533100_join(*hooks.vslot_40_alloc, self, hooks.vslot_40_join_detail);
        if (hooks.vslot_40_join_out != nullptr) {
            *hooks.vslot_40_join_out = jr;
        }
        if (!jr.ok) {
            return false;
        }
        // E425: always-on gfx-cell prelude before shadowmodels.crmp.
        const Vslot40_533100GfxResult gr = vslot_40_533100_gfx_2ee2d00(hooks.vslot_40_gfx_detail);
        if (hooks.vslot_40_gfx_out != nullptr) {
            *hooks.vslot_40_gfx_out = gr;
        }
        if (!gr.ok) {
            return false;
        }
        // E426: always-on shadowmodels load-attempt (stop before success branch).
        const Vslot40_533100ShadowResult sr =
            vslot_40_533100_shadowmodels(hooks.vslot_40_shadow_detail);
        if (hooks.vslot_40_shadow_out != nullptr) {
            *hooks.vslot_40_shadow_out = sr;
        }
        if (!sr.ok) {
            return false;
        }
        // E427: always-on attach (measured takes fail path; stop before shadowskels).
        const Vslot40_533100ShadowAttachResult ar = vslot_40_533100_shadow_attach(
            sr.loaded, /*success_flag=*/false, hooks.vslot_40_shadow_attach_detail);
        if (hooks.vslot_40_shadow_attach_out != nullptr) {
            *hooks.vslot_40_shadow_attach_out = ar;
        }
        if (!ar.ok) {
            return false;
        }
        // E428: always-on shadowskels load-attempt (stop before success atexit).
        const Vslot40_533100ShadowskelsResult kr =
            vslot_40_533100_shadowskels(hooks.vslot_40_shadowskels_detail);
        if (hooks.vslot_40_shadowskels_out != nullptr) {
            *hooks.vslot_40_shadowskels_out = kr;
        }
        if (!kr.ok) {
            return false;
        }
    }

    // OBS 0x255163..0x255174: rdi=*0x2ea4718; sil=0; call 0x357f0; return true
    void* flag_obj = g_flag_cell_2ea4718;
    if (flag_obj == nullptr) {
        return false;
    }
    (void)xchg_store_u32_357f0(flag_obj, 0);
    return true;
}

}  // namespace sotc
