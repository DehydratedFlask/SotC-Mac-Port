#include "helper_bool_functor_tags.hpp"

#include <algorithm>

namespace sotc {

bool is_bool_functor_tag(std::uint64_t tag) noexcept {
    return std::binary_search(kBoolFunctorTags.begin(), kBoolFunctorTags.end(), tag);
}

}  // namespace sotc
