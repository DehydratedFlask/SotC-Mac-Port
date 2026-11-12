// CPU-01-HELPER-CALLEE-MODELS-04: fixture for the 48 six byte getters / 24 hash records of startup slot 498.
// ELF pins: tools/lift/check_helper_498.py. FIXTURE_VERIFIED.
#include <set>

#include "core/helper_498_member_hashes.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    CHECK(kMemberHashRecordCount498 == 24 && kMemberHashRecordStride498 == 12 && kMemberHashCellsBytes498 == 0x120);
    CHECK(kMemberHashRecordCount498 * kMemberHashRecordStride498 == kMemberHashCellsBytes498);

    std::set<std::uint32_t> getters;
    for (const MemberHashRecord498& r : kMemberHashRecords498) {
        CHECK(r.hi_getter_rva == r.lo_getter_rva + kMemberHashGetterSpacing498);
        CHECK(getters.insert(r.lo_getter_rva).second && getters.insert(r.hi_getter_rva).second);   // 48 distinct getters
        std::uint32_t v = 0;
        CHECK(member_hash_getter_498(r.lo_getter_rva, v) && v == r.lo);
        CHECK(member_hash_getter_498(r.hi_getter_rva, v) && v == r.hi);
    }
    CHECK(getters.size() == 48);

    // unknown rvas are refused and leave the output alone
    std::uint32_t v = 0xdeadbeef;
    CHECK(!member_hash_getter_498(0x249bf8, v) && !member_hash_getter_498(0x252ae0, v) && v == 0xdeadbeef);

    // spot rows (first, a repeated-lo row, the 0x2 size row, last)
    CHECK(kMemberHashRecords498[0].lo_getter_rva == 0x3295d0 && kMemberHashRecords498[0].lo == 0x3afc714u &&
          kMemberHashRecords498[0].hi == 0x30e40edau && kMemberHashRecords498[0].size == 0x28);
    CHECK(kMemberHashRecords498[3].lo == 0xaadb73f3u && kMemberHashRecords498[10].lo == 0xaadb73f3u);
    CHECK(kMemberHashRecords498[23].lo_getter_rva == 0x2a2c60 && kMemberHashRecords498[23].size == 2);

    // the block: every byte is written, record i sits at 12 * i
    std::uint8_t cells[kMemberHashCellsBytes498];
    std::memset(cells, kPoison, sizeof cells);
    fill_member_hash_cells_498(cells);
    for (std::size_t i = 0; i < kMemberHashRecordCount498; ++i) {
        std::uint32_t d[3];
        std::memcpy(d, cells + 12 * i, 12);
        CHECK(d[0] == kMemberHashRecords498[i].lo && d[1] == kMemberHashRecords498[i].hi && d[2] == kMemberHashRecords498[i].size);
    }
    // writes stop at the block: a canary after it survives
    struct { std::uint8_t cells[kMemberHashCellsBytes498]; std::uint32_t canary; } blk{};
    blk.canary = 0xcafef00du;
    fill_member_hash_cells_498(blk.cells);
    CHECK(blk.canary == 0xcafef00du);
    std::puts("OK test_helper_498");
    return 0;
}
