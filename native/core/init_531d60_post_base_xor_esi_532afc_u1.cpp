#include "init_531d60_post_base_xor_esi_532afc_u1.hpp"

namespace sotc {

Init531d60PostBaseXorEsi_532afc_U1Result init_531d60_post_base_xor_esi_532afc_u1() noexcept {
    Init531d60PostBaseXorEsi_532afc_U1Result out{};
    out.esi = 0;
    ++out.xor_esi_count;
    // Next: mov edx,2 @ 0x532afe — see continue_e.
    out.ok = (out.xor_esi_count == 1);
    return out;
}

}  // namespace sotc
