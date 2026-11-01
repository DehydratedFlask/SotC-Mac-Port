#include "../core/vtable_1598e10_slot18_1bf710.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::Vtable1598e10Slot18_1bf710Cells;
using sotc::Vtable1598e10Slot18_1bf710Hooks;
using sotc::vtable_1598e10_slot18_1bf710;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    std::vector<void*> pre_calls;
    std::vector<std::pair<void*, std::uint32_t>> frees;
};

Trace g_trace;

void hook_pre(void* self) { g_trace.pre_calls.push_back(self); }

bool deallocate(void* pointer, std::uint32_t tag) {
    g_trace.frees.emplace_back(pointer, tag);
    return true;
}

bool deallocate_fail(void*, std::uint32_t) { return false; }

void test_constants() {
    check("rva", sotc::kVtable1598e10Slot18_1bf710Rva == 0x1bf710);
    check("size", sotc::kVtable1598e10Slot18_1bf710Size == 0x1c);
    check("end", sotc::kVtable1598e10Slot18_1bf710EndAddress == 0x1bf72c);
    check("vtable", sotc::kVtable1598e10Slot18_1bf710VtableAddress == 0x1598e10);
    check("slot", sotc::kVtable1598e10Slot18_1bf710SlotOffset == 0x18);
    check("reloc", sotc::kVtable1598e10Slot18_1bf710RelocAddress == 0x1598e28);
    check("pre", sotc::kVtable1598e10Slot18_1bf710PreCallRva == 0x1c0010);
    check("free", sotc::kVtable1598e10Slot18_1bf710FreeTargetRva == 0x170b0);
}

void test_full_path() {
    g_trace = Trace{};
    int self = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    Vtable1598e10Slot18_1bf710Cells cells{};
    cells.self = &self;
    Vtable1598e10Slot18_1bf710Hooks hooks{};
    hooks.pre_call_1c0010 = hook_pre;
    hooks.alloc = &contract;
    const auto result = vtable_1598e10_slot18_1bf710(cells, hooks);
    check("full ok", result.ok);
    check("full pre", result.pre_call_modelled && g_trace.pre_calls.size() == 1 &&
                         g_trace.pre_calls[0] == &self);
    check("full free", result.freed && g_trace.frees.size() == 1 &&
                           g_trace.frees[0].first == &self && g_trace.frees[0].second == 1);
    check("full tag", result.free_tag == 1);
}

void test_null_object_without_contract() {
    g_trace = Trace{};
    Vtable1598e10Slot18_1bf710Hooks hooks{};
    hooks.pre_call_1c0010 = hook_pre;
    const auto result = vtable_1598e10_slot18_1bf710({}, hooks);
    check("null ok", result.ok);
    check("null pre", result.pre_call_modelled);
    check("null freed", result.freed);
    check("null no dispatch", g_trace.frees.empty());
}

void test_nonnull_without_contract_fails() {
    int self = 0;
    Vtable1598e10Slot18_1bf710Cells cells{};
    cells.self = &self;
    const auto result = vtable_1598e10_slot18_1bf710(cells);
    check("no contract fails", !result.ok);
    check("no contract not freed", !result.freed);
}

void test_free_failure_propagates() {
    int self = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate_fail;
    Vtable1598e10Slot18_1bf710Cells cells{};
    cells.self = &self;
    Vtable1598e10Slot18_1bf710Hooks hooks{};
    hooks.alloc = &contract;
    const auto result = vtable_1598e10_slot18_1bf710(cells, hooks);
    check("fail not ok", !result.ok);
    check("fail not freed", !result.freed);
    check("fail pre counted", result.pre_call_modelled);
}

}  // namespace

int main() {
    test_constants();
    test_full_path();
    test_null_object_without_contract();
    test_nonnull_without_contract_fails();
    test_free_failure_propagates();

    if (failures == 0) {
        std::printf("OK test_vtable_1598e10_slot18_1bf710\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_1598e10_slot18_1bf710: %d failure(s)\n", failures);
    return 1;
}
