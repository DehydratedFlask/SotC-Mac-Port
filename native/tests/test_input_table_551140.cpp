// Numeric deepen fixture for original ctor RVA 0x551140 (275-entry table).
// Expected rows come from the generated include; ELF tie lives in tools/lift/check_f551140.py.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/input_table_551140.hpp"

#define CHECK(c)                                                                         \
    do {                                                                                 \
        if (!(c)) {                                                                      \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);              \
            std::exit(1);                                                                \
        }                                                                                \
    } while (0)

using namespace sotc;

int main() {
    CHECK(kInputTable551140Count == 275);
    CHECK(kInputTable551140Count == input_table_551140_detail::kCount);
    CHECK(input_table_551140_count() == 0x113u);
    CHECK(std::strlen(input_table_551140_detail::kPayloadSha256) == 64);

    InputTable551140 table;
    std::memset(table.data(), 0xa5, sizeof(table));
    init_input_table_551140(table);

    CHECK(input_table_551140_base(table) == table.data());
    CHECK(input_table_551140_base(static_cast<const InputTable551140&>(table)) == table.data());

    for (std::size_t i = 0; i != kInputTable551140Count; ++i) {
        const auto& e = table[i];
        const auto& row = input_table_551140_detail::kRows[i];
        CHECK(e.name_a == nullptr);
        CHECK(e.name_b == nullptr);
        CHECK(e.a == row.a);
        CHECK(e.b == row.b);
        CHECK(e.c == row.c);
        CHECK(e.pad_1c == 0);
        CHECK(e.callback == nullptr);
        CHECK(e.fnptr_state == nullptr);
        CHECK(e.zero_30 == 0);
        CHECK(e.zero_38 == 0);
        CHECK(e.self_or_null == nullptr);
        CHECK(e.zero_48 == 0);
        CHECK(e.f50 == row.f50);
        CHECK(e.zero_58 == 0);
        CHECK(e.zero_5c == 0);
        CHECK(e.a <= 5);
        CHECK(e.b <= 3);
        CHECK(e.c <= 3);
        CHECK(e.f50 <= 15);
    }

    // Spot-check first / mid / last against the known private decode (numeric only).
    CHECK(table[0].a == 3 && table[0].b == 0 && table[0].c == 0 && table[0].f50 == 0);
    CHECK(table[2].a == 4 && table[2].b == 3 && table[2].c == 1);
    CHECK(table[274].a == 1 && table[274].b == 0 && table[274].c == 0 && table[274].f50 == 0);

    // Idempotent overwrite.
    std::memset(table.data(), 0x5a, sizeof(table));
    init_input_table_551140(table);
    CHECK(table[0].a == 3 && table[0].name_a == nullptr && table[0].callback == nullptr);

    std::puts("input_table_551140: all checks passed");
    return 0;
}
