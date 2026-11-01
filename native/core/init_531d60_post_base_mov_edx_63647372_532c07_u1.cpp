#include "init_531d60_post_base_mov_edx_63647372_532c07_u1.hpp"

namespace sotc {

Init531d60PostBaseMovEdx63647372_532c07_U1Result init_531d60_post_base_mov_edx_63647372_532c07_u1() noexcept {
    Init531d60PostBaseMovEdx63647372_532c07_U1Result out{};
    out.edx = kInit531d60PostBaseMovEdx63647372_532c07_U1Immediate;
    ++out.mov_edx_63647372_532c07_count;
    // Next: mov rdi,[rax] @ 0x532c0c — not modelled here.
    out.ok = (out.mov_edx_63647372_532c07_count == 1 &&
              out.edx == kInit531d60PostBaseMovEdx63647372_532c07_U1Immediate);
    return out;
}

}  // namespace sotc
