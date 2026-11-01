#include "init_531d60_post_base_mov_r13_mem_r12_40_532bae_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR13MemR12_40_532bae_U1Result init_531d60_post_base_mov_r13_mem_r12_40_532bae_u1(
    const Init531d60PostBaseMovR13MemR12_40_532bae_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR13MemR12_40_532bae_U1Result out{};

    void* r13 = nullptr;
    if (cells.r12_base != nullptr) {
        const auto* base = static_cast<const std::uint8_t*>(cells.r12_base);
        r13 = *reinterpret_cast<void* const*>(base + kInit531d60PostBaseMovR13MemR12_40_532bae_U1Disp);
    }
    out.r13 = r13;
    if (cells.r13_out != nullptr) {
        *cells.r13_out = r13;
    }
    ++out.mov_r13_mem_r12_40_532bae_count;

    out.ok = (out.mov_r13_mem_r12_40_532bae_count == 1);
    return out;
}

}  // namespace sotc
