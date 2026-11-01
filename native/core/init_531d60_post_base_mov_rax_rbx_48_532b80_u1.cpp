#include "init_531d60_post_base_mov_rax_rbx_48_532b80_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRaxRbx_48_532b80_U1Result init_531d60_post_base_mov_rax_rbx_48_532b80_u1(
    const Init531d60PostBaseMovRaxRbx_48_532b80_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRaxRbx_48_532b80_U1Result out{};

    void* rax = nullptr;
    if (cells.rbx_base != nullptr) {
        const auto* base = static_cast<const std::uint8_t*>(cells.rbx_base);
        rax = *reinterpret_cast<void* const*>(base + kInit531d60PostBaseMovRaxRbx_48_532b80_U1Disp);
    }
    out.rax = rax;
    if (cells.rax_out != nullptr) {
        *cells.rax_out = rax;
    }
    ++out.mov_rax_rbx_48_count;

    // Next: lea rdi,[rbx+0x28] @ 0x532b84 — peeled in E577 sibling unit.
    out.ok = (out.mov_rax_rbx_48_count == 1);
    return out;
}

}  // namespace sotc
