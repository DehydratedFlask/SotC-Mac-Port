// Fixture for the 0x2569d0 member-install driver: row order, args, replace idioms, failure stops.
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/owner_members.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
std::vector<void*> g_live;
std::vector<std::size_t> g_ctor_rows;
std::vector<void*> g_ctor_args;
std::vector<void*> g_destroyed;
int g_virtual = 0;
int g_dtor = 0;
std::size_t g_fail_ctor_row = 0;
std::size_t g_fail_alloc_call = 0;
std::size_t g_alloc_calls = 0;

void* alloc(std::size_t size) {
    ++g_alloc_calls;
    if (g_fail_alloc_call != 0 && g_alloc_calls == g_fail_alloc_call) {
        return nullptr;
    }
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    g_live.push_back(p);
    return p;
}

bool dealloc(void* ptr) {
    const auto it = std::find(g_live.begin(), g_live.end(), ptr);
    CHECK(it != g_live.end());
    g_live.erase(it);
    std::free(ptr);
    return true;
}

bool ctor(std::size_t row, void* object, void* arg) {
    if (g_fail_ctor_row != 0 && row == g_fail_ctor_row) {
        return false;
    }
    g_ctor_rows.push_back(row);
    g_ctor_args.push_back(arg);
    *static_cast<std::uint8_t*>(object) = static_cast<std::uint8_t>(row);
    if (row == 4 || row == 8) {
        CHECK(arg != nullptr);
        *static_cast<void**>(arg) = object;  // self-registered cell
    }
    return true;
}

void virtual_dtor(void* object) {
    ++g_virtual;
    g_destroyed.push_back(object);
    CHECK(dealloc(object));
}

void dtor(void* object) {
    ++g_dtor;
    g_destroyed.push_back(object);
}

void reset_logs() {
    g_ctor_rows.clear();
    g_ctor_args.clear();
    g_destroyed.clear();
    g_virtual = 0;
    g_dtor = 0;
    g_fail_ctor_row = 0;
    g_fail_alloc_call = 0;
    g_alloc_calls = 0;
}

void free_all() {
    for (void* p : g_live) {
        std::free(p);
    }
    g_live.clear();
}
}  // namespace

int main() {
    CHECK(kOwnerMemberRowCount == 21);
    CHECK(kOwnerMembersStartRva == 0x256a15);
    CHECK(kOwnerMembersEndRva == 0x2570a9);
    CHECK(kOwnerMemberRows[0].size == 0x350);
    CHECK(kOwnerMemberRows[6].size == 0x1);
    CHECK(kOwnerMemberRows[10].size == 0x178);
    CHECK(kOwnerMemberRows[15].size == 0x4028);
    CHECK(kOwnerMemberRows[20].size == 0x90);

    OwnerMemberHooks hooks{};
    hooks.allocate = alloc;
    hooks.deallocate = dealloc;
    hooks.ctor = ctor;
    hooks.virtual_dtor = virtual_dtor;
    hooks.dtor = dtor;

    // ---- 1. First install: 19 stored rows + 2 self-registered, no destruction. ----
    {
        OwnerMemberState state;
        reset_logs();
        const auto result = init_owner_members(state, hooks);
        CHECK(result.ok);
        CHECK(result.installed == 19);
        CHECK(result.self_registered == 2);
        CHECK(result.replaced == 0);
        CHECK(result.virtual_destroyed == 0);
        CHECK(result.dtor_destroyed == 0);
        CHECK(result.stopped_row == 0);
        CHECK(g_ctor_rows.size() == 21);
        for (std::size_t i = 0; i < 21; ++i) {
            CHECK(g_ctor_rows[i] == i + 1);
        }

        CHECK(state.globals[0x2EE2D00] != nullptr);
        CHECK(state.globals[0x2EE2A50] != nullptr);
        CHECK(state.globals[0x2EE2CF8] != nullptr);
        CHECK(state.globals[0x2EDEF60] != nullptr);
        CHECK(state.this_fields[0x3B48] != nullptr);
        CHECK(state.this_fields[0x3B50] != nullptr);
        CHECK(state.this_fields[0x3B68] != nullptr);
        CHECK(state.this_fields[0x3B70] != nullptr);
        CHECK(state.self_cells[0x2EEC4D8] != nullptr);
        CHECK(state.self_cells[0x2EEC4E0] != nullptr);

        // Row 3 takes row 1's object; rows 17/18 take row 9's.
        CHECK(g_ctor_args[2] == state.globals[0x2EE2D00]);
        CHECK(g_ctor_args[16] == state.globals[0x2EDF140]);
        CHECK(g_ctor_args[17] == state.globals[0x2EDF140]);
        CHECK(g_ctor_args[3] == &state.self_cells[0x2EEC4D8]);
        CHECK(g_ctor_args[7] == &state.self_cells[0x2EEC4E0]);

        // ---- 2. Second install: 18 replaced rows; row 7 is stored without destroying. ----
        void* const row7_old = state.globals[0x2ED37B8];
        const auto again = init_owner_members(state, hooks);
        CHECK(again.ok);
        CHECK(again.installed == 19);
        CHECK(again.replaced == 18);  // all but self-registered (2) and row 7 (1)
        CHECK(again.virtual_destroyed == 7);
        CHECK(again.dtor_destroyed == 11);
        CHECK(again.freed == 11);
        CHECK(again.stopped_row == 0);
        CHECK(g_virtual == 7);
        CHECK(g_dtor == 11);
        CHECK(std::find(g_destroyed.begin(), g_destroyed.end(), row7_old) == g_destroyed.end());
        CHECK(std::find(g_live.begin(), g_live.end(), row7_old) != g_live.end());

        free_all();
    }

    // ---- 3. Missing ctor at row 5 stops after four rows and frees the new object. ----
    {
        OwnerMemberState state;
        reset_logs();
        g_fail_ctor_row = 5;
        const auto result = init_owner_members(state, hooks);
        CHECK(!result.ok);
        CHECK(result.stopped_row == 5);
        CHECK(result.installed == 3);  // rows 1, 2, 3
        CHECK(result.self_registered == 1);
        CHECK(g_live.size() == 4);  // rows 1..4, row 5 freed
        CHECK(state.globals[0x2EE2D00] != nullptr);
        CHECK(state.globals[0x2EE2688] == nullptr);  // row 5 never installed
        free_all();
    }

    // ---- 4. Allocation failure at row 3 stops with two installed. ----
    {
        OwnerMemberState state;
        reset_logs();
        g_fail_alloc_call = 3;
        const auto result = init_owner_members(state, hooks);
        CHECK(!result.ok);
        CHECK(result.stopped_row == 3);
        CHECK(result.installed == 2);
        CHECK(g_live.size() == 2);
        free_all();
    }

    // ---- 5. Missing virtual-dtor hook fails before destroying or storing row 1. ----
    {
        OwnerMemberState state;
        reset_logs();
        OwnerMemberHooks no_dtor = hooks;
        no_dtor.virtual_dtor = nullptr;
        const auto first = init_owner_members(state, hooks);
        CHECK(first.ok);
        const auto second = init_owner_members(state, no_dtor);
        CHECK(!second.ok);
        CHECK(second.stopped_row == 1);
        CHECK(state.globals[0x2EE2D00] != nullptr);
        free_all();
    }

    // ---- 6. Missing allocator/deallocator fails before any row. ----
    {
        OwnerMemberState state;
        reset_logs();
        OwnerMemberHooks no_alloc{};
        CHECK(!init_owner_members(state, no_alloc).ok);
        CHECK(g_live.empty());
    }

    return 0;
}
