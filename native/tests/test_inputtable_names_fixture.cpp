// Private 275-name fixture loader for input table 0x551140 / registry fill 0x52a220.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/fnv_cstr_161fb0.hpp"
#include "core/fnv_path_cstr_162020.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"

#define CHECK(c)                                                                         \
    do {                                                                                 \
        if (!(c)) {                                                                      \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);              \
            std::exit(1);                                                                \
        }                                                                                \
    } while (0)

using namespace sotc;

int main() {
    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        std::fprintf(stderr,
                     "inputtable_names_fixture: SKIP (private fixture missing — fail-visible "
                     "path exercised)\n");
        // Missing fixture must fail visibly for consumers; loader itself returns false.
        CHECK(!fix.loaded);
        // Still pin constants so the binary links the hash helpers.
        CHECK(std::strlen(kInputtableNamesHashpairSha256) == 64);
        std::puts("inputtable_names_fixture: missing-fixture path OK");
        return 0;
    }
    CHECK(fix.loaded);
    CHECK(fix.pairs[0].name_a != nullptr && fix.pairs[0].name_b != nullptr);

    // Recompute hashes and aggregate digest without printing strings.
    unsigned char digest_in[275 * 8];
    for (std::size_t i = 0; i != kInputTable551140Count; ++i) {
        const std::uint32_t ha = fnv_cstr_52a990(fix.pairs[i].name_a);
        const std::uint32_t hb = fnv_cstr_a2ddc0(fix.pairs[i].name_b);
        CHECK(ha == fix.pairs[i].hash_a);
        CHECK(hb == fix.pairs[i].hash_b);
        std::memcpy(digest_in + i * 8, &ha, 4);
        std::memcpy(digest_in + i * 8 + 4, &hb, 4);
    }

    InputTable551140 table{};
    init_input_table_551140(table);
    CHECK(apply_inputtable_names_fixture(table, fix));
    CHECK(table[0].name_a == fix.pairs[0].name_a);
    CHECK(table[274].name_b == fix.pairs[274].name_b);

    std::puts("inputtable_names_fixture: all checks passed");
    return 0;
}
