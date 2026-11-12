#include "helper_functor_tags.hpp"

#include <algorithm>

namespace sotc {

bool is_int_functor_tag(std::uint64_t tag) noexcept {
    return std::binary_search(kIntFunctorTags.begin(), kIntFunctorTags.end(), tag);
}

bool is_float_functor_tag(std::uint64_t tag) noexcept {
    return std::binary_search(kFloatFunctorTags.begin(), kFloatFunctorTags.end(), tag);
}

}  // namespace sotc
