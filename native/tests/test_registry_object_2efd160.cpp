// Prologue-only deepen of registry ctor 0x52a220 (object behind global 0x2efd160).
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/input_table_551140.hpp"
#include "core/registry_object_2efd160.hpp"

#define CHECK(c)                                                                         \
    do {                                                                                 \
        if (!(c)) {                                                                      \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);              \
            std::exit(1);                                                                \
        }                                                                                \
    } while (0)

int main() {
    using namespace sotc;
    CHECK(sizeof(RegistryObject2efd160) == 0x98);
    CHECK(kRegistryObjectFloat5 == 0x40a00000u);
    CHECK(kRegistryObjectFloat60 == 0x42700000u);

    RegistryObject2efd160 obj;
    std::memset(&obj, 0xa5, sizeof(obj));
    InputTable551140 table{};
    init_registry_object_header_52a220(obj, table.data());

    CHECK(obj.table_a_begin == nullptr);
    CHECK(obj.table_a_end == nullptr);
    CHECK(obj.table_a_cap == nullptr);
    CHECK(obj.table_b_begin == nullptr);
    CHECK(obj.table_b_end == nullptr);
    CHECK(obj.table_b_cap == nullptr);
    CHECK(obj.listeners_begin == nullptr);
    CHECK(obj.listeners_end == nullptr);
    CHECK(obj.listeners_cap == nullptr);
    CHECK(obj.input_table == table.data());
    CHECK(obj.float_5 == kRegistryObjectFloat5);
    CHECK(obj.float_60 == kRegistryObjectFloat60);

    // Prologue does not write pad_68 / pad_70; poison must remain.
    CHECK(obj.pad_68 == 0xa5a5a5a5u);
    CHECK(obj.pad_70 == 0xa5a5a5a5a5a5a5a5ull);

    std::puts("registry_object_2efd160: all checks passed");
    return 0;
}
