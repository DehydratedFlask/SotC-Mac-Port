#include "../core/vtable_15d1678_slot_68_a40e20.hpp"
#include "../core/vtable_15d1678_slot_e8_c2e0.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>

namespace {

using sotc::Vtable15d1678SlotE8_c2e0Cells;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_c2e0_copy() {
    int source_value = 11;
    const void* source_field = &source_value;
    void* copy_field = nullptr;
    Vtable15d1678SlotE8_c2e0Cells cells{};
    cells.source_field = &source_field;
    cells.copy_field = &copy_field;
    const auto result = sotc::vtable_15d1678_slot_e8_c2e0(cells);
    check("c2e0 ok", result.ok);
    check("c2e0 copied", copy_field == &source_value);
    check("c2e0 result", result.copied == &source_value);
    check("c2e0 offsets", sotc::kVtable15d1678SlotE8_c2e0SourceOffset == 0x10 &&
                              sotc::kVtable15d1678SlotE8_c2e0CopyFieldOffset == 0x10);
}

void test_c2e0_missing_cells() {
    const auto result = sotc::vtable_15d1678_slot_e8_c2e0({});
    check("c2e0 missing fails", !result.ok);
    check("c2e0 missing no copy", result.copied == nullptr);
}

void test_a40e20_noop() {
    const auto first = sotc::vtable_15d1678_slot_68_a40e20();
    const auto second = sotc::vtable_15d1678_slot_68_a40e20();
    check("a40e20 ok", first.ok && second.ok);
    check("a40e20 counts", first.calls == 1 && second.calls == 1);
    check("a40e20 size", sotc::kVtable15d1678Slot68_a40e20Size == 0x1);
}

void test_constants() {
    check("c2e0 rva", sotc::kVtable15d1678SlotE8_c2e0Rva == 0xc2e0);
    check("c2e0 slot", sotc::kVtable15d1678SlotE8_c2e0SlotOffset == 0xe8);
    check("c2e0 reloc", sotc::kVtable15d1678SlotE8_c2e0RelocAddress == 0x15d1760);
    check("a40e20 rva", sotc::kVtable15d1678Slot68_a40e20Rva == 0xa40e20);
    check("a40e20 slot", sotc::kVtable15d1678Slot68_a40e20SlotOffset == 0x68);
    check("a40e20 reloc", sotc::kVtable15d1678Slot68_a40e20RelocAddress == 0x15d16e0);
}

}  // namespace

int main() {
    test_constants();
    test_c2e0_copy();
    test_c2e0_missing_cells();
    test_a40e20_noop();

    if (failures == 0) {
        std::printf("OK test_vtable_resolved_bodies_3\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_resolved_bodies_3: %d failure(s)\n", failures);
    return 1;
}
