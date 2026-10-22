#include "appmain_init_prologue_tail_slice.hpp"

#include <cstring>

namespace sotc {

AppmainInitPrologueTailSliceResult run_appmain_init_prologue_tail_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const BaseInitialize255130Hooks& base_hooks, void* setter_obj, std::uint8_t flag_in,
    std::uint8_t set_a, std::uint8_t set_b, std::uint8_t set_c) {
    AppmainInitPrologueTailSliceResult out{};
    out.early = run_prologue_255180_early(alloc, early_hooks);
    if (!out.early.ok) {
        return out;
    }
    out.tail = run_prologue_255180_tail(alloc, self, tail_hooks);
    if (!out.tail.ok) {
        return out;
    }

    BaseInitialize255130Hooks hooks = base_hooks;
    hooks.prologue_255180 = nullptr;  // early+tail already ran

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
