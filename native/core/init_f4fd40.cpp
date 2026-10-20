#include "init_f4fd40.hpp"
namespace sotc {
void init_f4fd40(InitF4fd40& g) noexcept {
    g.ones0 = ~std::uint64_t{0};
    g.ones1 = ~std::uint64_t{0};
    g.epsilon_x4 = {kEpsilonBits, kEpsilonBits, kEpsilonBits, kEpsilonBits};
}
}  // namespace sotc
