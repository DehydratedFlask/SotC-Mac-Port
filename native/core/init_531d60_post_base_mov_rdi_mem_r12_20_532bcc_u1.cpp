#include "init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiMemR12_20_532bcc_U1Result init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1(
    const Init531d60PostBaseMovRdiMemR12_20_532bcc_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiMemR12_20_532bcc_U1Result out{};

    void* rdi = nullptr;
    if (cells.r12_base != nullptr) {
        const auto* base = static_cast<const std::uint8_t*>(cells.r12_base);
        rdi = *reinterpret_cast<void* const*>(base + kInit531d60PostBaseMovRdiMemR12_20_532bcc_U1Disp);
    }
    out.rdi = rdi;
    if (cells.rdi_out != nullptr) {
        *cells.rdi_out = rdi;
    }
    ++out.mov_rdi_mem_r12_20_532bcc_count;

    out.ok = (out.mov_rdi_mem_r12_20_532bcc_count == 1);
    return out;
}

}  // namespace sotc
