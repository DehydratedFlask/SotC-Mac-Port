#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace sotc {

[[nodiscard]] std::uint32_t fnv1a32(std::span<const std::byte> bytes, std::uint32_t seed) noexcept;

}  // namespace sotc
