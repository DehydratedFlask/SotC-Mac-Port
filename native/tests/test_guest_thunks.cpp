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
char g_printf_text[64] = {};

void printf_hook(const char* text) {
    std::snprintf(g_printf_text, sizeof(g_printf_text), "%s", text != nullptr ? text : "");
}

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
    CHECK(guest_thunk_count() == 99);
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

    // 0xa1cc0 has no entry-map name; its only caller is the direct call at 0x256c38 in 0x2569d0.
    // The return is dead there, so the body is an exact no-op.
    Object full_scan_canary{};
    std::memset(full_scan_canary.bytes, 0x3c, sizeof(full_scan_canary.bytes));
    const std::uint64_t full_scan_args[6] = {
        reinterpret_cast<std::uint64_t>(&full_scan_canary), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0xa1cc0, full_scan_args, &result));
    for (unsigned char byte : full_scan_canary.bytes) CHECK(byte == 0x3c);

    // 0x20d80 non-null path: case-insensitive RB-set insert, duplicate rejection and container
    // size. Alpha, Beta, Gamma is a right-right insertion and forces a rotation.
    alignas(8) unsigned char concmd_container[0x80] = {};
    alignas(8) unsigned char concmd_a[0x60];
    alignas(8) unsigned char concmd_b[0x60];
    alignas(8) unsigned char concmd_c[0x60];
    alignas(8) unsigned char concmd_dup[0x60];
    std::memset(concmd_a, 0x11, sizeof(concmd_a));
    std::memset(concmd_b, 0x22, sizeof(concmd_b));
    std::memset(concmd_c, 0x44, sizeof(concmd_c));
    std::memset(concmd_dup, 0x33, sizeof(concmd_dup));
    const char key_a[] = "Alpha";
    const char key_b[] = "Beta";
    const char key_c[] = "Gamma";
    const char key_dup[] = "alpha";
    const std::uint64_t key_a_address = reinterpret_cast<std::uint64_t>(key_a);
    const std::uint64_t key_b_address = reinterpret_cast<std::uint64_t>(key_b);
    const std::uint64_t key_c_address = reinterpret_cast<std::uint64_t>(key_c);
    const std::uint64_t key_dup_address = reinterpret_cast<std::uint64_t>(key_dup);
    std::memcpy(concmd_a + 0x20, &key_a_address, sizeof(key_a_address));
    std::memcpy(concmd_b + 0x20, &key_b_address, sizeof(key_b_address));
    std::memcpy(concmd_c + 0x20, &key_c_address, sizeof(key_c_address));
    std::memcpy(concmd_dup + 0x20, &key_dup_address, sizeof(key_dup_address));
    unsigned char concmd_before_a[0x60], concmd_before_b[0x60], concmd_before_c[0x60], concmd_before_dup[0x60];
    std::memcpy(concmd_before_a, concmd_a, sizeof(concmd_before_a));
    std::memcpy(concmd_before_b, concmd_b, sizeof(concmd_before_b));
    std::memcpy(concmd_before_c, concmd_c, sizeof(concmd_before_c));
    std::memcpy(concmd_before_dup, concmd_dup, sizeof(concmd_before_dup));
    const std::uint64_t concmd_a_args[2] = {reinterpret_cast<std::uint64_t>(concmd_a),
                                            reinterpret_cast<std::uint64_t>(concmd_container)};
    const std::uint64_t concmd_b_args[2] = {reinterpret_cast<std::uint64_t>(concmd_b),
                                            reinterpret_cast<std::uint64_t>(concmd_container)};
    const std::uint64_t concmd_c_args[2] = {reinterpret_cast<std::uint64_t>(concmd_c),
                                            reinterpret_cast<std::uint64_t>(concmd_container)};
    const std::uint64_t concmd_dup_args[2] = {reinterpret_cast<std::uint64_t>(concmd_dup),
                                              reinterpret_cast<std::uint64_t>(concmd_container)};
    CHECK(guest_thunk_invoke(0x20d80, concmd_a_args, &result));
    CHECK(guest_thunk_invoke(0x20d80, concmd_b_args, &result));
    CHECK(guest_thunk_invoke(0x20d80, concmd_c_args, &result));
    CHECK(guest_thunk_invoke(0x20d80, concmd_dup_args, &result));
    CHECK(guest_concmd_registered_count(concmd_container) == 3);
    CHECK(guest_concmd_rotation_count(concmd_container) >= 1);
    CHECK(guest_concmd_root_node(concmd_container) != 0);
    CHECK(std::memcmp(concmd_before_a, concmd_a, sizeof(concmd_before_a)) == 0);
    CHECK(std::memcmp(concmd_before_b, concmd_b, sizeof(concmd_before_b)) == 0);
    CHECK(std::memcmp(concmd_before_c, concmd_c, sizeof(concmd_before_c)) == 0);
    CHECK(std::memcmp(concmd_before_dup, concmd_dup, sizeof(concmd_before_dup)) == 0);

    // 0x207d0 is the one-byte ret at vtable 0x1594d60 slot +0x18, called at 0x20a9e. Its body
    // must leave the caller's canary untouched.
    Object slot_canary{};
    std::memset(slot_canary.bytes, 0xa5, sizeof(slot_canary.bytes));
    const std::uint64_t slot_args[6] = {
        reinterpret_cast<std::uint64_t>(&slot_canary), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x207d0, slot_args, &result));
    CHECK(result == 0);
    for (unsigned char byte : slot_canary.bytes) CHECK(byte == 0xa5);

    // 0x25c90 is the one-byte ret at vtable 0x1595100 slot +0x18, called at 0x26199. Its body
    // must leave the caller's canary untouched.
    Object slot2_canary{};
    std::memset(slot2_canary.bytes, 0x5a, sizeof(slot2_canary.bytes));
    const std::uint64_t slot2_args[6] = {
        reinterpret_cast<std::uint64_t>(&slot2_canary), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x25c90, slot2_args, &result));
    CHECK(result == 0);
    for (unsigned char byte : slot2_canary.bytes) CHECK(byte == 0x5a);

    // 0x23370 is the one-byte ret at vtable 0x1594e78 slot +0x18, called at 0x23c2e. Its body
    // must leave the caller's canary untouched.
    Object slot3_canary{};
    std::memset(slot3_canary.bytes, 0x3c, sizeof(slot3_canary.bytes));
    const std::uint64_t slot3_args[6] = {
        reinterpret_cast<std::uint64_t>(&slot3_canary), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x23370, slot3_args, &result));
    CHECK(result == 0);
    for (unsigned char byte : slot3_canary.bytes) CHECK(byte == 0x3c);

    // 0x235c0 is the one-byte ret at vtable 0x1594e98 slot +0x18, called at 0x23a84. Its body
    // must leave the caller's canary untouched.
    Object slot4_canary{};
    std::memset(slot4_canary.bytes, 0x77, sizeof(slot4_canary.bytes));
    const std::uint64_t slot4_args[6] = {
        reinterpret_cast<std::uint64_t>(&slot4_canary), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x235c0, slot4_args, &result));
    CHECK(result == 0);
    for (unsigned char byte : slot4_canary.bytes) CHECK(byte == 0x77);

    // 0x16a5d0 is the one-byte ret at vtable 0x1598780 slot +0x18, called at 0x16aa97. Its body
    // must leave the caller's canary untouched.
    Object slot5_canary{};
    std::memset(slot5_canary.bytes, 0x99, sizeof(slot5_canary.bytes));
    const std::uint64_t slot5_args[6] = {
        reinterpret_cast<std::uint64_t>(&slot5_canary), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x16a5d0, slot5_args, &result));
    CHECK(result == 0);
    for (unsigned char byte : slot5_canary.bytes) CHECK(byte == 0x99);

    // Null container takes 0x20e7f: append both original nodes to the global sentinel list
    // without reading a key. The second node links behind the first node's +0x8 link.
    alignas(8) unsigned char concmd_null_a[0x40] = {};
    alignas(8) unsigned char concmd_null_b[0x40] = {};
    std::memset(concmd_null_a, 0x55, sizeof(concmd_null_a));
    std::memset(concmd_null_b, 0x66, sizeof(concmd_null_b));
    const std::uint64_t concmd_null_a_args[2] = {reinterpret_cast<std::uint64_t>(concmd_null_a), 0};
    const std::uint64_t concmd_null_b_args[2] = {reinterpret_cast<std::uint64_t>(concmd_null_b), 0};
    CHECK(guest_thunk_invoke(0x20d80, concmd_null_a_args, &result));
    CHECK(guest_thunk_invoke(0x20d80, concmd_null_b_args, &result));
    CHECK(guest_concmd_global_count() == 2);
    std::uint64_t null_a_link = 0, null_a_prev = 0, null_b_prev = 0;
    std::memcpy(&null_a_link, concmd_null_a + 0x8, sizeof(null_a_link));
    std::memcpy(&null_a_prev, concmd_null_a + 0x10, sizeof(null_a_prev));
    std::memcpy(&null_b_prev, concmd_null_b + 0x10, sizeof(null_b_prev));
    CHECK(null_a_link == reinterpret_cast<std::uint64_t>(concmd_null_b + 0x8));
    CHECK(null_b_prev == reinterpret_cast<std::uint64_t>(concmd_null_a + 0x8));
    CHECK(null_a_prev != 0);

    // 0x9844b0 is a tail jump to imported printf with the pinned "%s" format. Its third
    // argument is forwarded as the string; both resolved indirect sites ignore the return.
    g_printf_text[0] = 0;
    guest_set_printf_hook(printf_hook);
    const char* printf_message = "retail-printf";
    const std::uint64_t printf_args[3] = {0, 0,
        reinterpret_cast<std::uint64_t>(printf_message)};
    CHECK(guest_thunk_invoke(0x9844b0, printf_args, &result));
    CHECK(std::strcmp(g_printf_text, printf_message) == 0);
    CHECK(result == std::strlen(printf_message));
    guest_set_printf_hook(nullptr);

    // Direct invocation of 0x20d60 keeps the same observable contract.
    Object direct{};
    const std::uint64_t direct_args[6] = {reinterpret_cast<std::uint64_t>(&direct), 0, 0, 0, 0, 0};
    CHECK(guest_thunk_invoke(0x20d60, direct_args, &result));
    CHECK(direct.bytes[0x18] == 1);

    const std::uint64_t unknown_args[6] = {0, 0, 0, 0, 0, 0};
    CHECK(!guest_thunk_invoke(0x12345678, unknown_args, &result));

    // A direct-call-only byte setter: 0xa431e0 writes sil at object+0x51 and leaves the
    // neighboring bytes alone. Its only mapped caller proves rax dead after the call.
    Object byte_canary{};
    std::memset(byte_canary.bytes, 0x5a, sizeof(byte_canary.bytes));
    const std::uint64_t byte_args[2] = {reinterpret_cast<std::uint64_t>(&byte_canary), 0x1ff};
    CHECK(guest_thunk_invoke(0xa431e0, byte_args, &result));
    CHECK(result == 0);
    CHECK(byte_canary.bytes[0x51] == 0xff);
    CHECK(byte_canary.bytes[0x50] == 0x5a && byte_canary.bytes[0x52] == 0x5a);

    std::printf("OK test_guest_thunks: thunks=%zu stubs=%d setter=0x%x byte=0x%x full=%d const=0x%llx ctor_copy=0x%llx "
                "ctor_const=0x%llx dispatch=0x%llx flag=%u hooks=%d\n",
                guest_thunk_count(), stubs_called, load32(setter, 0x1c),
                byte_canary.bytes[0x51], 1,
                static_cast<unsigned long long>(1),
                static_cast<unsigned long long>(load64(dst, 0x10)),
                static_cast<unsigned long long>(load64(dst, 0x20)),
                static_cast<unsigned long long>(target), command.bytes[0x18], g_hook_calls);
    return 0;
}
