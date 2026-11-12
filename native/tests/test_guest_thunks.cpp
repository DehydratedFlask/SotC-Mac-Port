// Fixture for the retail-ABI thunk registry (CPU-01-AOT-CFG-CALLABLE-01 successor).
// Calls the registered entries through guest_thunk_invoke (a real SysV call) and asserts the
// observed side effects and return values, plus the unknown-RVA negative.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/guest_thunks.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

int g_hook_calls = 0;
void* g_hook_object = nullptr;

void register_hook(void* object, void*) {
    ++g_hook_calls;
    g_hook_object = object;
}

struct alignas(8) Object {
    unsigned char bytes[0x40];
};

}  // namespace

int main() {
    CHECK(guest_thunk_count() == 2);
    CHECK(guest_thunk_find(0x20d60) != nullptr);
    CHECK(guest_thunk_find(0x13d20) != nullptr);
    CHECK(guest_thunk_find(0x12345678) == nullptr);

    // 0x13d20: `movl %esi,0x1c(%rdi); ret`. Observe the stored dword after the call.
    Object setter{};
    const std::uint64_t setter_args[6] = {reinterpret_cast<std::uint64_t>(&setter), 0x2aULL, 0, 0, 0, 0};
    std::uint64_t result = 0xdeadbeef;
    CHECK(guest_thunk_invoke(0x13d20, setter_args, &result));
    std::uint32_t stored = 0;
    std::memcpy(&stored, setter.bytes + 0x1c, sizeof(stored));
    CHECK(stored == 0x2a);
    CHECK(result == 0);

    // 0x20d60: first call flips object+0x18 and reports through the hook; the second is a no-op.
    Object conccmd{};
    guest_set_concmd_register_hook(register_hook);
    const std::uint64_t conccmd_args[6] = {reinterpret_cast<std::uint64_t>(&conccmd), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x20d60, conccmd_args, &result));
    CHECK(conccmd.bytes[0x18] == 1);
    CHECK(g_hook_calls == 1);
    CHECK(g_hook_object == &conccmd);
    CHECK(guest_thunk_invoke(0x20d60, conccmd_args, &result));
    CHECK(conccmd.bytes[0x18] == 1);
    CHECK(g_hook_calls == 1);

    guest_set_concmd_register_hook(nullptr);
    const std::uint64_t unknown_args[6] = {0, 0, 0, 0, 0, 0};
    CHECK(!guest_thunk_invoke(0x12345678, unknown_args, &result));

    std::printf("OK test_guest_thunks: thunks=%zu setter=0x%x register_flag=%u hook_calls=%d\n",
                guest_thunk_count(), stored, conccmd.bytes[0x18], g_hook_calls);
    return 0;
}
