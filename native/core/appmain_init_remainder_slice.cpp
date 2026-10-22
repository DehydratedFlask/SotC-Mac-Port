#include "appmain_init_remainder_slice.hpp"

namespace sotc {

AppmainInitRemainderSliceResult run_appmain_init_remainder_slice(
    InputTable551140& table, AllocatorFront& alloc, void* setter_obj, std::uint8_t flag_in,
    std::uint8_t set_a, std::uint8_t set_b, std::uint8_t set_c) {
    AppmainInitRemainderSliceResult out{};

    // Early Init leave contracts (after a successful base Initialize in the original).
    ret_nop_56dca0();
    ret_nop_56db70();
    ret_nop_5b3d00();
    ret_nop_5b3cf0();
    ret_nop_56fad0();
    hide_splash_screen_f90310();

    // Mid-path trivial callees immediately before registry fragment.
    set_init_flag_pair_4c4950(flag_in);
    set_byte_a52b70(setter_obj, set_a);
    set_byte_a599e0(setter_obj, set_b);
    set_byte_a431e0(setter_obj, set_c);

    out.flag_2efa890 = init_flag_cell_2efa890();
    out.flag_2efa891 = init_flag_cell_2efa891();
    auto* bytes = static_cast<std::uint8_t*>(setter_obj);
    out.setter_at_01 = bytes[kSetterOffA52b70];
    out.setter_at_29 = bytes[kSetterOffA599e0];
    out.setter_at_51 = bytes[kSetterOffA431e0];
    out.trivial_ok = (out.flag_2efa890 == flag_in) && (out.flag_2efa891 == 1) &&
                     (out.setter_at_01 == set_a) && (out.setter_at_29 == set_b) &&
                     (out.setter_at_51 == set_c);

    out.registry = run_appmain_init_registry_slice(table, alloc);
    return out;
}

}  // namespace sotc
