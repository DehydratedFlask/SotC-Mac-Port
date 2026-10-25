// Fixture for the container destructor 0x1bede0: list-C payload no-op + free, lists B and A
// release both holders then free, counts/sentinels reset, and the missing-hook failure surface.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/container_dtor_1bede0.hpp"
#include "core/container_1bdb10.hpp"
#include "core/insert_1bdb50.hpp"
#include "core/insert_1bdc80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_allocs = 0;
int g_frees = 0;
int g_payload_destroys = 0;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g_allocs;
    (void)tag;
    (void)align;
    (void)flags;
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool test_deallocate(void* ptr, std::uint32_t tag) {
    ++g_frees;
    CHECK(tag == 2);
    std::free(ptr);
    return true;
}

void free_tag2(void* ptr) {
    CHECK(test_deallocate(ptr, 2));
}

void* delegate_allocate(std::size_t size) {
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

void payload_destroy(void* storage) {
    ++g_payload_destroys;
    (void)storage;
}

}  // namespace

int main() {
    CHECK(kContainerDtor1bede0Rva == 0x1bede0);
    CHECK(kContainerDtor1bede0Size == 208);
    CHECK(kContainerDtor1bede0EndRva == 0x1beeb0);
    CHECK(kContainerDtor1bede0ListCSlot3 == 0x19f0);

    Insert1bdb50Hooks entry_hooks{};
    entry_hooks.alloc.allocate = test_allocate;
    entry_hooks.alloc.deallocate = test_deallocate;
    Insert1beeb0Alloc delegate_alloc{};
    delegate_alloc.alloc = delegate_allocate;
    Insert1bdc80Hooks list_b_hooks{};
    list_b_hooks.alloc.allocate = test_allocate;
    list_b_hooks.alloc.deallocate = test_deallocate;
    list_b_hooks.release = entry_hooks.release;

    // ---- 1. Full teardown: C, B, A in order. ----
    {
        Container1beeb0 container{};
        init_container_1bdb10(container);
        g_allocs = 0;
        g_frees = 0;
        g_payload_destroys = 0;

        Callable1bdeb0 a{};
        a.value_00 = 0x1111;
        Callable1bdeb0 b{};
        b.value_00 = 0x2222;
        List1bdeb0& list_a = *reinterpret_cast<List1bdeb0*>(&container.list1_prev);
        CHECK(insert_1bdb50(list_a, a, b, 1, entry_hooks).ok);
        CHECK(insert_1bdb50(list_a, a, b, 1, entry_hooks).ok);
        CHECK(insert_1bdc80(container, a, b, list_b_hooks).ok);

        Descriptor1beeb0 descriptor{};
        descriptor.ctx = 0x3333;
        descriptor.invoke = 0x4444;
        CHECK(insert_1beeb0(container, descriptor, delegate_alloc).ok);
        CHECK(insert_1beeb0(container, descriptor, delegate_alloc).ok);
        CHECK(container.count3 == 2);
        CHECK(g_allocs == 3);

        ContainerDtor1bede0Hooks hooks{};
        hooks.free_tag2 = free_tag2;
        const auto result = container_dtor_1bede0(container, hooks);
        CHECK(result.ok);
        CHECK(result.freed_c == 2);
        CHECK(result.freed_b == 1);
        CHECK(result.freed_a == 2);
        CHECK(result.releases == 6);  // two per A/B node, three nodes
        CHECK(result.payload_destroys == 0);
        CHECK(result.release_failures == 0);
        CHECK(g_frees == 5);
        CHECK(container.count1 == 0);
        CHECK(container.count2 == 0);
        CHECK(container.count3 == 0);
        CHECK(container.list1_prev == reinterpret_cast<Node1beeb0*>(&container.list1_prev));
        CHECK(container.list1_next == container.list1_prev);
        CHECK(container.list2_prev == reinterpret_cast<Node1beeb0*>(&container.list2_prev));
        CHECK(container.list2_next == container.list2_prev);
        CHECK(container.list3_prev == reinterpret_cast<Node1beeb0*>(&container.list3_prev));
        CHECK(container.list3_next == container.list3_prev);
    }

    // ---- 2. Documented list-C override is called once per node. ----
    {
        Container1beeb0 container{};
        init_container_1bdb10(container);
        Descriptor1beeb0 descriptor{};
        CHECK(insert_1beeb0(container, descriptor, delegate_alloc).ok);
        g_payload_destroys = 0;
        ContainerDtor1bede0Hooks hooks{};
        hooks.free_tag2 = free_tag2;
        hooks.destroy_list_c_payload = payload_destroy;
        const auto result = container_dtor_1bede0(container, hooks);
        CHECK(result.ok);
        CHECK(result.freed_c == 1);
        CHECK(result.payload_destroys == 1);
        CHECK(g_payload_destroys == 1);
    }

    // ---- 3. Missing free hook: visible failure and no mutation. ----
    {
        Container1beeb0 container{};
        init_container_1bdb10(container);
        Descriptor1beeb0 descriptor{};
        CHECK(insert_1beeb0(container, descriptor, delegate_alloc).ok);
        const auto before = container.count3;
        ContainerDtor1bede0Hooks hooks{};
        const auto result = container_dtor_1bede0(container, hooks);
        CHECK(!result.ok);
        CHECK(result.freed_c == 0);
        CHECK(container.count3 == before);
    }

    return 0;
}
