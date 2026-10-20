#include "init_f4eba0.hpp"
namespace sotc {
void init_f4eba0(InitF4eba0& g) noexcept {
    g.ones = ~std::uint64_t{0};
    g.ones_from_nan = ~std::uint64_t{0};
    g.zero = 0;
    g.q1 = 0xffffffff00000000ull;
    g.epsilon_x4 = {0x34000000u, 0x34000000u, 0x34000000u, 0x34000000u};
    g.q2 = 0xff7fffffff7fffffull;
    g.f2_qword = 0x00000000ff7fffffull;
}
}  // namespace sotc
