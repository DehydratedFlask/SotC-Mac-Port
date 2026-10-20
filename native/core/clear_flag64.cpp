#include "clear_flag64.hpp"

namespace sotc {

void clear_flag64(std::atomic<std::uint64_t>& cell) noexcept {
    // Original: xor eax,eax; xchg rax,[cell] (REX.W). xchg-to-memory is implicitly locked on x86.
    (void)cell.exchange(0, std::memory_order_seq_cst);
}

}  // namespace sotc
