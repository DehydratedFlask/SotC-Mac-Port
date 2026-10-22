#include "atomic_xadd_b30cf0.hpp"

namespace sotc {

std::int32_t atomic_dec_fetch_b30cf0(std::atomic<std::int32_t>& cell) noexcept {
    const std::uint32_t old = static_cast<std::uint32_t>(cell.fetch_add(-1, std::memory_order_seq_cst));
    return static_cast<std::int32_t>(old - 1u);        // `dec eax` on the xadd result (wraps)
}

std::int32_t atomic_add_fetch_b30d00(std::atomic<std::int32_t>& cell, std::int32_t delta) noexcept {
    const std::uint32_t old = static_cast<std::uint32_t>(cell.fetch_add(delta, std::memory_order_seq_cst));
    return static_cast<std::int32_t>(old + static_cast<std::uint32_t>(delta));   // `lea eax,[rax+rsi]`
}

}  // namespace sotc
