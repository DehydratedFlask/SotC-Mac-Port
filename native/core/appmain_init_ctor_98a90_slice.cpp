#include "appmain_init_ctor_98a90_slice.hpp"

#include <cstring>

namespace sotc {

AppmainInitCtor98a90SliceResult run_appmain_init_ctor_98a90_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const Ctor98a90_3be050Hooks& ctor_hooks, const BaseInitialize255130Hooks& base_hooks,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c) {
    AppmainInitCtor98a90SliceResult out{};
    out.early = run_prologue_255180_early(alloc, early_hooks);
    if (!out.early.ok) {
        return out;
    }

    // Tail with lifted 0x3be050 (override null → use_lifted).
    Prologue255180TailHooks th = tail_hooks;
    th.ctor_98a90_3be050 = nullptr;
    th.use_lifted_ctor_98a90 = true;
    th.ctor_98a90_detail = ctor_hooks;
    out.tail = run_prologue_255180_tail(alloc, self, th);
    if (!out.tail.ok) {
        return out;
    }
    // Re-read ctor side effects from installed object.
    if (out.tail.obj_98a90 != nullptr) {
        auto* o = static_cast<std::uint8_t*>(out.tail.obj_98a90);
        std::memcpy(&out.ctor.nested_48, o + 8, sizeof(void*));
        std::memcpy(&out.ctor.buf_80, o + kCtor98a90VecBegin, sizeof(void*));
        out.ctor.ok = (out.ctor.nested_48 != nullptr) && (out.ctor.buf_80 != nullptr);
        out.ctor.ensure_called = true;  // header zeros ⇒ ensure path
        out.ctor.buf_grown = (out.ctor.buf_80 != nullptr);
    }

    BaseInitialize255130Hooks hooks = base_hooks;
    hooks.prologue_255180 = nullptr;
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
