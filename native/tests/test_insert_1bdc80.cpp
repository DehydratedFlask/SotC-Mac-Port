// Fixture for the list-B inserter 0x1bdc80: 0x50 B nodes, container+0x20 sentinel, count at +0x30,
// reference hand-off and the allocation-failure restore.
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/app_info_registry.hpp"
#include "core/insert_1bdc80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_allocs = 0;
int g_frees = 0;
bool g_fail = false;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g_allocs;
    CHECK(size == 0x50);
    CHECK(tag == 1);
    CHECK(align == 8);
    CHECK(flags == 2);  // 0x17090: the tag-2 family, paired with the 0x170c0 free
    if (g_fail) {
        return nullptr;
    }
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

int g_destroyed = 0;
void count_destroy(void* obj, int) {
    ++g_destroyed;
    (void)obj;
}
void free_control(void* p) { std::free(p); }

std::atomic<std::uint64_t>* new_control(std::uint64_t n) {
    auto* c = static_cast<std::atomic<std::uint64_t>*>(std::malloc(sizeof(std::atomic<std::uint64_t>)));
    CHECK(c != nullptr);
    new (c) std::atomic<std::uint64_t>(n);
    return c;
}
}  // namespace

int main() {
    CHECK(kInsert1bdc80Rva == 0x1bdc80);
    CHECK(kInsert1bdc80NodeSize == 0x50);
    CHECK(kInsert1bdc80ListOffset == 0x20);
    CHECK(kInsert1bdc80CountOffset == 0x30);
    CHECK(sizeof(Node1bdc80) == 0x50);
    CHECK(sizeof(List1bdc80) == 0x20);

    Insert1bdc80Hooks hooks{};
    hooks.alloc.allocate = test_allocate;
    hooks.alloc.deallocate = test_deallocate;
    hooks.release.destroy = count_destroy;
    hooks.release.free_control = free_control;

    AppInfoRegistry registry;
    Container1beeb0& container = registry.container();
    Node1bdc80* const sentinel = reinterpret_cast<Node1bdc80*>(
        reinterpret_cast<std::byte*>(&container) + kInsert1bdc80ListOffset);
    CHECK(sentinel == reinterpret_cast<Node1bdc80*>(&container.list2_prev));
    CHECK(sentinel->prev == sentinel && sentinel->next == sentinel);

    // ---- 1. Insert one node with two controlled holders. ----
    auto* const ca = new_control(1);
    auto* const cb = new_control(1);
    Callable1bdeb0 a{};
    a.value_00 = 0x1111;
    a.value_08 = 2;
    a.control_18 = ca;
    Callable1bdeb0 b{};
    b.value_00 = 0x2222;
    b.control_18 = cb;

    g_allocs = 0;
    const auto first = insert_1bdc80(container, a, b, hooks);
    CHECK(first.ok);
    CHECK(first.allocated);
    CHECK(first.acquires == 4);
    CHECK(first.node != nullptr);
    CHECK(first.count_after == 1);
    CHECK(g_allocs == 1);
    // Node owns one reference per holder (the other three were released).
    CHECK(ca->load() == 2);
    CHECK(cb->load() == 2);
    CHECK(first.node->next == sentinel);
    CHECK(first.node->prev == sentinel);
    CHECK(sentinel->next == first.node);
    CHECK(sentinel->prev == first.node);
    CHECK(first.node->callable_10.value_00 == 0x1111);
    CHECK(first.node->callable_10.value_08 == 2);
    CHECK(first.node->callable_30.value_00 == 0x2222);

    // ---- 2. Second insert becomes the new tail; the first node's next is updated. ----
    Callable1bdeb0 c{};
    c.value_00 = 0x3333;
    const auto second = insert_1bdc80(container, c, b, hooks);
    CHECK(second.ok);
    CHECK(second.count_after == 2);
    CHECK(sentinel->next == first.node);
    CHECK(sentinel->prev == second.node);
    CHECK(first.node->next == second.node);
    CHECK(second.node->prev == first.node);

    // ---- 3. Allocation failure restores all four acquisitions. ----
    g_fail = true;
    const auto before_a = ca->load();
    const auto failed = insert_1bdc80(container, a, b, hooks);
    CHECK(!failed.ok);
    CHECK(!failed.allocated);
    CHECK(failed.node == nullptr);
    CHECK(failed.refs_restored);
    CHECK(failed.count_after == 0);
    CHECK(ca->load() == before_a);
    CHECK(sentinel->prev == second.node);  // list untouched
    g_fail = false;

    // Test cleanup: release the node-held refs and free the nodes.
    for (Node1bdc80* node = sentinel->next; node != sentinel;) {
        Node1bdc80* const next = node->next;
        SharedHolderF590 ha{reinterpret_cast<void*>(node->callable_10.value_00),
                            nullptr, nullptr, node->callable_10.control_18};
        SharedHolderF590 hb{reinterpret_cast<void*>(node->callable_30.value_00),
                            nullptr, nullptr, node->callable_30.control_18};
        (void)shared_release_f590(ha, hooks.release);
        (void)shared_release_f590(hb, hooks.release);
        std::free(node);
        node = next;
    }
    std::free(ca);
    std::free(cb);

    return 0;
}
