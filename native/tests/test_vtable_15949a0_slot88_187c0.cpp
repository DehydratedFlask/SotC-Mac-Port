#include "../core/vtable_15949a0_slot88_187c0.hpp"

#include <cstdio>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::Vtable15949a0Slot88_187c0Cells;
using sotc::Vtable15949a0Slot88_187c0Hooks;
using sotc::vtable_15949a0_slot88_187c0;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    std::vector<void*> pre_calls;
    std::vector<std::pair<void*, std::uint32_t>> frees;  // pointer, tag
};

Trace g_trace;

void hook_pre(void* self) { g_trace.pre_calls.push_back(self); }

bool deallocate(void* pointer, std::uint32_t tag) {
    g_trace.frees.emplace_back(pointer, tag);
    return true;
}

bool deallocate_fail(void*, std::uint32_t) { return false; }

void test_constants() {
    check("rva", sotc::kVtable15949a0Slot88_187c0Rva == 0x187c0);
    check("size", sotc::kVtable15949a0Slot88_187c0Size == 0x1c);
    check("end", sotc::kVtable15949a0Slot88_187c0EndAddress == 0x187dc);
    check("vtable", sotc::kVtable15949a0Slot88_187c0VtableAddress == 0x15949a0);
    check("slot", sotc::kVtable15949a0Slot88_187c0SlotOffset == 0x88);
    check("reloc", sotc::kVtable15949a0Slot88_187c0RelocAddress == 0x1594a28);
    check("pre", sotc::kVtable15949a0Slot88_187c0PreCallRva == 0x47620);
    check("free", sotc::kVtable15949a0Slot88_187c0FreeTargetRva == 0x170b0);
}

void test_null_object_without_contract() {
    g_trace = Trace{};
    Vtable15949a0Slot88_187c0Cells cells{};
    Vtable15949a0Slot88_187c0Hooks hooks{};
    hooks.pre_call_47620 = hook_pre;
    const auto result = vtable_15949a0_slot88_187c0(cells, hooks);
    check("null ok", result.ok);
    check("null pre counted", result.pre_call_modelled);
    check("null freed", result.freed);
    check("null tag", result.free_tag == 1);
    check("null pre hook", g_trace.pre_calls.size() == 1 && g_trace.pre_calls[0] == nullptr);
    check("null no free dispatch", g_trace.frees.empty());
}

void test_nonnull_without_contract_fails() {
    g_trace = Trace{};
    int object = 0;
    Vtable15949a0Slot88_187c0Cells cells{};
    cells.self = &object;
    const auto result = vtable_15949a0_slot88_187c0(cells);
    check("no-contract fails", !result.ok);
    check("no-contract not freed", !result.freed);
    check("no-contract tag", result.free_tag == 1);
}

void test_nonnull_with_contract_frees() {
    g_trace = Trace{};
    int object = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    Vtable15949a0Slot88_187c0Cells cells{};
    cells.self = &object;
    Vtable15949a0Slot88_187c0Hooks hooks{};
    hooks.alloc = &contract;
    hooks.pre_call_47620 = hook_pre;
    const auto result = vtable_15949a0_slot88_187c0(cells, hooks);
    check("contract ok", result.ok);
    check("contract freed", result.freed);
    check("contract pre", result.pre_call_modelled);
    check("contract one free", g_trace.frees.size() == 1);
    check("contract free pointer", !g_trace.frees.empty() && g_trace.frees[0].first == &object);
    check("contract free tag", !g_trace.frees.empty() && g_trace.frees[0].second == 1);
}

void test_order_pre_before_free() {
    g_trace = Trace{};
    int object = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    Vtable15949a0Slot88_187c0Cells cells{};
    cells.self = &object;
    Vtable15949a0Slot88_187c0Hooks hooks{};
    hooks.alloc = &contract;
    hooks.pre_call_47620 = hook_pre;
    (void)vtable_15949a0_slot88_187c0(cells, hooks);
    check("order pre ran", g_trace.pre_calls.size() == 1);
    check("order free ran", g_trace.frees.size() == 1);
    check("order pre saw object", !g_trace.pre_calls.empty() && g_trace.pre_calls[0] == &object);
}

void test_free_failure_propagates() {
    g_trace = Trace{};
    int object = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate_fail;
    Vtable15949a0Slot88_187c0Cells cells{};
    cells.self = &object;
    Vtable15949a0Slot88_187c0Hooks hooks{};
    hooks.alloc = &contract;
    const auto result = vtable_15949a0_slot88_187c0(cells, hooks);
    check("fail not ok", !result.ok);
    check("fail not freed", !result.freed);
    check("fail pre counted", result.pre_call_modelled);
}

}  // namespace

int main() {
    test_constants();
    test_null_object_without_contract();
    test_nonnull_without_contract_fails();
    test_nonnull_with_contract_frees();
    test_order_pre_before_free();
    test_free_failure_propagates();

    if (failures == 0) {
        std::printf("OK test_vtable_15949a0_slot88_187c0\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15949a0_slot88_187c0: %d failure(s)\n", failures);
    return 1;
}
