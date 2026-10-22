#include "leaf_7ce8d0.hpp"

namespace sotc {

void init_leaf_7ce8d0(Leaf7ce8d0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x325e470: 8-byte store: 01000000 0000803f (movabs rax,0x3f80000000000001)
    g.word0 = 0x3f80000000000001ull;
    // 0x325e480: xchg dword [cell],ecx with ecx == 0 (locked: full barrier; old value discarded)
    g.flag0.exchange(0u, std::memory_order_seq_cst);
    // 0x325e490: 8-byte store of the same constant
    g.word1 = 0x3f80000000000001ull;
    // 0x325e4a0: xchg dword [cell],ecx with ecx == 0
    g.flag1.exchange(0u, std::memory_order_seq_cst);
}

}  // namespace sotc
