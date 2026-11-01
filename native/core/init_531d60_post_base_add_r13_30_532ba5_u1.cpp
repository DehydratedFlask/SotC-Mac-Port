#include "init_531d60_post_base_add_r13_30_532ba5_u1.hpp"

namespace sotc {

Init531d60PostBaseAddR13_30_532ba5_U1Result init_531d60_post_base_add_r13_30_532ba5_u1(
    const Init531d60PostBaseAddR13_30_532ba5_U1Cells& cells) noexcept {
    Init531d60PostBaseAddR13_30_532ba5_U1Result out{};

    if (cells.r13_in != nullptr) {
        out.r13 = reinterpret_cast<void*>(
            reinterpret_cast<std::uintptr_t>(cells.r13_in) +
            static_cast<std::uintptr_t>(kInit531d60PostBaseAddR13_30_532ba5_U1Imm));
    }
    if (cells.r13_out != nullptr) {
        *cells.r13_out = out.r13;
    }
    ++out.add_r13_30_532ba5_count;

    out.ok = (out.add_r13_30_532ba5_count == 1);
    return out;
}

}  // namespace sotc
