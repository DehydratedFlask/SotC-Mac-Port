#include "zero_xmm16.hpp"

namespace sotc {

void zero_xmm16(Xmm16& cell) noexcept {
    cell = {0, 0, 0, 0};
}

}  // namespace sotc
