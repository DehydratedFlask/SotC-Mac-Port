#include "../core/vtable_159c908_slot58_2687b0.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::Vtable159c908Slot58_2687b0Cells;
using sotc::Vtable159c908Slot58_2687b0Hooks;
using sotc::vtable_159c908_slot58_2687b0;

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
    check("rva", sotc::kVtable159c908Slot58_2687b0Rva == 0x2687b0);
    check("size", sotc::kVtable159c908Slot58_2687b0Size == 0x1c);
    check("end", sotc::kVtable159c908Slot58_2687b0EndAddress == 0x2687cc);
    check("vtable", sotc::kVtable159c908Slot58_2687b0VtableAddress == 0x159c908);
    check("slot", sotc::kVtable159c908Slot58_2687b0SlotOffset == 0x58);
    check("reloc", sotc::kVtable159c908Slot58_2687b0RelocAddress == 0x159c960);
    check("pre", sotc::kVtable159c908Slot58_2687b0PreCallRva == 0x2687d0);
    check("free", sotc::kVtable159c908Slot58_2687b0FreeTargetRva == 0x170b0);
}

void test_full_path() {
    g_trace = Trace{};
    int self = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    Vtable159c908Slot58_2687b0Cells cells{};
    cells.self = &self;
    Vtable159c908Slot58_2687b0Hooks hooks{};
    hooks.pre_call_2687d0 = hook_pre;
    hooks.alloc = &contract;
    const auto result = vtable_159c908_slot58_2687b0(cells, hooks);
    check("full ok", result.ok);
    check("full pre", result.pre_call_modelled && g_trace.pre_calls.size() == 1 &&
                         g_trace.pre_calls[0] == &self);
    check("full free", result.freed && g_trace.frees.size() == 1 &&
                           g_trace.frees[0].first == &self && g_trace.frees[0].second == 1);
    check("full tag", result.free_tag == 1);
}

void test_null_object_without_contract() {
    g_trace = Trace{};
    Vtable159c908Slot58_2687b0Hooks hooks{};
    hooks.pre_call_2687d0 = hook_pre;
    const auto result = vtable_159c908_slot58_2687b0({}, hooks);
    check("null ok", result.ok);
    check("null pre", result.pre_call_modelled);
    check("null freed", result.freed);
    check("null no dispatch", g_trace.frees.empty());
}

void test_nonnull_without_contract_fails() {
    int self = 0;
    Vtable159c908Slot58_2687b0Cells cells{};
    cells.self = &self;
    const auto result = vtable_159c908_slot58_2687b0(cells);
    check("no contract fails", !result.ok);
    check("no contract not freed", !result.freed);
}

void test_free_failure_propagates() {
    int self = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate_fail;
    Vtable159c908Slot58_2687b0Cells cells{};
    cells.self = &self;
    Vtable159c908Slot58_2687b0Hooks hooks{};
    hooks.alloc = &contract;
    const auto result = vtable_159c908_slot58_2687b0(cells, hooks);
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
        std::printf("OK test_vtable_159c908_slot58_2687b0\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_159c908_slot58_2687b0: %d failure(s)\n", failures);
    return 1;
}
