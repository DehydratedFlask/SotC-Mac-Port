#include "zero_xmm_qword.hpp"

namespace sotc {

void zero_xmm_qword(Xmm16& xmm_cell, std::uint64_t& q_cell) noexcept {
    xmm_cell = {0, 0, 0, 0};
    q_cell = 0;
}

}  // namespace sotc
