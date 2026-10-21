#include "leaf_9b75a0.hpp"

#include <algorithm>

namespace sotc {

void init_leaf_9b75a0(Leaf9b75a0& g) noexcept {
    std::uint8_t* const cells = g.cells.data();
    // rax counts 8, 0x30, ... until 0x2808 (256 iterations); each iteration issues the store at
    // table+rax+0xc and then the store at table+rax, both 16 zero bytes. Offsets below are
    // relative to table+8 (the first store address of the loop).
    for (std::size_t record = 0; record != kLeaf9b75a0Records; ++record) {
        std::uint8_t* const at = cells + record * kLeaf9b75a0Stride;
        std::fill_n(at + 0x0c, 16, std::uint8_t{0});
        std::fill_n(at, 16, std::uint8_t{0});
    }
    // 0x9b75c7: mov dword [table + 0x2800], 0
    std::fill_n(cells + 0x27f8, 4, std::uint8_t{0});
}

}  // namespace sotc
