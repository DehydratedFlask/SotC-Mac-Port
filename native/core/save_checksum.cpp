#include "save_checksum.hpp"

namespace sotc {

std::uint32_t fnv1a32(std::span<const std::byte> bytes, std::uint32_t seed) noexcept {
    std::uint32_t hash = seed;
    for (const std::byte byte : bytes) {
        hash ^= std::to_integer<std::uint32_t>(byte);
        hash *= 0x01000193u;
    }
    return hash;
}

}  // namespace sotc
