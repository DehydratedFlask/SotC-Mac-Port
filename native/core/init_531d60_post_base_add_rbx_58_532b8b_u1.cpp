#include "init_531d60_post_base_add_rbx_58_532b8b_u1.hpp"

namespace sotc {

Init531d60PostBaseAddRbx_58_532b8b_U1Result init_531d60_post_base_add_rbx_58_532b8b_u1(
    const Init531d60PostBaseAddRbx_58_532b8b_U1Cells& cells) noexcept {
    Init531d60PostBaseAddRbx_58_532b8b_U1Result out{};

    if (cells.rbx_in != nullptr) {
        out.rbx = reinterpret_cast<void*>(
            reinterpret_cast<std::uintptr_t>(cells.rbx_in) +
            static_cast<std::uintptr_t>(kInit531d60PostBaseAddRbx_58_532b8b_U1Imm));
    }
    if (cells.rbx_out != nullptr) {
        *cells.rbx_out = out.rbx;
    }
    ++out.add_rbx_58_count;

    out.ok = (out.add_rbx_58_count == 1);
    return out;
}

}  // namespace sotc
