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

    // OBS 0x255152: call 0x255340
    if (hooks.mid_255340 != nullptr) {
        hooks.mid_255340(self);
    }

    // OBS 0x255160: call [rax+0x40] with rsi=arg
    if (hooks.vslot_40 != nullptr) {
        hooks.vslot_40(self, arg);
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
