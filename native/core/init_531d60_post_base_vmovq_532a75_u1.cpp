#include "init_531d60_post_base_vmovq_532a75_u1.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseVmovq532a75_U1Result init_531d60_post_base_vmovq_532a75_u1(
    const Init531d60PostBaseVmovq532a75_U1Cells& cells) noexcept {
    Init531d60PostBaseVmovq532a75_U1Result out{};

    if (cells.src_15ff528 != nullptr && cells.dst_xmm0 != nullptr) {
        std::memcpy(cells.dst_xmm0, cells.src_15ff528, kInit531d60PostBaseVmovq532a75_U1LaneBytes);
    }
    ++out.vmovq_count;

    // Next: lea rax,[0x5388e0] @ 0x532a7d — NOT modelled here.
    out.ok = (out.vmovq_count == 1);
    return out;
}

}  // namespace sotc
