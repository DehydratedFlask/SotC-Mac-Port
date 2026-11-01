#include "init_531d60_post_base_lea_rbx_2efd160_532bb3_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRbx2efd160_532bb3_U1Result init_531d60_post_base_lea_rbx_2efd160_532bb3_u1(
    const Init531d60PostBaseLeaRbx2efd160_532bb3_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRbx2efd160_532bb3_U1Result out{};

    out.rbx = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRbx2efd160_532bb3_U1TargetRva));
    if (cells.rbx_out != nullptr) {
        *cells.rbx_out = out.rbx;
    }
    ++out.lea_rbx_2efd160_532bb3_count;

    out.ok = (out.lea_rbx_2efd160_532bb3_count == 1);
    return out;
}

}  // namespace sotc
