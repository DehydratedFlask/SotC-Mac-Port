#include "init_531d60_post_base_test_r12_r12_532b4d_u1.hpp"

namespace sotc {

Init531d60PostBaseTestR12R12_532b4d_U1Result init_531d60_post_base_test_r12_r12_532b4d_u1(
    const Init531d60PostBaseTestR12R12_532b4d_U1Cells& cells) noexcept {
    Init531d60PostBaseTestR12R12_532b4d_U1Result out{};

    out.r12_is_zero = (cells.r12_in == nullptr);
    ++out.test_r12_r12_count;

    // Next: je @ 0x532b50 — peeled as E564 u1.
    out.ok = (out.test_r12_r12_count == 1);
    return out;
}

}  // namespace sotc
