#include "../core/vtable_15e2d60_slot_d0_e24900.hpp"

#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::Vtable15e2d60SlotD0_e24900Cells;
using sotc::Vtable15e2d60SlotD0_e24900Hooks;
using sotc::vtable_15e2d60_slot_d0_e24900;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    std::vector<std::string> order;
    int status_value = 3;
    void* stored = nullptr;
};

Trace g_trace;
int g_a_token = 0;
int g_result_token = 0;
bool g_null_result = false;

void* slot28(void*) { g_trace.order.emplace_back("slot28"); return &g_a_token; }
std::uint32_t slot48(void*) { g_trace.order.emplace_back("slot48"); return 7; }
void* sub_slot40(void*, void* a, std::uint32_t index) {
    g_trace.order.emplace_back("sub40");
    g_trace.stored = a;
    (void)index;
    return g_null_result ? nullptr : &g_result_token;
}
std::int32_t result_b8(void*, void*) {
    g_trace.order.emplace_back("b8");
    return g_trace.status_value;
}
void result_10(void*) { g_trace.order.emplace_back("slot10"); }

Vtable15e2d60SlotD0_e24900Hooks hooks() {
    Vtable15e2d60SlotD0_e24900Hooks h{};
    h.slot28 = slot28;
    h.slot48 = slot48;
    h.sub_slot40 = sub_slot40;
    h.result_slot_b8 = result_b8;
    h.result_slot10 = result_10;
    return h;
}

void test_constants() {
    check("rva", sotc::kVtable15e2d60SlotD0_e24900Rva == 0xe24900);
    check("size", sotc::kVtable15e2d60SlotD0_e24900Size == 0x84);
    check("vtable", sotc::kVtable15e2d60SlotD0_e24900VtableAddress == 0x15e2d60);
    check("slot", sotc::kVtable15e2d60SlotD0_e24900SlotOffset == 0xd0);
    check("reloc", sotc::kVtable15e2d60SlotD0_e24900RelocAddress == 0x15e2e30);
    check("sub", sotc::kVtable15e2d60SlotD0_e24900SubOffset == 0x08);
    check("status", sotc::kVtable15e2d60SlotD0_e24900StatusNull == 0x1b);
}

void test_null_result() {
    g_trace = Trace{};
    g_null_result = true;
    int sub = 0;
    void* out = reinterpret_cast<void*>(0x1);
    Vtable15e2d60SlotD0_e24900Cells cells{};
    cells.sub = &sub;
    cells.out = &out;
    const auto result = vtable_15e2d60_slot_d0_e24900(cells, hooks());
    check("null result ok", result.ok);
    check("null early", result.early_null && result.status == 0x1b);
    check("null stored", out == nullptr);
    check("null order", g_trace.order.size() == 3);
    g_null_result = false;
}

void test_status_zero_clears() {
    g_trace = Trace{};
    g_trace.status_value = 0;
    int sub = 0;
    void* out = nullptr;
    Vtable15e2d60SlotD0_e24900Cells cells{};
    cells.sub = &sub;
    cells.out = &out;
    const auto result = vtable_15e2d60_slot_d0_e24900(cells, hooks());
    check("zero ok", result.ok && result.status == 0);
    check("zero cleared", result.out_cleared && out == nullptr);
    check("zero no release", !result.release_hook_called);
    check("zero order", g_trace.order.size() == 4 && g_trace.order.back() == "b8");
}

void test_status_nonzero_releases() {
    g_trace = Trace{};
    g_trace.status_value = 5;
    int sub = 0;
    void* out = nullptr;
    Vtable15e2d60SlotD0_e24900Cells cells{};
    cells.sub = &sub;
    cells.out = &out;
    const auto result = vtable_15e2d60_slot_d0_e24900(cells, hooks());
    check("nonzero status", result.ok && result.status == 5);
    check("nonzero released", result.release_hook_called && result.out_cleared);
    check("nonzero order", g_trace.order.size() == 5 && g_trace.order.back() == "slot10");
}

void test_missing_cells_fail() {
    const auto result = vtable_15e2d60_slot_d0_e24900({});
    check("missing fails", !result.ok);
}

void test_missing_status_hook_fails() {
    g_trace = Trace{};
    int sub = 0;
    void* out = nullptr;
    Vtable15e2d60SlotD0_e24900Cells cells{};
    cells.sub = &sub;
    cells.out = &out;
    Vtable15e2d60SlotD0_e24900Hooks h = hooks();
    h.result_slot_b8 = nullptr;
    const auto result = vtable_15e2d60_slot_d0_e24900(cells, h);
    check("missing b8 fails", !result.ok);
    check("missing b8 stored", out == &g_result_token);
}

}  // namespace

int main() {
    test_constants();
    test_null_result();
    test_status_zero_clears();
    test_status_nonzero_releases();
    test_missing_cells_fail();
    test_missing_status_hook_fails();

    if (failures == 0) {
        std::printf("OK test_vtable_15e2d60_slot_d0_e24900\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15e2d60_slot_d0_e24900: %d failure(s)\n", failures);
    return 1;
}
