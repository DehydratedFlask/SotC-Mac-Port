// Native recovery of original ctor RVA 0xf4b540 (index 6; ELF sha256 d0723afb...5861d3).
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace sotc {
inline constexpr std::size_t kInitF4b540Bytes = 240;
using InitF4b540 = std::array<std::uint8_t, kInitF4b540Bytes>;
void init_f4b540(InitF4b540& g) noexcept;
}  // namespace sotc
