#include "init_531d60_post_base_mov_rbx_r13_10_532b94_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRbxR13_10_532b94_U1Result init_531d60_post_base_mov_rbx_r13_10_532b94_u1(
    const Init531d60PostBaseMovRbxR13_10_532b94_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRbxR13_10_532b94_U1Result out{};

    void* rbx = nullptr;
    if (cells.r13_base != nullptr) {
        const auto* base = static_cast<const std::uint8_t*>(cells.r13_base);
        rbx = *reinterpret_cast<void* const*>(base + kInit531d60PostBaseMovRbxR13_10_532b94_U1Disp);
    }
    out.rbx = rbx;
    if (cells.rbx_out != nullptr) {
        *cells.rbx_out = rbx;
    }
    ++out.mov_rbx_r13_10_532b94_count;

    // Next: test rbx,rbx @ 0x532b98 — not modelled here.
    out.ok = (out.mov_rbx_r13_10_532b94_count == 1);
    return out;
}

}  // namespace sotc
