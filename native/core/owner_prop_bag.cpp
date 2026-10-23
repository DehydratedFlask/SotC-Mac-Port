#include "owner_prop_bag.hpp"

namespace sotc {

void OwnerPropBag::insert_a(std::uint32_t key, const OwnerValueA& value) {
    tree_a_.emplace(key, value);  // multimap: equal keys are kept in insertion order
}

void OwnerPropBag::insert_b(std::uint32_t key, const OwnerValueB& value) {
    tree_b_.emplace(key, value);
}

bool OwnerPropBagSink::insert_a(std::uint16_t key, std::int32_t i, float f,
                                std::uint32_t tag) {
    bag_.insert_a(key, OwnerValueA{i, f, tag});
    return true;
}

bool OwnerPropBagSink::insert_b(std::uint16_t key, float lo, float hi, float v,
                                std::uint32_t tag) {
    bag_.insert_b(key, OwnerValueB{lo, hi, v, tag});
    return true;
}

}  // namespace sotc
