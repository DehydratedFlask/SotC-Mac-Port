#include "trig_constants.hpp"

#include <bit>

#include "libm_policy.hpp"

namespace sotc {

void init_trig_constants(TrigConstants& g) noexcept {
    g.xyz_mask = {0xffffffffu, 0xffffffffu, 0xffffffffu, 0};
    g.cos_3_degrees = libm::fsin(std::bit_cast<float>(kThreeDegreesBits), 1);   // libm-sensitive
}

}  // namespace sotc
