#include "../core/vtable_1594ae8_slot28_13d20.hpp"
#include "../core/vtable_15955c0_slot08_358c0.hpp"
#include "../core/vtable_15d1d80_slot08_c740.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>

namespace {

using sotc::Vtable1594ae8Slot28_13d20Cells;
using sotc::Vtable15955c0Slot08_358c0Cells;
using sotc::Vtable15d1d80Slot08_c740Cells;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_13d20_setter() {
    std::uint32_t field = 0xAABBCCDDu;
    Vtable1594ae8Slot28_13d20Cells cells{};
    cells.field = &field;
    const auto result = sotc::vtable_1594ae8_slot28_13d20(0x11223344u, cells);
    check("setter ok", result.ok);
    check("setter value", field == 0x11223344u);
    check("setter previous", result.previous == 0xAABBCCDDu);
    check("setter written", result.written == 0x11223344u);
    check("setter field offset", sotc::kVtable1594ae8Slot28_13d20FieldOffset == 0x1c);
}

void test_13d20_missing_field() {
    const auto result = sotc::vtable_1594ae8_slot28_13d20(7u);
    check("setter missing fails", !result.ok);
    check("setter missing no write", result.written == 0);
}

void test_358c0_shape() {
    void* table_field = nullptr;
    int source_value = 5;
    const void* source_field = &source_value;
    void* copy_field = nullptr;
    Vtable15955c0Slot08_358c0Cells cells{};
    cells.table_field = &table_field;
    cells.source_field = &source_field;
    cells.copy_field = &copy_field;
    const auto result = sotc::vtable_15955c0_slot08_358c0(cells);
    check("358c0 ok", result.ok);
    check("358c0 table", table_field == reinterpret_cast<void*>(0x15955c0));
    check("358c0 copied", copy_field == &source_value);
    check("358c0 result table", result.table_written == reinterpret_cast<void*>(0x15955c0));
    check("358c0 result copy", result.copied == &source_value);
}

void test_358c0_missing_cells() {
    void* table_field = nullptr;
    Vtable15955c0Slot08_358c0Cells cells{};
    cells.table_field = &table_field;
    const auto result = sotc::vtable_15955c0_slot08_358c0(cells);
    check("358c0 missing fails", !result.ok);
    check("358c0 missing no write", table_field == nullptr);
}

void test_c740_shape() {
    void* table_field = nullptr;
    int source_value = 9;
    const void* source_field = &source_value;
    void* copy_field = nullptr;
    Vtable15d1d80Slot08_c740Cells cells{};
    cells.table_field = &table_field;
    cells.source_field = &source_field;
    cells.copy_field = &copy_field;
    const auto result = sotc::vtable_15d1d80_slot08_c740(cells);
    check("c740 ok", result.ok);
    check("c740 table", table_field == reinterpret_cast<void*>(0x15944a0));
    check("c740 copied", copy_field == &source_value);
    check("c740 result table", result.table_written == reinterpret_cast<void*>(0x15944a0));
}

void test_c740_missing_cells() {
    const auto result = sotc::vtable_15d1d80_slot08_c740({});
    check("c740 missing fails", !result.ok);
}

void test_constants() {
    check("13d20 rva", sotc::kVtable1594ae8Slot28_13d20Rva == 0x13d20);
    check("13d20 slot", sotc::kVtable1594ae8Slot28_13d20SlotOffset == 0x28);
    check("358c0 rva", sotc::kVtable15955c0Slot08_358c0Rva == 0x358c0);
    check("358c0 installed", sotc::kVtable15955c0Slot08_358c0InstalledTable == 0x15955c0);
    check("c740 rva", sotc::kVtable15d1d80Slot08_c740Rva == 0xc740);
    check("c740 installed", sotc::kVtable15d1d80Slot08_c740InstalledTable == 0x15944a0);
}

}  // namespace

int main() {
    test_constants();
    test_13d20_setter();
    test_13d20_missing_field();
    test_358c0_shape();
    test_358c0_missing_cells();
    test_c740_shape();
    test_c740_missing_cells();

    if (failures == 0) {
        std::printf("OK test_vtable_resolved_bodies_2\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_resolved_bodies_2: %d failure(s)\n", failures);
    return 1;
}
