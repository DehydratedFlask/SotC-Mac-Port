// Native recovery of original ctor RVA 0xf4eba0 (index 5; ELF sha256 d0723afb...5861d3).
#pragma once
#include <array>
#include <cstdint>
namespace sotc {
struct InitF4eba0 {
    std::uint64_t ones = 0;
    std::uint64_t ones_from_nan = 0;
    std::uint64_t zero = 0;
    std::uint64_t q1 = 0;
    std::array<std::uint32_t, 4> epsilon_x4{};
    std::uint64_t q2 = 0;
    std::uint64_t f2_qword = 0;
};
void init_f4eba0(InitF4eba0& g) noexcept;
}  // namespace sotc
