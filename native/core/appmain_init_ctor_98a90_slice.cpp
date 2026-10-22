#include "appmain_init_ctor_98a90_slice.hpp"

#include <cstring>

namespace sotc {

AppmainInitCtor98a90SliceResult run_appmain_init_ctor_98a90_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const Ctor98a90_3be050Hooks& ctor_hooks, const BaseInitialize255130Hooks& base_hooks,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c, const SingletonF08Hooks& singleton_hooks) {
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
    out.base_ok = base_initialize_255130(self, arg, hooks);
    if (out.early.flag_obj != nullptr) {
        std::memcpy(&out.flag_after_base,
                    static_cast<std::uint8_t*>(out.early.flag_obj) + kXchgStore357f0Offset, 4);
    }
    if (!out.base_ok) {
        return out;
    }
    out.remainder = run_appmain_init_remainder_slice(table, alloc, setter_obj, flag_in, set_a,
                                                     set_b, set_c);
    return out;
}

}  // namespace sotc
