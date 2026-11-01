#include "../core/vtable_15d1678_slot280_a460f0.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::Vtable15d1678Slot280_a460f0Cells;
using sotc::Vtable15d1678Slot280_a460f0Hooks;
using sotc::vtable_15d1678_slot280_a460f0;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    std::vector<void*> helper_calls;
    std::vector<void*> post_calls;
    std::vector<std::pair<void*, std::uint32_t>> frees;
};

Trace g_trace;

void hook_helper(void* helper) { g_trace.helper_calls.push_back(helper); }
void hook_post(void* self) { g_trace.post_calls.push_back(self); }
bool deallocate(void* pointer, std::uint32_t tag) {
    g_trace.frees.emplace_back(pointer, tag);
    return true;
}

void test_constants() {
    check("rva", sotc::kVtable15d1678Slot280_a460f0Rva == 0xa460f0);
    check("size", sotc::kVtable15d1678Slot280_a460f0Size == 0x3e);
    check("vtable", sotc::kVtable15d1678Slot280_a460f0VtableAddress == 0x15d1678);
    check("slot", sotc::kVtable15d1678Slot280_a460f0SlotOffset == 0x280);
    check("reloc", sotc::kVtable15d1678Slot280_a460f0RelocAddress == 0x15d18f8);
    check("table", sotc::kVtable15d1678Slot280_a460f0InstalledTable == 0x15d1818);
    check("helper slot", sotc::kVtable15d1678Slot280_a460f0HelperSlot == 0x18);
    check("post", sotc::kVtable15d1678Slot280_a460f0PostCallRva == 0x20b50);
}

void test_full_path() {
    g_trace = Trace{};
    int self = 0;
    void* vtable_field = nullptr;
    int helper = 0;
    const void* helper_field = &helper;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    Vtable15d1678Slot280_a460f0Cells cells{};
    cells.self = &self;
    cells.vtable_field = &vtable_field;
    cells.helper_field = &helper_field;
    Vtable15d1678Slot280_a460f0Hooks hooks{};
    hooks.helper_vcall18 = hook_helper;
    hooks.post_call_20b50 = hook_post;
    hooks.alloc = &contract;

    const auto result = vtable_15d1678_slot280_a460f0(cells, hooks);
    check("full ok", result.ok);
    check("full vtable", vtable_field == reinterpret_cast<void*>(0x15d1818));
    check("full helper", result.helper_called && g_trace.helper_calls.size() == 1 &&
                             g_trace.helper_calls[0] == &helper);
    check("full post", result.post_called && g_trace.post_calls.size() == 1 &&
                           g_trace.post_calls[0] == &self);
    check("full free", result.freed && g_trace.frees.size() == 1 &&
                           g_trace.frees[0].first == &self && g_trace.frees[0].second == 1);
    check("full tag", result.free_tag == 1);
}

void test_null_helper_fails_after_vtable_store() {
    g_trace = Trace{};
    int self = 0;
    void* vtable_field = nullptr;
    const void* helper_field = nullptr;  // helper pointer is null
    Vtable15d1678Slot280_a460f0Cells cells{};
    cells.self = &self;
    cells.vtable_field = &vtable_field;
    cells.helper_field = &helper_field;
    const auto result = vtable_15d1678_slot280_a460f0(cells);
    check("null helper fails", !result.ok);
    check("null helper vtable still written", vtable_field == reinterpret_cast<void*>(0x15d1818));
    check("null helper no call", !result.helper_called && !result.post_called);
    check("null helper no free", !result.freed);
}

void test_missing_cells() {
    const auto result = vtable_15d1678_slot280_a460f0({});
    check("missing cells fails", !result.ok);
    check("missing cells no write", result.vtable_written == nullptr);
}

void test_without_contract_fails_free() {
    g_trace = Trace{};
    int self = 0;
    void* vtable_field = nullptr;
    int helper = 0;
    const void* helper_field = &helper;
    Vtable15d1678Slot280_a460f0Cells cells{};
    cells.self = &self;
    cells.vtable_field = &vtable_field;
    cells.helper_field = &helper_field;
    const auto result = vtable_15d1678_slot280_a460f0(cells);
    check("no contract fails", !result.ok);
    check("no contract helper ran", result.helper_called);
    check("no contract post ran", result.post_called);
    check("no contract not freed", !result.freed);
}

}  // namespace

int main() {
    test_constants();
    test_full_path();
    test_null_helper_fails_after_vtable_store();
    test_missing_cells();
    test_without_contract_fails_free();

    if (failures == 0) {
        std::printf("OK test_vtable_15d1678_slot280_a460f0\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15d1678_slot280_a460f0: %d failure(s)\n", failures);
    return 1;
}
