#include "../core/vtable_1594dc0_slot20_20d60.hpp"

#include <cstdio>
#include <cstdint>
#include <string_view>

namespace {

using sotc::Vtable1594dc0Slot20_20d60Cells;
using sotc::Vtable1594dc0Slot20_20d60Hooks;
using sotc::vtable_1594dc0_slot20_20d60;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct HookState {
    int calls = 0;
    void* self = nullptr;
    void* global = nullptr;
};

HookState g_hook;

void hook_tail(void* self, void* global) {
    ++g_hook.calls;
    g_hook.self = self;
    g_hook.global = global;
}

void test_constants() {
    check("rva", sotc::kVtable1594dc0Slot20_20d60Rva == 0x20d60);
    check("size", sotc::kVtable1594dc0Slot20_20d60Size == 0x1a);
    check("end", sotc::kVtable1594dc0Slot20_20d60EndAddress == 0x20d7a);
    check("vtable", sotc::kVtable1594dc0Slot20_20d60VtableAddress == 0x1594dc0);
    check("slot", sotc::kVtable1594dc0Slot20_20d60SlotOffset == 0x20);
    check("reloc", sotc::kVtable1594dc0Slot20_20d60RelocAddress == 0x1594de0);
    check("flag", sotc::kVtable1594dc0Slot20_20d60OnceFlagOffset == 0x18);
    check("global", sotc::kVtable1594dc0Slot20_20d60GlobalAddress == 0x2dfcbc8);
    check("tail", sotc::kVtable1594dc0Slot20_20d60TailTargetRva == 0x20d80);
}

void test_first_call_sets_flag_and_tails() {
    std::uint8_t flag = 0;
    int global_target = 42;
    const void* global_cell = &global_target;
    Vtable1594dc0Slot20_20d60Cells cells{};
    cells.once_flag = &flag;
    cells.global_cell = &global_cell;
    Vtable1594dc0Slot20_20d60Hooks hooks{};
    hooks.tail_call = hook_tail;
    g_hook = HookState{};

    int self = 0;
    const auto result = vtable_1594dc0_slot20_20d60(&self, cells, hooks);
    check("first ok", result.ok);
    check("first not early", !result.early_return);
    check("first wrote flag", result.flag_written);
    check("first flag byte", flag == 1);
    check("first global", result.global_value == &global_target);
    check("first tail modelled", result.tail_call_modelled);
    check("first hook called", g_hook.calls == 1);
    check("first hook self", g_hook.self == &self);
    check("first hook global", g_hook.global == &global_target);
}

void test_second_call_early_returns() {
    std::uint8_t flag = 1;
    int global_target = 7;
    const void* global_cell = &global_target;
    Vtable1594dc0Slot20_20d60Cells cells{};
    cells.once_flag = &flag;
    cells.global_cell = &global_cell;
    Vtable1594dc0Slot20_20d60Hooks hooks{};
    hooks.tail_call = hook_tail;
    g_hook = HookState{};

    const auto result = vtable_1594dc0_slot20_20d60(nullptr, cells, hooks);
    check("second ok", result.ok);
    check("second early", result.early_return);
    check("second no write", !result.flag_written);
    check("second flag stays", flag == 1);
    check("second no tail", !result.tail_call_modelled);
    check("second no hook", g_hook.calls == 0);
    check("second no global", result.global_value == nullptr);
}

void test_two_calls_in_sequence() {
    std::uint8_t flag = 0;
    const void* global_cell = nullptr;
    Vtable1594dc0Slot20_20d60Cells cells{};
    cells.once_flag = &flag;
    cells.global_cell = &global_cell;
    Vtable1594dc0Slot20_20d60Hooks hooks{};
    hooks.tail_call = hook_tail;
    g_hook = HookState{};

    const auto first = vtable_1594dc0_slot20_20d60(nullptr, cells, hooks);
    const auto second = vtable_1594dc0_slot20_20d60(nullptr, cells, hooks);
    check("seq first tails", first.ok && !first.early_return && first.flag_written);
    check("seq second early", second.ok && second.early_return && !second.flag_written);
    check("seq hook once", g_hook.calls == 1);
    check("seq flag", flag == 1);
}

void test_missing_flag_fails() {
    Vtable1594dc0Slot20_20d60Cells cells{};
    const auto result = vtable_1594dc0_slot20_20d60(nullptr, cells);
    check("null flag fails", !result.ok);
    check("null flag no write", !result.flag_written);
    check("null flag no tail", !result.tail_call_modelled);
}

void test_no_global_and_no_hook() {
    std::uint8_t flag = 0;
    Vtable1594dc0Slot20_20d60Cells cells{};
    cells.once_flag = &flag;
    const auto result = vtable_1594dc0_slot20_20d60(nullptr, cells);
    check("bare ok", result.ok);
    check("bare wrote flag", result.flag_written);
    check("bare global null", result.global_value == nullptr);
    check("bare tail not modelled", !result.tail_call_modelled);
}

}  // namespace

int main() {
    test_constants();
    test_first_call_sets_flag_and_tails();
    test_second_call_early_returns();
    test_two_calls_in_sequence();
    test_missing_flag_fails();
    test_no_global_and_no_hook();

    if (failures == 0) {
        std::printf("OK test_vtable_1594dc0_slot20_20d60\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_1594dc0_slot20_20d60: %d failure(s)\n", failures);
    return 1;
}
