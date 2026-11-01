#include "init_531d60_post_base_cmp_r15_r11_532b78_u1.hpp"

namespace sotc {

Init531d60PostBaseCmpR15R11_532b78_U1Result init_531d60_post_base_cmp_r15_r11_532b78_u1(
    const Init531d60PostBaseCmpR15R11_532b78_U1Cells& cells) noexcept {
    Init531d60PostBaseCmpR15R11_532b78_U1Result out{};

    out.pointers_equal = (cells.r15_in == cells.r11_in);
    ++out.cmp_r15_r11_count;

    // Next: je @ 0x532b7b — peeled as registry_body unit E574.
    out.ok = (out.cmp_r15_r11_count == 1);
    return out;
}

}  // namespace sotc
