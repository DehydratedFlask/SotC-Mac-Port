#include "init_531d60_post_base_lea_32f4fe0.hpp"

namespace sotc {

Init531d60PostBaseLea32f4fe0Result init_531d60_post_base_lea_32f4fe0(
    const Init531d60PostBaseLea32f4fe0Cells& cells) noexcept {
    Init531d60PostBaseLea32f4fe0Result out{};

    if (cells.r13_cell == nullptr || cells.r13_out == nullptr) {
        return out;
    }

    // OBS: lea r13, [0x32f4fe0] — address pin.
    *cells.r13_out = cells.r13_cell;
    ++out.lea_count;

    // Next: mov rdi,[r13] / call 0xa2caf0 @ 0x5323ec — NOT modelled here.
    out.ok = (out.lea_count == 1);
    return out;
}

}  // namespace sotc
