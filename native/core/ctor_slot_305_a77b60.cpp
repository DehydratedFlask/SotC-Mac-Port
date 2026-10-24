#include "ctor_slot_305_a77b60.hpp"

namespace sotc {
namespace {

const Pair128& static_pair_32fb1a8() noexcept {
    static const Pair128 value = pair_zero_51230();
    return value;
}

}  // namespace

void init_slot305_a77b60(Slot305Pairs& pairs) noexcept {
    pairs[0] = static_pair_a77960();
    pairs[1] = static_pair_a77960();
    pairs[2] = static_pair_a77960();
    pairs[3] = static_pair_32fb1a8();
    pairs[4] = static_pair_32fb1a8();
    pairs[5] = static_pair_32fb1a8();
}

}  // namespace sotc
