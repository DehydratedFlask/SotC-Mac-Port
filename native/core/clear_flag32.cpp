#include "clear_flag32.hpp"

namespace sotc {

void clear_flag32(std::atomic<std::uint32_t>& cell) noexcept {
    // Plain xchg m32 is implicitly locked on x86; model as seq_cst exchange discarding the old value.
    (void)cell.exchange(0, std::memory_order_seq_cst);
}

}  // namespace sotc
