#include "appmain_init_base_initialize_slice.hpp"

#include <cstring>

namespace sotc {

AppmainInitBaseInitializeSliceResult run_appmain_init_base_initialize_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const BaseInitialize255130Hooks& hooks, void* flag_obj, void* setter_obj,
    std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b, std::uint8_t set_c,
    const SingletonF08Hooks& singleton_hooks) {
    AppmainInitBaseInitializeSliceResult out{};

    // OBS 0x531d84..0x531dcf before base Initialize @ 0x531dda.
    out.singleton = install_singleton_f08_531d84(alloc, singleton_hooks);
    if (!out.singleton.ok) {
        return out;
    }

    base_init_flag_cell_2ea4718() = flag_obj;
    std::uint32_t prev = 0;
    if (flag_obj != nullptr) {
        std::memcpy(&prev, static_cast<std::uint8_t*>(flag_obj) + kXchgStore357f0Offset, 4);
        out.flag_prev = prev;
    }

    out.base_ok = base_initialize_255130(self, arg, hooks);
    if (flag_obj != nullptr) {
        std::memcpy(&out.flag_after,
                    static_cast<std::uint8_t*>(flag_obj) + kXchgStore357f0Offset, 4);
    }

    if (!out.base_ok) {
        out.base_fail_visible = true;
        return out;
    }

    out.remainder = run_appmain_init_remainder_slice(table, alloc, setter_obj, flag_in, set_a,
                                                     set_b, set_c);
    return out;
}

}  // namespace sotc
