#include "init_531d60_post_base_test_rbx_rbx_532b98_u1.hpp"

namespace sotc {

Init531d60PostBaseTestRbxRbx_532b98_U1Result init_531d60_post_base_test_rbx_rbx_532b98_u1(
    const Init531d60PostBaseTestRbxRbx_532b98_U1Cells& cells) noexcept {
    Init531d60PostBaseTestRbxRbx_532b98_U1Result out{};

    out.rbx_is_zero = (cells.rbx_in == nullptr);
    ++out.test_rbx_rbx_count;

    out.ok = (out.test_rbx_rbx_count == 1);
    return out;
}

}  // namespace sotc
