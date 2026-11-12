// Fixture for the retail-ABI thunk registry and the CFG-resolved dispatch site
// (CPU-01-AOT-CFG-CALLABLE-01 continuation). Everything is invoked through guest_thunk_invoke /
// guest_dispatch_slot, i.e. a real SysV call, and asserted on the observed object delta or return.
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
    unsigned char bytes[0x80];
};

std::uint32_t load32(const Object& object, std::size_t offset) {
    std::uint32_t value = 0;
    std::memcpy(&value, object.bytes + offset, sizeof(value));
    return value;
}

std::uint64_t load64(const Object& object, std::size_t offset) {
    std::uint64_t value = 0;
    std::memcpy(&value, object.bytes + offset, sizeof(value));
    return value;
}

}  // namespace

int main() {
    CHECK(guest_thunk_count() == 81);
    CHECK(guest_thunk_find(0x20d60) != nullptr);
    CHECK(guest_thunk_find(0x13d20) != nullptr);
    CHECK(guest_thunk_find(0xc4f0) != nullptr);
    CHECK(guest_thunk_find(0x12345678) == nullptr);

    // 0x13d20: `movl %esi,0x1c(%rdi); ret`. Observe the stored dword after the call.
    Object setter{};
    const std::uint64_t setter_args[6] = {reinterpret_cast<std::uint64_t>(&setter), 0x2aULL, 0, 0, 0, 0};
    std::uint64_t result = 0xdeadbeef;
    CHECK(guest_thunk_invoke(0x13d20, setter_args, &result));
    CHECK(load32(setter, 0x1c) == 0x2a);

    // 0xb890: `movl $1,%eax; ret`. The pinned immediate is the observed return.
    CHECK(guest_thunk_invoke(0xb890, nullptr, &result));
    CHECK(result == 1);

    // 0xc2a0: copy rsi+0x10 to rdi+0x10, install 0x1594280 at rdi+0x20, return the copied value.
    Object dst{}, src{};
    const std::uint64_t copied = 0x1122334455667788ULL;
    std::memcpy(src.bytes + 0x10, &copied, sizeof(copied));
    const std::uint64_t ctor_args[6] = {reinterpret_cast<std::uint64_t>(&dst),
                                        reinterpret_cast<std::uint64_t>(&src), 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0xc2a0, ctor_args, &result));
    CHECK(result == copied);
    CHECK(load64(dst, 0x10) == copied);
    CHECK(load64(dst, 0x20) == 0x1594280);

    // 0xc4f0: one-byte `ret`, registered only because every resolved caller ignores rax.
    CHECK(guest_thunk_invoke(0xc4f0, nullptr, &result));

    // The CFG-resolved site 0x28471 reads slot 0x20 of the host vtable image and invokes 0x20d60.
    const GuestDispatchSite& site = guest_dispatch_site();
    CHECK(site.site_rva == 0x28471);
    CHECK(site.function_rva == 0x27990);
    CHECK(site.target_rva == 0x20d60);
    CHECK(site.vtable_rva == 0x1594dc0);
    CHECK(site.slot_offset == 0x20);
    std::size_t vtable_size = 0;
    const std::uint8_t* vtable = guest_dispatch_vtable(&vtable_size);
    CHECK(vtable != nullptr);
    CHECK(vtable_size == 0x28);

    Object command{};
    guest_set_concmd_register_hook(register_hook);
    g_hook_calls = 0;
    const std::uint64_t command_args[6] = {reinterpret_cast<std::uint64_t>(&command), 0, 0, 0, 0, 0};
    std::uint64_t target = 0;
    CHECK(guest_dispatch_slot(vtable, vtable_size, static_cast<std::size_t>(site.slot_offset),
                              command_args, &result, &target));
    CHECK(target == 0x20d60);
    CHECK(command.bytes[0x18] == 1);
    CHECK(g_hook_calls == 1);
    CHECK(g_hook_object == &command);
    // Second dispatch is a no-op because the flag is already set.
    CHECK(guest_dispatch_slot(vtable, vtable_size, static_cast<std::size_t>(site.slot_offset),
                              command_args, &result, &target));
    CHECK(g_hook_calls == 1);
    // A slot outside the host vtable image is rejected.
    CHECK(!guest_dispatch_slot(vtable, vtable_size, vtable_size, command_args, &result, &target));
    guest_set_concmd_register_hook(nullptr);

    // Every one of the 15 previously resolved targets runs; the 14 one-byte stubs must leave a
    // canary object byte-for-byte unchanged.
    std::size_t target_count = 0;
    const std::uint64_t* targets = guest_resolved_targets(&target_count);
    CHECK(targets != nullptr);
    CHECK(target_count == 15);
    int stubs_called = 0;
    for (std::size_t i = 0; i < target_count; ++i) {
        if (targets[i] == 0x20d60) continue;
        Object canary{};
        std::memset(canary.bytes, 0xa5, sizeof(canary.bytes));
        const std::uint64_t canary_args[6] = {reinterpret_cast<std::uint64_t>(&canary), 0, 0, 0, 0, 0};
        CHECK(guest_thunk_invoke(targets[i], canary_args, &result));
        for (unsigned char byte : canary.bytes) CHECK(byte == 0xa5);
        ++stubs_called;
    }
    CHECK(stubs_called == 14);

    // Direct invocation of 0x20d60 keeps the same observable contract.
    Object direct{};
    const std::uint64_t direct_args[6] = {reinterpret_cast<std::uint64_t>(&direct), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x20d60, direct_args, &result));
    CHECK(direct.bytes[0x18] == 1);

    const std::uint64_t unknown_args[6] = {0, 0, 0, 0, 0, 0};
    CHECK(!guest_thunk_invoke(0x12345678, unknown_args, &result));

    std::printf("OK test_guest_thunks: thunks=%zu stubs=%d setter=0x%x const=0x%llx ctor_copy=0x%llx "
                "ctor_const=0x%llx dispatch=0x%llx flag=%u hooks=%d\n",
                guest_thunk_count(), stubs_called, load32(setter, 0x1c),
                static_cast<unsigned long long>(1),
                static_cast<unsigned long long>(load64(dst, 0x10)),
                static_cast<unsigned long long>(load64(dst, 0x20)),
                static_cast<unsigned long long>(target), command.bytes[0x18], g_hook_calls);
    return 0;
}
