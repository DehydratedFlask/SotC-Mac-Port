#include "free_list_carve.hpp"

namespace sotc {
namespace {

bool is_pow2_u32(std::uint32_t v) noexcept { return v != 0 && (v & (v - 1u)) == 0; }

}  // namespace

std::optional<CarveFit> try_carve_fit(std::uint64_t free_start, std::uint64_t free_size,
                                      std::uint64_t req_size,
                                      std::uint32_t alignment) noexcept {
    if (req_size == 0 || free_size < req_size) return std::nullopt;
    if (!is_pow2_u32(alignment)) return std::nullopt;

    // 0x1a83d..0x1a879: lead = align_up(free_start, alignment) - free_start
    const std::uint64_t mask = carve_align_mask(alignment);
    const std::uint64_t aligned = (free_start + (alignment - 1u)) & mask;
    if (aligned < free_start) return std::nullopt;  // overflow
    const std::uint64_t lead = aligned - free_start;

    if (lead == 0) {
        // 0x1a87f → 0x1a8a1: consume the whole free node (size stays free_size).
        return CarveFit{.lead = 0,
                        .allocated_start = free_start,
                        .allocated_size = free_size,
                        .exact_node = true};
    }

    // 0x1a881..0x1a888: need lead + req_size <= free_size
    if (lead > free_size || free_size - lead < req_size) return std::nullopt;

    // Split path 0x1a8b6..0x1a8fa: allocated gets [aligned, free_size - lead).
    return CarveFit{.lead = lead,
                    .allocated_start = aligned,
                    .allocated_size = free_size - lead,
                    .exact_node = false};
}

}  // namespace sotc
