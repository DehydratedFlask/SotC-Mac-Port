#include "init_531d60_post_base_mov_edx_2_532afe_u1.hpp"

namespace sotc {

Init531d60PostBaseMovEdx2_532afe_U1Result init_531d60_post_base_mov_edx_2_532afe_u1() noexcept {
    Init531d60PostBaseMovEdx2_532afe_U1Result out{};
    out.edx = kInit531d60PostBaseMovEdx2_532afe_U1Immediate;
    ++out.mov_edx_count;
    // Next: mov rdi,[rax] @ 0x532b03 — see continue_e.
    out.ok = (out.mov_edx_count == 1 && out.edx == 2);
    return out;
}

}  // namespace sotc
