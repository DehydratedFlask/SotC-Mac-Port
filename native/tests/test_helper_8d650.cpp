// CPU-01-HELPER-CALLEE-MODELS-03: fixture for the 7 byte leaf 0x8d650 (`mov dword [rdi], 1 ; ret`).
// ELF pins: tools/lift/check_helper_8d650.py. FIXTURE_VERIFIED.
#include "core/helper_8d650_store_one.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    CHECK(kStoreOne8d650Rva == 0x8d650 && kStoreOne8d650Size == 7 && kStoreOne8d650Value == 1);

    // exactly the four bytes at the cell are written; the neighbouring dwords (slot 497 stores 0x780 / 0x438 there) are not
    struct Display { std::uint32_t cell; std::uint32_t w; std::uint32_t h; std::uint32_t tail; } d{0xdeadbeefu, 0x780, 0x438, 0xcafef00du};
    store_one_8d650(d.cell);
    CHECK(d.cell == 1 && d.w == 0x780 && d.h == 0x438 && d.tail == 0xcafef00du);

    // idempotent, and overwrites any previous value including zero
    store_one_8d650(d.cell);
    CHECK(d.cell == 1);
    std::uint32_t zero = 0;
    store_one_8d650(zero);
    CHECK(zero == 1);

    // byte footprint on a poisoned buffer: 4 bytes written, the rest untouched
    alignas(4) std::uint8_t buf[12];
    std::memset(buf, kPoison, sizeof buf);
    store_one_8d650(*reinterpret_cast<std::uint32_t*>(buf + 4));
    for (std::size_t i = 0; i < sizeof buf; ++i) {
        const std::uint8_t want = i == 4 ? 1 : (i >= 5 && i <= 7 ? 0 : kPoison);
        CHECK(buf[i] == want);
    }
    std::puts("OK test_helper_8d650");
    return 0;
}
