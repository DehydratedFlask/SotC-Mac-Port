#include "../core/vtable_15e2028_slot2a0_e21420.hpp"

#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::Vtable15e2028Slot2A0_e21420Cells;
using sotc::Vtable15e2028Slot2A0_e21420Hooks;
using sotc::Vtable15e2028Slot2A0_e21420Path;
using sotc::vtable_15e2028_slot2a0_e21420;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Provider {
    void* object = nullptr;
    int state = 0x0e;
    std::uint64_t kind = 1;
    std::vector<const char*> names;
};

Provider g_provider;
std::vector<int> g_found_hooks;
int g_missing_hooks = 0;

void* lookup(void* provider, const char*) { return static_cast<Provider*>(provider)->object; }
std::int32_t state_of(void* object) { return static_cast<Provider*>(object)->state; }
std::uint64_t kind_of(void* object) { return static_cast<Provider*>(object)->kind; }
std::int32_t count_of(void* provider) {
    return static_cast<std::int32_t>(static_cast<Provider*>(provider)->names.size());
}
const char* name_of(void* provider, std::int32_t index) {
    return static_cast<Provider*>(provider)->names[static_cast<std::size_t>(index)];
}
void on_missing(void*) { ++g_missing_hooks; }
void on_found(void*, std::int32_t index) { g_found_hooks.push_back(index); }

Vtable15e2028Slot2A0_e21420Hooks hooks() {
    Vtable15e2028Slot2A0_e21420Hooks h{};
    h.provider_lookup = lookup;
    h.object_state = state_of;
    h.object_kind = kind_of;
    h.provider_count = count_of;
    h.provider_name = name_of;
    h.on_missing = on_missing;
    h.on_found = on_found;
    return h;
}

void reset() {
    g_provider = Provider{};
    g_found_hooks.clear();
    g_missing_hooks = 0;
}

void test_constants() {
    check("rva", sotc::kVtable15e2028Slot2A0_e21420Rva == 0xe21420);
    check("size", sotc::kVtable15e2028Slot2A0_e21420Size == 0x2a9);
    check("vtable", sotc::kVtable15e2028Slot2A0_e21420VtableAddress == 0x15e2028);
    check("slot", sotc::kVtable15e2028Slot2A0_e21420SlotOffset == 0x2a0);
    check("reloc", sotc::kVtable15e2028Slot2A0_e21420RelocAddress == 0x15e22c8);
    check("provider offset", sotc::kVtable15e2028Slot2A0_e21420ProviderOffset == 0x48);
    check("expected state", sotc::kVtable15e2028Slot2A0_e21420ExpectedState == 0x0e);
    check("statuses", sotc::kVtable15e2028Slot2A0_e21420StatusState == 0x13 &&
                          sotc::kVtable15e2028Slot2A0_e21420StatusScan == 0x14 &&
                          sotc::kVtable15e2028Slot2A0_e21420StatusMissing == 0x1b);
    check("strcmp", sotc::kVtable15e2028Slot2A0_e21420StrcmpTarget == 0xf8f490);
}

void test_missing_object() {
    reset();
    g_provider.object = nullptr;
    Vtable15e2028Slot2A0_e21420Cells cells{};
    cells.provider = &g_provider;
    cells.name = "alpha";
    const auto result = vtable_15e2028_slot2a0_e21420(cells, hooks());
    check("missing path", result.path == Vtable15e2028Slot2A0_e21420Path::missing);
    check("missing status", result.status == 0x1b && result.ok);
    check("missing hook", result.missing_hook_called && g_missing_hooks == 1);
}

void test_state_mismatch() {
    reset();
    g_provider.object = &g_provider;
    g_provider.state = 0x0d;
    Vtable15e2028Slot2A0_e21420Cells cells{};
    cells.provider = &g_provider;
    cells.name = "alpha";
    const auto result = vtable_15e2028_slot2a0_e21420(cells, hooks());
    check("state path", result.path == Vtable15e2028Slot2A0_e21420Path::state_mismatch);
    check("state status", result.status == 0x13);
    check("state no hooks", g_missing_hooks == 0 && g_found_hooks.empty());
}

void test_kind_mismatch_goes_missing() {
    reset();
    g_provider.object = &g_provider;
    g_provider.kind = 2;
    Vtable15e2028Slot2A0_e21420Cells cells{};
    cells.provider = &g_provider;
    cells.name = "alpha";
    const auto result = vtable_15e2028_slot2a0_e21420(cells, hooks());
    check("kind path", result.path == Vtable15e2028Slot2A0_e21420Path::missing);
    check("kind status", result.status == 0x1b);
}

void test_null_name_with_many_entries() {
    reset();
    g_provider.object = &g_provider;
    g_provider.names = {"a", "b"};
    Vtable15e2028Slot2A0_e21420Cells cells{};
    cells.provider = &g_provider;
    cells.name = nullptr;
    const auto result = vtable_15e2028_slot2a0_e21420(cells, hooks());
    check("null name path", result.path == Vtable15e2028Slot2A0_e21420Path::scan_exhausted);
    check("null name status", result.status == 0x14);
}

void test_found() {
    reset();
    g_provider.object = &g_provider;
    g_provider.names = {"alpha", "beta", "gamma"};
    Vtable15e2028Slot2A0_e21420Cells cells{};
    cells.provider = &g_provider;
    cells.name = "beta";
    const auto result = vtable_15e2028_slot2a0_e21420(cells, hooks());
    check("found path", result.path == Vtable15e2028Slot2A0_e21420Path::found);
    check("found index", result.found_index == 1);
    check("found scanned", result.scanned == 2);
    check("found hook", result.found_hook_called && g_found_hooks.size() == 1 &&
                            g_found_hooks[0] == 1);
}

void test_not_found() {
    reset();
    g_provider.object = &g_provider;
    g_provider.names = {"alpha"};
    Vtable15e2028Slot2A0_e21420Cells cells{};
    cells.provider = &g_provider;
    cells.name = "delta";
    const auto result = vtable_15e2028_slot2a0_e21420(cells, hooks());
    check("not found path", result.path == Vtable15e2028Slot2A0_e21420Path::scan_exhausted);
    check("not found status", result.status == 0x14);
    check("not found scanned", result.scanned == 1);
}

void test_missing_hooks_fail() {
    reset();
    Vtable15e2028Slot2A0_e21420Cells cells{};
    const auto result = vtable_15e2028_slot2a0_e21420(cells);
    check("missing hooks fail", !result.ok);
    check("missing hooks path none", result.path == Vtable15e2028Slot2A0_e21420Path::none);
}

void test_path_names() {
    check("name found", std::string_view(sotc::vtable_15e2028_slot2a0_e21420_path_name(
                             Vtable15e2028Slot2A0_e21420Path::found)) == "found");
    check("name missing", std::string_view(sotc::vtable_15e2028_slot2a0_e21420_path_name(
                             Vtable15e2028Slot2A0_e21420Path::missing)) == "missing");
}

}  // namespace

int main() {
    test_constants();
    test_missing_object();
    test_state_mismatch();
    test_kind_mismatch_goes_missing();
    test_null_name_with_many_entries();
    test_found();
    test_not_found();
    test_missing_hooks_fail();
    test_path_names();

    if (failures == 0) {
        std::printf("OK test_vtable_15e2028_slot2a0_e21420\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15e2028_slot2a0_e21420: %d failure(s)\n", failures);
    return 1;
}
