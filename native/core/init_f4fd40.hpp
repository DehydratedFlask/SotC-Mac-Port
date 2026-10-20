// Native recovery of original ctor RVA 0xf4fd40 (index 4; ELF sha256 d0723afb...5861d3), task CPU-01e.
#pragma once
#include <array>
#include <cstdint>
namespace sotc {
inline constexpr std::uint32_t kEpsilonBits = 0x34000000u;
struct InitF4fd40 {
    std::uint64_t ones0 = 0;
    std::uint64_t ones1 = 0;
    std::array<std::uint32_t, 4> epsilon_x4{};
};
void init_f4fd40(InitF4fd40& g) noexcept;
}  // namespace sotc
