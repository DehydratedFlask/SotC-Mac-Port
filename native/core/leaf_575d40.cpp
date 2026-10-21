#include "leaf_575d40.hpp"

#include <cstring>

namespace sotc {

void init_leaf_575d40(const Leaf575d40Cell& source, Leaf575d40Cell& dest) noexcept {
    // 0x575d4e: mov ecx, [rax+0x10]   0x575d51: vmovups xmm0, [rax]   (both loads first)
    std::uint8_t tail[4];
    std::uint8_t head[16];
    std::memcpy(tail, source.data() + 0x10, sizeof tail);
    std::memcpy(head, source.data(), sizeof head);
    // 0x575d55: mov [rdx+0x10], ecx   0x575d58: vmovups [rdx], xmm0
    std::memcpy(dest.data() + 0x10, tail, sizeof tail);
    std::memcpy(dest.data(), head, sizeof head);
}

}  // namespace sotc
