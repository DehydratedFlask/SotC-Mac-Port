#include "leaf_df94c0.hpp"

#include <bit>

#include "libm_policy.hpp"
#include "trig_constants.hpp"

namespace sotc {

void init_leaf_df94c0(LeafDf94c0& g) noexcept {
    // Same float bits as 0xe10490's input (bits 0x3d567750): _FSin(x, 1, 0) is the cosine.
    g.cos_3_degrees = libm::fsin(std::bit_cast<float>(kThreeDegreesBits), 1);   // libm-sensitive
}

}  // namespace sotc
