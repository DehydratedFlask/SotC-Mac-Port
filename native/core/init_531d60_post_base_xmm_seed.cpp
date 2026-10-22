#include "init_531d60_post_base_xmm_seed.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseXmmSeedResult init_531d60_post_base_xmm_seed(
    const Init531d60PostBaseXmmSeedCells& cells) noexcept {
    Init531d60PostBaseXmmSeedResult out{};

    if (cells.src_1014be0 == nullptr || cells.dst_16042d8 == nullptr ||
        cells.dst_160430c == nullptr) {
        return out;
    }

    // OBS: mov dword [rcx=0x16042d8], 0x3f800000
    std::uint32_t d_one = kInit531d60PostBaseXmmSeedF32One;
    std::memcpy(cells.dst_16042d8, &d_one, sizeof(d_one));
    ++out.store_count;

    // OBS: scalar fills + vmovups [rsi+0xc], xmm0 + qword @+0x1c
    std::uint32_t d_neg = kInit531d60PostBaseXmmSeedF32NegOne;
    std::uint32_t z = 0;
    std::memcpy(cells.dst_160430c + 0, &d_neg, sizeof(d_neg));
    std::memcpy(cells.dst_160430c + 4, &z, sizeof(z));
    std::memcpy(cells.dst_160430c + 8, &z, sizeof(z));
    std::memcpy(cells.dst_160430c + 0xc, cells.src_1014be0,
                kInit531d60PostBaseXmmSeedLaneBytes);
    std::uint64_t q = kInit531d60PostBaseXmmSeedQOneHi;
    std::memcpy(cells.dst_160430c + 0x1c, &q, sizeof(q));
    ++out.store_count;

    // Next: mov ecx,4 / ymm reload→0x160433c — E460; not this fragment.
    out.ok = (out.store_count == 2);
    return out;
}

}  // namespace sotc
