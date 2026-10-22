#include "hashed_name_ctors.hpp"

#include "fnv_cstr_161fb0.hpp"

namespace sotc {
namespace {
template <std::size_t N>
void fill(std::array<NameHashCell, N>& cells, const std::array<const char*, N>& names) noexcept {
    // Record order = original order: pointer store, helper call, hash store.
    for (std::size_t k = 0; k != N; ++k) {
        cells[k].name = names[k];
        cells[k].hash = fnv_cstr_52a990(names[k]);
    }
}
}  // namespace

void init_ctor_669450(std::array<NameHashCell, 1>& cells, const std::array<const char*, 1>& names) noexcept { fill(cells, names); }
void init_ctor_842450(std::array<NameHashCell, 3>& cells, const std::array<const char*, 3>& names) noexcept { fill(cells, names); }
void init_ctor_7dace0(std::array<NameHashCell, 2>& cells, const std::array<const char*, 2>& names) noexcept { fill(cells, names); }
void init_ctor_6dde20(std::array<NameHashCell, 3>& cells, const std::array<const char*, 3>& names) noexcept { fill(cells, names); }

}  // namespace sotc
