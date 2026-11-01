#include "init_531d60_post_base_call_170b0_532bf3_u1.hpp"

namespace sotc {

Init531d60PostBaseCall170b0_532bf3_U1Result init_531d60_post_base_call_170b0_532bf3_u1(
    const Init531d60PostBaseCall170b0_532bf3_U1Cells& cells) noexcept {
    Init531d60PostBaseCall170b0_532bf3_U1Result out{};

    if (cells.alloc == nullptr) {
        if (cells.rdi_in != nullptr) {
            return out;
        }
        ++out.call_170b0_532bf3_count;
        out.ok = (out.call_170b0_532bf3_count == 1);
        return out;
    }

    const AllocWrapperResult freed = free_170b0(cells.rdi_in, *cells.alloc);
    if (!freed.ok) {
        return out;
    }
    ++out.call_170b0_532bf3_count;

    // Next: tail @ 0x532bf8 — not modelled here.
    out.ok = (out.call_170b0_532bf3_count == 1);
    return out;
}

}  // namespace sotc
