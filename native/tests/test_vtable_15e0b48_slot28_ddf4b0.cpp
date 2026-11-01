#include "../core/vtable_15e0b48_slot28_ddf4b0.hpp"

#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::Vtable15e0b48Slot28_ddf4b0Cells;
using sotc::Vtable15e0b48Slot28_ddf4b0Hooks;
using sotc::vtable_15e0b48_slot28_ddf4b0;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    std::vector<void*> pre_calls;
    std::vector<std::pair<void*, void*>> tails;
    int manager_fetches = 0;
};

Trace g_trace;
int g_manager_token = 0;
bool g_manager_null = false;

void hook_pre(void* self) { g_trace.pre_calls.push_back(self); }
void* fetch_manager() {
    ++g_trace.manager_fetches;
    return g_manager_null ? nullptr : &g_manager_token;
}
void hook_tail(void* manager, void* self) { g_trace.tails.emplace_back(manager, self); }

void test_constants() {
    check("rva", sotc::kVtable15e0b48Slot28_ddf4b0Rva == 0xddf4b0);
    check("size", sotc::kVtable15e0b48Slot28_ddf4b0Size == 0x34);
    check("vtable", sotc::kVtable15e0b48Slot28_ddf4b0VtableAddress == 0x15e0b48);
    check("slot", sotc::kVtable15e0b48Slot28_ddf4b0SlotOffset == 0x28);
    check("reloc", sotc::kVtable15e0b48Slot28_ddf4b0RelocAddress == 0x15e0b70);
    check("pre", sotc::kVtable15e0b48Slot28_ddf4b0PreCallRva == 0xddf310);
    check("manager", sotc::kVtable15e0b48Slot28_ddf4b0ManagerFetchRva == 0xb2f940);
    check("manager slot", sotc::kVtable15e0b48Slot28_ddf4b0ManagerSlot == 0x18);
}

void test_full_path() {
    g_trace = Trace{};
    g_manager_null = false;
    int self = 0;
    Vtable15e0b48Slot28_ddf4b0Cells cells{};
    cells.self = &self;
    Vtable15e0b48Slot28_ddf4b0Hooks hooks{};
    hooks.pre_call_ddf310 = hook_pre;
    hooks.fetch_manager = fetch_manager;
    hooks.tail_vcall18 = hook_tail;
    const auto result = vtable_15e0b48_slot28_ddf4b0(cells, hooks);
    check("full ok", result.ok);
    check("full pre", result.pre_call_modelled && g_trace.pre_calls.size() == 1 &&
                         g_trace.pre_calls[0] == &self);
    check("full manager", result.manager == &g_manager_token && g_trace.manager_fetches == 1);
    check("full tail", result.tail_modelled && g_trace.tails.size() == 1 &&
                           g_trace.tails[0].first == &g_manager_token &&
                           g_trace.tails[0].second == &self);
    check("full not early", !result.early_return);
}

void test_null_self_early_return() {
    g_trace = Trace{};
    g_manager_null = false;
    Vtable15e0b48Slot28_ddf4b0Hooks hooks{};
    hooks.pre_call_ddf310 = hook_pre;
    hooks.fetch_manager = fetch_manager;
    hooks.tail_vcall18 = hook_tail;
    const auto result = vtable_15e0b48_slot28_ddf4b0({}, hooks);
    check("null ok", result.ok);
    check("null early", result.early_return);
    check("null no manager fetch", g_trace.manager_fetches == 0);
    check("null no tail", g_trace.tails.empty() && !result.tail_modelled);
    check("null pre ran", g_trace.pre_calls.size() == 1);
}

void test_missing_manager_hook_fails() {
    g_trace = Trace{};
    int self = 0;
    Vtable15e0b48Slot28_ddf4b0Cells cells{};
    cells.self = &self;
    Vtable15e0b48Slot28_ddf4b0Hooks hooks{};
    hooks.pre_call_ddf310 = hook_pre;
    const auto result = vtable_15e0b48_slot28_ddf4b0(cells, hooks);
    check("no manager hook fails", !result.ok);
    check("no manager hook no tail", !result.tail_modelled && g_trace.tails.empty());
}

void test_null_manager_fails() {
    g_trace = Trace{};
    g_manager_null = true;
    int self = 0;
    Vtable15e0b48Slot28_ddf4b0Cells cells{};
    cells.self = &self;
    Vtable15e0b48Slot28_ddf4b0Hooks hooks{};
    hooks.fetch_manager = fetch_manager;
    hooks.tail_vcall18 = hook_tail;
    const auto result = vtable_15e0b48_slot28_ddf4b0(cells, hooks);
    check("null manager fails", !result.ok && result.manager == nullptr);
    check("null manager no tail", g_trace.tails.empty());
    g_manager_null = false;
}

void test_tail_without_hook_is_counted() {
    g_trace = Trace{};
    g_manager_null = false;
    int self = 0;
    Vtable15e0b48Slot28_ddf4b0Cells cells{};
    cells.self = &self;
    Vtable15e0b48Slot28_ddf4b0Hooks hooks{};
    hooks.fetch_manager = fetch_manager;
    const auto result = vtable_15e0b48_slot28_ddf4b0(cells, hooks);
    check("no tail hook ok", result.ok);
    check("no tail hook counted", result.tail_modelled && g_trace.tails.empty());
}

}  // namespace

int main() {
    test_constants();
    test_full_path();
    test_null_self_early_return();
    test_missing_manager_hook_fails();
    test_null_manager_fails();
    test_tail_without_hook_is_counted();

    if (failures == 0) {
        std::printf("OK test_vtable_15e0b48_slot28_ddf4b0\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15e0b48_slot28_ddf4b0: %d failure(s)\n", failures);
    return 1;
}
