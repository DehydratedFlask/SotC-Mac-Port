#include "region_split.hpp"

namespace sotc {

bool split_free_at_lead(RegionNode& free, RegionNode& neu, std::uint64_t lead) noexcept {
    if (lead == 0) return false;
    const std::uint64_t free_size = free.size();
    const std::uint64_t free_start = free.start();
    if (lead >= free_size) return false;
    if (free_start + lead < free_start) return false;

    const std::uint64_t rem = free_size - lead;
    const std::uint8_t flag = free.flag_a();

    neu.set_flag_a(flag);
    neu.set_size(rem);
    neu.set_start(free_start + lead);
    neu.raw[RegionNode::kFlagBOffset] = 0;
    std::uint64_t zero = 0;
    std::memcpy(neu.raw + RegionNode::kPtrOffset, &zero, 8);
    std::uint32_t all_ones = 0xffffffffu;
    std::memcpy(neu.raw + RegionNode::kSentinelOffset, &all_ones, 4);
    neu.set_capacity(rem);

    free.set_size(lead);
    free.set_capacity(lead);
    return true;
}

std::optional<CarveFit> split_using_carve_fit(RegionNode& free, RegionNode& neu,
                                              std::uint64_t req_size,
                                              std::uint32_t alignment) noexcept {
    auto fit = try_carve_fit(free.start(), free.size(), req_size, alignment);
    if (!fit || fit->exact_node || fit->lead == 0) return std::nullopt;
    if (!split_free_at_lead(free, neu, fit->lead)) return std::nullopt;
    return fit;
}

}  // namespace sotc
