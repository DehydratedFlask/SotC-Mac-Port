// Fixture for the owner destructor 0x1bdae0 (destroy -> delete -> clear slot).
#include <cstdio>
#include <cstdlib>

#include "core/owner_dtor_1bdae0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_order[4] = {0, 0, 0, 0};
int g_n = 0;
void* g_destroyed = nullptr;
void* g_freed = nullptr;

void destroy_hook(void* p) { g_order[g_n++] = 1; g_destroyed = p; }
void free_hook(void* p) { g_order[g_n++] = 2; g_freed = p; }
}  // namespace

int main() {
    CHECK(kOwnerDtor1bdae0Rva == 0x1bdae0);
    CHECK(kOwnerDtor1bdae0Size == 46);
    CHECK(kOwnerDtor1bdae0EndRva == 0x1bdb0e);
    CHECK(kOwnerDtor1bdae0DestroyRva == 0x1bede0);
    CHECK(kOwnerDtor1bdae0FreeRva == 0x170b0);

    OwnerDtor1bdae0Hooks h{};
    h.destroy = destroy_hook;
    h.deallocate = free_hook;

    {
        int obj = 0;
        void* slot = &obj;
        g_n = 0;
        const auto r = owner_dtor_1bdae0(&slot, h);
        CHECK(r.ok);
        CHECK(!r.was_empty);
        CHECK(r.destroyed && r.deallocated && r.slot_cleared);
        CHECK(r.owned == &obj);
        CHECK(slot == nullptr);
        CHECK(g_n == 2 && g_order[0] == 1 && g_order[1] == 2);  // destroy before free
        CHECK(g_destroyed == &obj && g_freed == &obj);
    }

    // Already empty: the original's `je` path does nothing and succeeds.
    {
        void* slot = nullptr;
        g_n = 0;
        const auto r = owner_dtor_1bdae0(&slot, h);
        CHECK(r.ok);
        CHECK(r.was_empty);
        CHECK(!r.destroyed && !r.deallocated && !r.slot_cleared);
        CHECK(g_n == 0);
    }

    // Missing destroy hook on a non-null slot: visible failure, slot untouched.
    {
        int obj = 0;
        void* slot = &obj;
        OwnerDtor1bdae0Hooks partial{};
        const auto r = owner_dtor_1bdae0(&slot, partial);
        CHECK(!r.ok);
        CHECK(!r.destroyed);
        CHECK(slot == &obj);
    }

    // Missing free hook: destruction happened but the slot is NOT cleared and ok is false.
    {
        int obj = 0;
        void* slot = &obj;
        OwnerDtor1bdae0Hooks partial{};
        partial.destroy = destroy_hook;
        g_n = 0;
        const auto r = owner_dtor_1bdae0(&slot, partial);
        CHECK(!r.ok);
        CHECK(r.destroyed);
        CHECK(!r.deallocated);
        CHECK(!r.slot_cleared);
        CHECK(slot == &obj);
    }

    std::printf("owner_dtor_1bdae0: all checks passed (destroy->free->clear, empty path, visible failures)\n");
    return 0;
}
