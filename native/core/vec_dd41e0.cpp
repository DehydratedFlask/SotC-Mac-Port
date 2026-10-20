#include "vec_dd41e0.hpp"

namespace sotc {

void init_vec_dd41e0(VecDd41e0& g) noexcept {
    if (static_cast<std::uint8_t>(g.guard) != 0) return;
    g.lanes = {kNeg036BitsDd41, kNeg036BitsDd41, kNeg036BitsDd41, kNeg036BitsDd41};
    g.guard = 1;
}

}  // namespace sotc
