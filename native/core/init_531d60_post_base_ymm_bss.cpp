#include "init_531d60_post_base_ymm_bss.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseYmmBssResult init_531d60_post_base_ymm_bss(
    const Init531d60PostBaseYmmBssCells& cells) noexcept {
    Init531d60PostBaseYmmBssResult out{};

    if (cells.src_1014a80 == nullptr || cells.dst_1604278 == nullptr ||
        cells.dst_160426c == nullptr || cells.dst_16042dc == nullptr ||
        cells.dst_1604268 == nullptr) {
        return out;
    }

    // OBS: vmovups ymm0, [0x1014a80]; vmovups [rsi=0x1604278], ymm0
    std::memcpy(cells.dst_1604278, cells.src_1014a80, kInit531d60PostBaseYmmBssLaneBytes);
    // mov qword [rsi+0x20], 0x3f800000; mov qword [rsi+0x28], 0
    std::uint64_t q_one = kInit531d60PostBaseYmmBssF32One;
    std::uint64_t q_zero = 0;
    std::memcpy(cells.dst_1604278 + 0x20, &q_one, sizeof(q_one));
    std::memcpy(cells.dst_1604278 + 0x28, &q_zero, sizeof(q_zero));
    ++out.store_count;

    // OBS: mov dword [rcx=0x160426c], bf800000; [+4]=3f800000; [+8]=3f800000
    std::uint32_t d_neg = kInit531d60PostBaseYmmBssF32NegOne;
    std::uint32_t d_one = kInit531d60PostBaseYmmBssF32One;
    std::memcpy(cells.dst_160426c + 0, &d_neg, sizeof(d_neg));
    std::memcpy(cells.dst_160426c + 4, &d_one, sizeof(d_one));
    std::memcpy(cells.dst_160426c + 8, &d_one, sizeof(d_one));
    ++out.store_count;

    // OBS: vmovups [rdi=0x16042dc], ymm0; dwords @+0x20..+0x2c
    std::memcpy(cells.dst_16042dc, cells.src_1014a80, kInit531d60PostBaseYmmBssLaneBytes);
    std::uint32_t z = 0;
    std::memcpy(cells.dst_16042dc + 0x20, &d_one, sizeof(d_one));
    std::memcpy(cells.dst_16042dc + 0x24, &z, sizeof(z));
    std::memcpy(cells.dst_16042dc + 0x28, &z, sizeof(z));
    std::memcpy(cells.dst_16042dc + 0x2c, &z, sizeof(z));
    ++out.store_count;

    // OBS: mov dword [rax=0x1604268], 0x3f800000
    std::memcpy(cells.dst_1604268, &d_one, sizeof(d_one));
    ++out.store_count;

    // Next: inter-cell ymm copies @ 0x531ece — NOT modelled here.
    // Later: call 0x26800 / 0xf590 — NOT modelled here.
    out.ok = (out.store_count == 4);
    return out;
}

}  // namespace sotc
