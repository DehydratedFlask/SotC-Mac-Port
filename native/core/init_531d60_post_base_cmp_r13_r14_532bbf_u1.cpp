#include "init_531d60_post_base_cmp_r13_r14_532bbf_u1.hpp"

namespace sotc {

Init531d60PostBaseCmpR13R14_532bbf_U1Result init_531d60_post_base_cmp_r13_r14_532bbf_u1(
    const Init531d60PostBaseCmpR13R14_532bbf_U1Cells& cells) noexcept {
    Init531d60PostBaseCmpR13R14_532bbf_U1Result out{};

    out.pointers_equal = (cells.r13_in == nullptr);
    ++out.cmp_r13_r14_532bbf_count;

    // Next: je @ 0x532bc2 — not modelled in this unit.
    out.ok = (out.cmp_r13_r14_532bbf_count == 1);
    return out;
}

}  // namespace sotc
