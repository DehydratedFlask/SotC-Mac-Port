#include "scratch_reset.hpp"

#include <cstring>

namespace sotc {

void reset_scratch(std::array<std::byte, kScratchBytes>& buffer, std::atomic<std::uint64_t>& slot) noexcept {
    std::memset(buffer.data(), 0, buffer.size());
    // xchg with a memory operand is implicitly locked: seq_cst. Result of the exchange is unused.
    (void)slot.exchange(0, std::memory_order_seq_cst);
}

}  // namespace sotc
