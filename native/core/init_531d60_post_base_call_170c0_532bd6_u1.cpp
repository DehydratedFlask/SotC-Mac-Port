#include "init_531d60_post_base_call_170c0_532bd6_u1.hpp"

namespace sotc {

Init531d60PostBaseCall170c0_532bd6_U1Result init_531d60_post_base_call_170c0_532bd6_u1(
    const Init531d60PostBaseCall170c0_532bd6_U1Cells& cells) noexcept {
    Init531d60PostBaseCall170c0_532bd6_U1Result out{};

    if (cells.alloc == nullptr) {
        // No dispatch contract: model the call only, keep a non-null input fail-visible.
        if (cells.rdi_in != nullptr) {
            return out;
        }
        ++out.call_170c0_532bd6_count;
        out.ok = (out.call_170c0_532bd6_count == 1);
        return out;
    }

    const AllocWrapperResult freed = free_170c0(cells.rdi_in, *cells.alloc);
    if (!freed.ok) {
        return out;
    }
    ++out.call_170c0_532bd6_count;

    // Next: E613 mov rdi,[r12] @ 0x532bdb — separate unit.
    out.ok = (out.call_170c0_532bd6_count == 1);
    return out;
}

}  // namespace sotc
