#include "../core/vtable_15e2028_slot70_e1a5c0.hpp"

#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::Vtable15e2028Slot70_e1a5c0Cells;
using sotc::Vtable15e2028Slot70_e1a5c0Hooks;
using sotc::vtable_15e2028_slot70_e1a5c0;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Element {
    const char* key;
    std::int32_t value;
    int match_result;
};

std::vector<int> g_match_calls;

const char* element_key(void* element) {
    return static_cast<Element*>(element)->key;
}
std::int32_t element_value(void* element) {
    return static_cast<Element*>(element)->value;
}
std::int32_t element_match(void* element) {
    g_match_calls.push_back(static_cast<Element*>(element)->value);
    return static_cast<Element*>(element)->match_result;
}

void test_constants() {
    check("rva", sotc::kVtable15e2028Slot70_e1a5c0Rva == 0xe1a5c0);
    check("size", sotc::kVtable15e2028Slot70_e1a5c0Size == 0x97);
    check("vtable", sotc::kVtable15e2028Slot70_e1a5c0VtableAddress == 0x15e2028);
    check("slot", sotc::kVtable15e2028Slot70_e1a5c0SlotOffset == 0x70);
    check("reloc", sotc::kVtable15e2028Slot70_e1a5c0RelocAddress == 0x15e2098);
    check("strcmp", sotc::kVtable15e2028Slot70_e1a5c0StrcmpTarget == 0xf8f490);
}

void test_null_key() {
    Vtable15e2028Slot70_e1a5c0Cells cells{};
    const auto result = vtable_15e2028_slot70_e1a5c0(cells);
    check("null key early", result.ok && result.early_return && result.code == 1);
    check("null key no scan", result.elements_scanned == 0);
}

void test_empty_container() {
    std::uint32_t count = 0;
    Vtable15e2028Slot70_e1a5c0Cells cells{};
    cells.count = &count;
    cells.key = "alpha";
    const auto result = vtable_15e2028_slot70_e1a5c0(cells);
    check("empty early", result.ok && result.early_return && result.code == 1);
}

void test_found_middle() {
    g_match_calls.clear();
    Element elements[] = {{"alpha", 1, 7}, {nullptr, 2, 8}, {"beta", 3, 9}};
    void* pointers[] = {&elements[0], &elements[1], &elements[2]};
    std::uint32_t count = 3;
    Vtable15e2028Slot70_e1a5c0Cells cells{};
    cells.elements = pointers;
    cells.count = &count;
    cells.key = "beta";
    cells.value = 3;
    Vtable15e2028Slot70_e1a5c0Hooks hooks{};
    hooks.element_key = element_key;
    hooks.element_value = element_value;
    hooks.element_match = element_match;
    const auto result = vtable_15e2028_slot70_e1a5c0(cells, hooks);
    check("found ok", result.ok);
    check("found index", result.found_index == 2);
    check("found scanned", result.elements_scanned == 3);
    check("found code", result.code == 9);
    check("found match called", result.match_called && g_match_calls.size() == 1 &&
                                    g_match_calls[0] == 3);
}

void test_key_mismatch_and_value_mismatch() {
    g_match_calls.clear();
    Element elements[] = {{"alpha", 1, 7}, {"beta", 3, 9}};
    void* pointers[] = {&elements[0], &elements[1]};
    std::uint32_t count = 2;
    Vtable15e2028Slot70_e1a5c0Cells cells{};
    cells.elements = pointers;
    cells.count = &count;
    cells.key = "beta";
    cells.value = 4;  // right key, wrong value
    Vtable15e2028Slot70_e1a5c0Hooks hooks{};
    hooks.element_key = element_key;
    hooks.element_value = element_value;
    hooks.element_match = element_match;
    const auto result = vtable_15e2028_slot70_e1a5c0(cells, hooks);
    check("miss code", result.ok && result.code == 1 && result.found_index == -1);
    check("miss scanned", result.elements_scanned == 2);
    check("miss no match", !result.match_called && g_match_calls.empty());
}

void test_case_sensitive() {
    Element elements[] = {{"Beta", 3, 9}};
    void* pointers[] = {&elements[0]};
    std::uint32_t count = 1;
    Vtable15e2028Slot70_e1a5c0Cells cells{};
    cells.elements = pointers;
    cells.count = &count;
    cells.key = "beta";  // strcmp, not strcasecmp
    cells.value = 3;
    Vtable15e2028Slot70_e1a5c0Hooks hooks{};
    hooks.element_key = element_key;
    hooks.element_value = element_value;
    const auto result = vtable_15e2028_slot70_e1a5c0(cells, hooks);
    check("case sensitive miss", result.ok && result.code == 1);
}

void test_missing_hooks_fail() {
    void* pointers[] = {nullptr};
    std::uint32_t count = 1;
    Vtable15e2028Slot70_e1a5c0Cells cells{};
    cells.elements = pointers;
    cells.count = &count;
    cells.key = "alpha";
    const auto result = vtable_15e2028_slot70_e1a5c0(cells);
    check("missing hooks fail", !result.ok);
}

}  // namespace

int main() {
    test_constants();
    test_null_key();
    test_empty_container();
    test_found_middle();
    test_key_mismatch_and_value_mismatch();
    test_case_sensitive();
    test_missing_hooks_fail();

    if (failures == 0) {
        std::printf("OK test_vtable_15e2028_slot70_e1a5c0\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15e2028_slot70_e1a5c0: %d failure(s)\n", failures);
    return 1;
}
