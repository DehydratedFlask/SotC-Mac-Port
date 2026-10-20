#include "allocator_region.hpp"

namespace sotc {

std::optional<AllocatorRegion> make_region_view(std::uint64_t start, std::uint64_t size) noexcept {
    if (size == 0) return std::nullopt;
    if (start + size < start) return std::nullopt;
    return AllocatorRegion{.size = size, .start = start};
}

}  // namespace sotc
