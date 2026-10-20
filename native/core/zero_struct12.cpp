#include "zero_struct12.hpp"

#include <cstring>

namespace sotc {

void zero_struct12(Bytes12& cell) noexcept {
    std::memset(cell.data(), 0, cell.size());
}

}  // namespace sotc
