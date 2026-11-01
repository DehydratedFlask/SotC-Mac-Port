#include "init_531d60_post_base_mov_r13_8_rax_532bd9_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR13_8_Rax_532bd9_U1Result init_531d60_post_base_mov_r13_8_rax_532bd9_u1(
    const Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR13_8_Rax_532bd9_U1Result out{};
    out.rax_stored = cells.rax_in;
    if (cells.r13_8_out != nullptr) {
        *cells.r13_8_out = cells.rax_in;
    } else if (cells.r13_base != nullptr) {
        *reinterpret_cast<void**>(static_cast<char*>(cells.r13_base) +
                                  kInit531d60PostBaseMovR13_8_Rax_532bd9_U1Disp) = cells.rax_in;
    }
    ++out.mov_r13_8_rax_532bd9_count;
    // Next: E602 mov rax,[rdi] @ 0x532bde — not modelled here.
    out.ok = (out.mov_r13_8_rax_532bd9_count == 1);
    return out;
}

}  // namespace sotc
