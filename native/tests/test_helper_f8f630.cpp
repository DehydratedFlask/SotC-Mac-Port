// CPU-01-HELPER-CALLEE-MODELS-03: fixture for the libc strncpy import thunk 0xf8f630 and slot 503's NULL-name cell.
// ELF pins: tools/lift/check_helper_f8f630.py. FIXTURE_VERIFIED.
#include "core/helper_f8f630_strncpy.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    CHECK(kStrncpyPltRva == 0xf8f630 && kStrncpyGotRva == 0x1600108);

    // short source: copy, then pad with NULs up to exactly n
    {
        char buf[12];
        std::memset(buf, 0x7f, sizeof buf);
        char* r = strncpy_f8f630(buf, "ab", 8);
        CHECK(r == buf);
        CHECK(buf[0] == 'a' && buf[1] == 'b');
        for (int i = 2; i < 8; ++i) CHECK(buf[i] == 0);
        for (int i = 8; i < 12; ++i) CHECK(buf[i] == 0x7f);     // nothing past n
    }
    // source exactly n long: NO terminator is written
    {
        char buf[6];
        std::memset(buf, 0x7f, sizeof buf);
        (void)strncpy_f8f630(buf, "abcd", 4);
        CHECK(std::memcmp(buf, "abcd", 4) == 0 && buf[4] == 0x7f && buf[5] == 0x7f);
    }
    // source longer than n: truncated, not terminated
    {
        char buf[6];
        std::memset(buf, 0x7f, sizeof buf);
        (void)strncpy_f8f630(buf, "abcdefgh", 3);
        CHECK(std::memcmp(buf, "abc", 3) == 0 && buf[3] == 0x7f);
    }
    // n == 0 writes nothing and does not read the source
    {
        char buf[2] = {0x7f, 0x7f};
        CHECK(strncpy_f8f630(buf, nullptr, 0) == buf);
        CHECK(buf[0] == 0x7f && buf[1] == 0x7f);
    }
    // empty source: n NULs
    {
        char buf[5];
        std::memset(buf, 0x7f, sizeof buf);
        (void)strncpy_f8f630(buf, "", 4);
        for (int i = 0; i < 4; ++i) CHECK(buf[i] == 0);
        CHECK(buf[4] == 0x7f);
    }
    // an embedded NUL ends the copy
    {
        char buf[6];
        std::memset(buf, 0x7f, sizeof buf);
        (void)strncpy_f8f630(buf, "a\0bcd", 5);
        CHECK(buf[0] == 'a' && buf[1] == 0 && buf[2] == 0 && buf[3] == 0 && buf[4] == 0 && buf[5] == 0x7f);
    }

    // slot 503's cell: id 0, "NULL" then NULs through offset 0x13
    CHECK(kNullNameCellCopyLength == 15 && kNullNameCellTerminatorOffset == 0x13);
    {
        NullNameCell247f47 cell;
        poison(cell);
        store_null_name_cell_247f47(cell);
        CHECK(cell.id == 0);
        CHECK(std::memcmp(cell.name, "NULL", 4) == 0);
        for (int i = 4; i < 16; ++i) CHECK(cell.name[i] == 0);
        const auto* bytes = reinterpret_cast<const std::uint8_t*>(&cell);
        CHECK(bytes[0x13] == 0 && bytes[4] == 'N' && bytes[7] == 'L');
    }
    std::puts("OK test_helper_f8f630");
    return 0;
}
