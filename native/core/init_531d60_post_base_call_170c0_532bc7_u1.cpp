#include "init_531d60_post_base_call_170c0_532bc7_u1.hpp"

namespace sotc {

Init531d60PostBaseCall170c0_532bc7_U1Result init_531d60_post_base_call_170c0_532bc7_u1(
    const Init531d60PostBaseCall170c0_532bc7_U1Cells& cells) noexcept {
    Init531d60PostBaseCall170c0_532bc7_U1Result out{};

    if (cells.alloc == nullptr) {
        if (cells.rdi_in != nullptr) {
            return out;
        }
        ++out.call_170c0_532bc7_count;
        out.ok = (out.call_170c0_532bc7_count == 1);
        return out;
    }

    const AllocWrapperResult freed = free_170c0(cells.rdi_in, *cells.alloc);
    if (!freed.ok) {
        return out;
    }
    ++out.call_170c0_532bc7_count;

    // Next: mov @ 0x532bcc — not modelled here.
    out.ok = (out.call_170c0_532bc7_count == 1);
    return out;
}

}  // namespace sotc
