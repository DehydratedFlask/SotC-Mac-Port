// CPU-01-HELPER-CALLEE-MODELS-04: fixture for the 27 byte leaf 0x1f220. ELF pins: tools/lift/check_helper_1f220.py. FIXTURE_VERIFIED.
#include <vector>

#include "core/helper_1f220_ring_header.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    CHECK(kRingHeader1f220Rva == 0x1f220 && kRingHeader1f220Size == 27);
    CHECK(kRingHeader1f220StorageOffset == 0x28 && kRingHeader1f220EndOffset == 0x2628);

    std::vector<std::uint8_t> block(0x2640, kPoison);
    ring_header_init_1f220(block.data());
    auto qword = [&](std::size_t off) { std::uint8_t* p; std::memcpy(&p, block.data() + off, sizeof p); return p; };
    CHECK(qword(0x00) == block.data() + 0x28);
    CHECK(qword(0x08) == block.data() + 0x28);
    CHECK(qword(0x20) == block.data() + 0x28);
    CHECK(qword(0x10) == block.data() + 0x2628);

    // byte footprint: exactly the four qwords, everything else (including +0x18 and the storage area) keeps the poison
    for (std::size_t i = 0; i < block.size(); ++i) {
        const bool written = i < 0x18 || (i >= 0x20 && i < 0x28);
        if (!written) CHECK(block[i] == kPoison);
    }
    for (std::size_t i = 0x18; i < 0x20; ++i) CHECK(block[i] == kPoison);

    // idempotent, and a pre-existing non-poison state is simply overwritten
    std::vector<std::uint8_t> other(0x2640, 0);
    ring_header_init_1f220(other.data());
    ring_header_init_1f220(other.data());
    std::uint8_t* p;
    std::memcpy(&p, other.data() + 0x10, sizeof p);
    CHECK(p == other.data() + 0x2628);
    std::puts("OK test_helper_1f220");
    return 0;
}
