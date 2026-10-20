#include "exact_fit_finalize.hpp"

#include <cstring>

namespace sotc {

void exact_fit_finalize(RegionNode& node, std::uint64_t req_size, std::uint8_t arena_flag,
                        std::uint64_t ctx) noexcept {
    node.set_flag_a(arena_flag);                          // OBS 0x1aae4: mov [r10], dil
    node.set_size(req_size);                              // OBS 0x1aae7: mov [r9], rbx
    node.raw[RegionNode::kFlagBOffset] = 1;               // OBS 0x1aaea: mov byte [r14+0x69], 1
    std::memcpy(node.raw + RegionNode::kPtrOffset, &ctx, 8);  // OBS 0x1aaef
    std::uint32_t all_ones = 0xffffffffu;
    std::memcpy(node.raw + RegionNode::kSentinelOffset, &all_ones, 4);  // OBS 0x1aaf3
}

}  // namespace sotc
