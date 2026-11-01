#include "init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"

namespace sotc {

Init531d60PostBaseSubEax_010cb932_532be1_U1Result init_531d60_post_base_sub_eax_010cb932_532be1_u1(
    const Init531d60PostBaseSubEax_010cb932_532be1_U1Cells& cells) noexcept {
    Init531d60PostBaseSubEax_010cb932_532be1_U1Result out{};
    ++out.sub_eax_010cb932_532be1_count;

    out.eax = cells.eax_in - kInit531d60PostBaseSubEax_010cb932_532be1_U1Imm;
    if (cells.eax_out != nullptr) {
        *cells.eax_out = out.eax;
    }

    // Next: E604 test/je @ 0x532be6 — not modelled here.
    out.ok = (out.sub_eax_010cb932_532be1_count == 1);
    return out;
}

}  // namespace sotc
