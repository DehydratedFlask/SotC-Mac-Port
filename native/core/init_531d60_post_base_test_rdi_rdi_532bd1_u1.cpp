#include "init_531d60_post_base_test_rdi_rdi_532bd1_u1.hpp"

namespace sotc {

Init531d60PostBaseTestRdiRdi_532bd1_U1Result init_531d60_post_base_test_rdi_rdi_532bd1_u1(
    const Init531d60PostBaseTestRdiRdi_532bd1_U1Cells& cells) noexcept {
    Init531d60PostBaseTestRdiRdi_532bd1_U1Result out{};

    out.rdi_is_zero = (cells.rdi_in == nullptr);
    ++out.test_rdi_rdi_count;

    out.ok = (out.test_rdi_rdi_count == 1);
    return out;
}

}  // namespace sotc
