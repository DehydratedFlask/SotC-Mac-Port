// CPU-01-STARTUP-SLOT-419: fixture for startup constructor index 419 (0x56fae0): five 0x28-byte splash-screen
// records, then the TSC stamp by tail jump to store_tsc_323d0. Lane M2.
#include <cstddef>

#include "../tests/l4_test_util.hpp"
#include "core/ctor_slot_419_56fae0.hpp"

namespace {
using namespace l4;

std::uint64_t g_hook_calls = 0;
std::uint64_t fake_timebase() { ++g_hook_calls; return 0x1234; }

void section_constants() {
    CHECK(kSlot419Rva == 0x56fae0 && kSlot419Size == 402 && kSlot419TableRva == 0x2f117b0 &&
          kSlot419TscRva == 0x2f11878 && kSlot419RecordSize == 0x28 && kSlot419RecordCount == 5);
    CHECK(kSlot419TableRva + kSlot419RecordCount * kSlot419RecordSize == kSlot419TscRva);
    CHECK(sizeof(SplashRecord419) == 0x28 && offsetof(Slot419Objects, tsc) == 0xc8);
}

struct Expect {
    std::uint32_t a, b, c;
    std::uint8_t f0c, f0d;
    const char* path;
};

void section_alone() {
    // Reset the process-wide TSC cells so the first-call branch (guard == 0) is exercised deterministically.
    store_tsc_guard_cell_2ea4700() = 0;
    store_tsc_global_cell_2ea4708() = 0;
    Slot419Objects o;
    poison(o);
    StoreTsc323d0Hooks hooks;
    hooks.init_timebase_f8f860 = &fake_timebase;
    g_hook_calls = 0;
    const Slot419Result r = init_slot419_56fae0(o, hooks);
    CHECK(r.ok);
    const Expect want[5] = {
        {0x40a00000u, 0x3f000000u, 0u, 0, 0, kSlot419Path0},
        {0x40a00000u, 0x3f000000u, 0x3eaaaaabu, 0, 0, kSlot419Path1},
        {0x40000000u, 0x3f000000u, 0x3eaaaaabu, 0, 1, kSlot419Path2},
        {0x40000000u, 0x3f000000u, 0x3eaaaaabu, 0, 1, kSlot419Path3},
        {0x40000000u, 0u, 0x3eaaaaabu, 0, 1, kSlot419Path4},
    };
    for (std::size_t i = 0; i < 5; ++i) {
        const SplashRecord419& rec = o.records[i];
        CHECK(rec.a == want[i].a && rec.b == want[i].b && rec.c == want[i].c);
        CHECK(rec.f0c == want[i].f0c && rec.f0d == want[i].f0d && rec.zero10 == 0 && rec.minus1 == -1);
        CHECK(rec.path == want[i].path);
        check_untouched(rec, {{0x00, 4}, {0x04, 4}, {0x08, 4}, {0x0c, 1}, {0x0d, 1}, {0x10, 4}, {0x18, 8}, {0x20, 4}});
    }
    CHECK(std::strstr(want[0].path, "epilepsy_") != nullptr && std::strstr(want[4].path, "bluepoint_") != nullptr);
    CHECK(g_hook_calls == 1 && store_tsc_guard_cell_2ea4700() == 0x1234);
    CHECK(o.tsc != kPoison * 0x0101010101010101ull);   // the stamp qword was written by the leaf
}

void section_tsc_default_hooks() {
    // With the cells already initialised the leaf only stores the (host) timestamp.
    store_tsc_guard_cell_2ea4700() = 1;
    store_tsc_global_cell_2ea4708() = 7;
    Slot419Objects o;
    poison(o);
    const Slot419Result r = init_slot419_56fae0(o);
    CHECK(r.ok && o.tsc != 0xA5A5A5A5A5A5A5A5ull);
    CHECK(store_tsc_global_cell_2ea4708() == 7);   // the base cell is only set by the first call
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_tsc_default_hooks();
    std::puts("OK test_ctor_slot_419");
    return 0;
}
