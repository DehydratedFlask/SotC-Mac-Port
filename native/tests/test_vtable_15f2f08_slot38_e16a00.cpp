#include "../core/vtable_15f2f08_slot38_e16a00.hpp"

#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::Vtable15f2f08Slot38_e16a00Cells;
using sotc::Vtable15f2f08Slot38_e16a00Hooks;
using sotc::vtable_15f2f08_slot38_e16a00;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    std::vector<std::string> order;
    std::vector<std::pair<void*, void*>> args;
};

Trace g_trace;

void hook_first(void* self, void* argument) {
    g_trace.order.emplace_back("slot18");
    g_trace.args.emplace_back(self, argument);
}
void hook_tail(void* self, void* argument) {
    g_trace.order.emplace_back("slot20");
    g_trace.args.emplace_back(self, argument);
}

void test_constants() {
    check("rva", sotc::kVtable15f2f08Slot38_e16a00Rva == 0xe16a00);
    check("size", sotc::kVtable15f2f08Slot38_e16a00Size == 0x26);
    check("vtable", sotc::kVtable15f2f08Slot38_e16a00VtableAddress == 0x15f2f08);
    check("slot", sotc::kVtable15f2f08Slot38_e16a00SlotOffset == 0x38);
    check("reloc", sotc::kVtable15f2f08Slot38_e16a00RelocAddress == 0x15f2f40);
    check("first slot", sotc::kVtable15f2f08Slot38_e16a00FirstCallSlot == 0x18);
    check("tail slot", sotc::kVtable15f2f08Slot38_e16a00TailCallSlot == 0x20);
}

void test_full_path() {
    g_trace = Trace{};
    int self = 0;
    int argument = 0;
    const void* vtable = reinterpret_cast<const void*>(0x15f0000);
    const void* vtable_field = vtable;
    Vtable15f2f08Slot38_e16a00Cells cells{};
    cells.self = &self;
    cells.vtable_field = &vtable_field;
    cells.argument = &argument;
    Vtable15f2f08Slot38_e16a00Hooks hooks{};
    hooks.call_slot18 = hook_first;
    hooks.tail_slot20 = hook_tail;
    const auto result = vtable_15f2f08_slot38_e16a00(cells, hooks);
    check("full ok", result.ok);
    check("full vtable", result.vtable == vtable);
    check("full order", g_trace.order.size() == 2 && g_trace.order[0] == "slot18" &&
                            g_trace.order[1] == "slot20");
    check("full args", g_trace.args.size() == 2 && g_trace.args[0].first == &self &&
                           g_trace.args[0].second == &argument &&
                           g_trace.args[1].first == &self &&
                           g_trace.args[1].second == &argument);
    check("full flags", result.first_call_modelled && result.tail_modelled);
}

void test_null_self_fails() {
    g_trace = Trace{};
    const void* vtable_field = reinterpret_cast<const void*>(0x15f0000);
    Vtable15f2f08Slot38_e16a00Cells cells{};
    cells.vtable_field = &vtable_field;
    const auto result = vtable_15f2f08_slot38_e16a00(cells);
    check("null self fails", !result.ok);
    check("null self no calls", g_trace.order.empty() && !result.first_call_modelled);
}

void test_null_vtable_fails() {
    g_trace = Trace{};
    int self = 0;
    const void* vtable_field = nullptr;
    Vtable15f2f08Slot38_e16a00Cells cells{};
    cells.self = &self;
    cells.vtable_field = &vtable_field;
    Vtable15f2f08Slot38_e16a00Hooks hooks{};
    hooks.call_slot18 = hook_first;
    hooks.tail_slot20 = hook_tail;
    const auto result = vtable_15f2f08_slot38_e16a00(cells, hooks);
    check("null vtable fails", !result.ok);
    check("null vtable no calls", g_trace.order.empty());
}

void test_without_hooks_counts() {
    g_trace = Trace{};
    int self = 0;
    const void* vtable_field = reinterpret_cast<const void*>(0x15f0000);
    Vtable15f2f08Slot38_e16a00Cells cells{};
    cells.self = &self;
    cells.vtable_field = &vtable_field;
    const auto result = vtable_15f2f08_slot38_e16a00(cells);
    check("no hooks ok", result.ok);
    check("no hooks counted", result.first_call_modelled && result.tail_modelled);
    check("no hooks no trace", g_trace.order.empty());
}

void test_missing_cells() {
    const auto result = vtable_15f2f08_slot38_e16a00({});
    check("missing cells fails", !result.ok);
}

}  // namespace

int main() {
    test_constants();
    test_full_path();
    test_null_self_fails();
    test_null_vtable_fails();
    test_without_hooks_counts();
    test_missing_cells();

    if (failures == 0) {
        std::printf("OK test_vtable_15f2f08_slot38_e16a00\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15f2f08_slot38_e16a00: %d failure(s)\n", failures);
    return 1;
}
