#include "leaf_de0160.hpp"

#include <cstring>

namespace sotc {

void init_leaf_de0160(LeafDe0160& g) noexcept {
    // Original: lea rdi,[0x3352ee0]; mov esi,1; mov edx,0x400; call 0xf8f4f0 (memset).
    std::memset(g.fill.data(), 1, g.fill.size());
    // Original: vxorps xmm0,xmm0,xmm0; vmovups [0x33532e0],xmm0.
    std::memset(g.trailer.data(), 0, g.trailer.size());
    // EXPLICIT OMISSION: jmp 0xf8f390 → __cxa_atexit(0xddf980, 0, 0x1604218).
    // 0xddf980 is a one-byte `ret`; registering it would be a no-op at process exit.
}

}  // namespace sotc
