#include "vec_neg036.hpp"

namespace sotc {

void init_vec_neg036(VecNeg036& g) noexcept {
    if (static_cast<std::uint8_t>(g.guard) != 0) return;
    g.lanes = {kNeg036Bits, kNeg036Bits, kNeg036Bits, kNeg036Bits};
    g.guard = 1;
}

}  // namespace sotc
