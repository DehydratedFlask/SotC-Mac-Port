#include "../core/vtable_15ed2a0_slot_c0_e26f90.hpp"

#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::Vtable15ed2a0SlotC0_e26f90Cells;
using sotc::Vtable15ed2a0SlotC0_e26f90Hooks;
using sotc::vtable_15ed2a0_slot_c0_e26f90;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    std::vector<std::string> order;
    int status_value = 0;
    int helper_value = 42;
    std::uint64_t* canary_to_corrupt = nullptr;
    void* helper_arg2 = nullptr;
    std::int32_t helper_arg3 = 0;
};

Trace g_trace;

std::uint64_t slot78(void*) { g_trace.order.emplace_back("slot78"); return 0x99; }
std::int32_t slot90(void*, const char* name, void*) {
    g_trace.order.emplace_back(name != nullptr ? "slot90" : "slot90-noname");
    if (g_trace.canary_to_corrupt != nullptr) {
        ++*g_trace.canary_to_corrupt;
    }
    return g_trace.status_value;
}
std::int32_t helper(void*, void* arg2, std::int32_t arg3, void*, void*) {
    g_trace.order.emplace_back("helper");
    g_trace.helper_arg2 = arg2;
    g_trace.helper_arg3 = arg3;
    return g_trace.helper_value;
}

Vtable15ed2a0SlotC0_e26f90Hooks hooks() {
    Vtable15ed2a0SlotC0_e26f90Hooks h{};
    h.slot78 = slot78;
    h.slot90 = slot90;
    h.helper = helper;
    return h;
}

void test_constants() {
    check("rva", sotc::kVtable15ed2a0SlotC0_e26f90Rva == 0xe26f90);
    check("size", sotc::kVtable15ed2a0SlotC0_e26f90Size == 0xb7);
    check("vtable", sotc::kVtable15ed2a0SlotC0_e26f90VtableAddress == 0x15ed2a0);
    check("slot", sotc::kVtable15ed2a0SlotC0_e26f90SlotOffset == 0xc0);
    check("reloc", sotc::kVtable15ed2a0SlotC0_e26f90RelocAddress == 0x15ed360);
    check("name", sotc::kVtable15ed2a0SlotC0_e26f90NameRva == 0x131706d);
    check("helper", sotc::kVtable15ed2a0SlotC0_e26f90HelperRva == 0xe255b0);
}

void test_early_zero() {
    g_trace = Trace{};
    g_trace.status_value = 3;
    std::uint64_t canary = 1;
    char buffer[16] = {};
    int local = 0;
    Vtable15ed2a0SlotC0_e26f90Cells cells{};
    cells.canary = &canary;
    cells.buffer = buffer;
    cells.local = &local;
    const auto result = vtable_15ed2a0_slot_c0_e26f90(cells, hooks());
    check("early ok", result.ok);
    check("early zero", result.early_zero && result.status == 0);
    check("early no helper", !result.helper_called);
    check("early fetched", result.fetched == 0x99);
    check("early order", g_trace.order.size() == 2 && g_trace.order[0] == "slot78");
}

void test_helper_path() {
    g_trace = Trace{};
    g_trace.status_value = 0;
    g_trace.helper_value = 7;
    std::uint64_t canary = 2;
    char buffer[16] = {};
    int local = 0;
    int arg2 = 0;
    Vtable15ed2a0SlotC0_e26f90Cells cells{};
    cells.self = reinterpret_cast<void*>(0x1);
    cells.arg2 = &arg2;
    cells.arg3 = 5;
    cells.canary = &canary;
    cells.buffer = buffer;
    cells.local = &local;
    const auto result = vtable_15ed2a0_slot_c0_e26f90(cells, hooks());
    check("helper ok", result.ok);
    check("helper called", result.helper_called && result.status == 7);
    check("helper args", g_trace.helper_arg2 == &arg2 && g_trace.helper_arg3 == 5);
    check("helper order", g_trace.order.size() == 3 && g_trace.order.back() == "helper");
}

void test_canary_mismatch() {
    g_trace = Trace{};
    g_trace.status_value = 0;
    std::uint64_t canary = 4;
    g_trace.canary_to_corrupt = &canary;
    char buffer[16] = {};
    int local = 0;
    Vtable15ed2a0SlotC0_e26f90Cells cells{};
    cells.canary = &canary;
    cells.buffer = buffer;
    cells.local = &local;
    const auto result = vtable_15ed2a0_slot_c0_e26f90(cells, hooks());
    check("canary fails", !result.ok && result.canary_mismatch);
    g_trace.canary_to_corrupt = nullptr;
}

void test_missing_cells_fail() {
    const auto result = vtable_15ed2a0_slot_c0_e26f90({});
    check("missing fails", !result.ok);
}

}  // namespace

int main() {
    test_constants();
    test_early_zero();
    test_helper_path();
    test_canary_mismatch();
    test_missing_cells_fail();

    if (failures == 0) {
        std::printf("OK test_vtable_15ed2a0_slot_c0_e26f90\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15ed2a0_slot_c0_e26f90: %d failure(s)\n", failures);
    return 1;
}
