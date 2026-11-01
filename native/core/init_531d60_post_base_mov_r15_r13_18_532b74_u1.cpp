#include "init_531d60_post_base_mov_r15_r13_18_532b74_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR15R13_18_532b74_U1Result init_531d60_post_base_mov_r15_r13_18_532b74_u1(
    const Init531d60PostBaseMovR15R13_18_532b74_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR15R13_18_532b74_U1Result out{};

    void* r15 = nullptr;
    if (cells.r13_base != nullptr) {
        const auto* base = static_cast<const std::uint8_t*>(cells.r13_base);
        r15 = *reinterpret_cast<void* const*>(base + kInit531d60PostBaseMovR15R13_18_532b74_U1Disp);
    }
    out.r15 = r15;
    if (cells.r15_out != nullptr) {
        *cells.r15_out = r15;
    }
    ++out.mov_r15_r13_18_count;

    // Next: cmp r15,r11 @ 0x532b78 — peeled as registry_body unit E573.
    out.ok = (out.mov_r15_r13_18_count == 1);
    return out;
}

}  // namespace sotc
