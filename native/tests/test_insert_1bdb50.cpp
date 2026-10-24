// Fixture for the 0x58-byte-node inserter 0x1bdb50, including a round-trip through the 0x1bdeb0
// destructor that consumes the same list.
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/insert_1bdb50.hpp"
#include "core/list_dtor_1bdeb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
struct AllocLog {
    int allocs = 0;
    int frees = 0;
    std::size_t last_size = 0;
    std::uint32_t last_tag = 0;
    std::uint32_t last_align = 0;
    std::uint32_t last_flags = 0;
    std::vector<std::uint32_t> free_tags;
    bool fail_next = false;
} g_log;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g_log.allocs;
    g_log.last_size = size;
    g_log.last_tag = tag;
    g_log.last_align = align;
    g_log.last_flags = flags;
    if (g_log.fail_next) {
        return nullptr;
    }
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool test_deallocate(void* ptr, std::uint32_t tag) {
    ++g_log.frees;
    g_log.free_tags.push_back(tag);
    std::free(ptr);
    return true;
}

// The destructor's free hook is tag-less; route it through the same log as tag 2.
void dtor_free_tag2(void* p) { CHECK(test_deallocate(p, 2)); }

int g_destroyed = 0;
void count_destroy(void* obj, int) { ++g_destroyed; (void)obj; }
void free_control(void* p) { std::free(p); }

std::atomic<std::uint64_t>* new_control(std::uint64_t n) {
    auto* c = static_cast<std::atomic<std::uint64_t>*>(std::malloc(sizeof(std::atomic<std::uint64_t>)));
    CHECK(c != nullptr);
    new (c) std::atomic<std::uint64_t>(n);
    return c;
}

// The caller gives its own reference away once the node owns one, so a later release reaches zero.
void handover(std::atomic<std::uint64_t>* c) {
    CHECK(c->load() == 2);
    c->fetch_sub(1);
    CHECK(c->load() == 1);
}

}  // namespace

int main() {
    CHECK(kInsert1bdb50Rva == 0x1bdb50);
    CHECK(kInsert1bdb50Size == 0x130);
    CHECK(kInsert1bdb50EndRva == 0x1bdc80);
    CHECK(kInsert1bdb50AllocRva == 0x17090);
    CHECK(kInsert1bdb50ReleaseRva == 0xf590);
    CHECK(kInsert1bdb50NodeSize == 0x58);
    CHECK(kInsert1bdb50FlagOffset == 0x50);
    CHECK(kInsert1bdb50CountOffset == 0x10);
    CHECK(sizeof(Node1bdb50) == 0x58);
    CHECK(offsetof(Node1bdb50, callable_10) == 0x10);
    CHECK(offsetof(Node1bdb50, callable_30) == 0x30);

    Insert1bdb50Hooks hooks{};
    hooks.alloc.allocate = test_allocate;
    hooks.alloc.deallocate = test_deallocate;
    hooks.release.destroy = count_destroy;
    hooks.release.free_control = free_control;

    List1bdb50 list{};
    Node1bdb50* const sentinel = reinterpret_cast<Node1bdb50*>(&list);
    list.prev = sentinel;
    list.next = sentinel;

    // ---- 1. Insert into an empty list. ----
    auto* c1a = new_control(1);
    auto* c1b = new_control(1);
    Callable1bdb50 a1{};
    a1.value_00 = 0x1111;
    a1.value_08 = 0x2222;
    a1.value_10 = 0x3333;
    a1.control_18 = c1a;
    Callable1bdb50 b1{};
    b1.value_00 = 0x4444;
    b1.control_18 = c1b;

    g_log = AllocLog{};
    const auto r1 = insert_1bdb50(list, a1, b1, /*flag*/ 7, hooks);
    CHECK(r1.ok);
    CHECK(r1.allocated);
    CHECK(r1.acquires == 2);
    CHECK(r1.temp_release_ok);
    CHECK(r1.node != nullptr);
    CHECK(r1.count_after == 1);
    CHECK(g_log.allocs == 1);
    CHECK(g_log.last_size == 0x58);
    CHECK(g_log.last_tag == 1);
    CHECK(g_log.last_align == 8);
    CHECK(g_log.last_flags == 1);
    // Exactly one surviving reference per callable: the acquire, with the zeroed temp release a no-op.
    CHECK(c1a->load() == 2 && c1b->load() == 2);
    handover(c1a);
    handover(c1b);
    // Payload copied verbatim.
    CHECK(r1.node->callable_10.value_00 == 0x1111);
    CHECK(r1.node->callable_10.value_08 == 0x2222);
    CHECK(r1.node->callable_10.value_10 == 0x3333);
    CHECK(r1.node->callable_10.control_18 == c1a);
    CHECK(r1.node->callable_30.value_00 == 0x4444);
    CHECK(r1.node->callable_30.control_18 == c1b);
    CHECK(r1.node->flag_50 == 7);
    // Linked as sentinel <-> node.
    CHECK(list.next == r1.node && list.prev == r1.node);
    CHECK(r1.node->prev == sentinel && r1.node->next == sentinel);

    // ---- 2. Insert a second node: it becomes the head and the old head's prev is updated. ----
    Callable1bdb50 a2{};
    a2.value_00 = 0x5555;  // no control block
    Callable1bdb50 b2{};
    b2.value_00 = 0x6666;  // no control block
    const auto r2 = insert_1bdb50(list, a2, b2, /*flag*/ 0, hooks);
    CHECK(r2.ok);
    CHECK(r2.acquires == 0);  // null controls take no reference
    CHECK(r2.count_after == 2);
    CHECK(list.next == r2.node);
    CHECK(list.prev == r1.node);
    CHECK(r2.node->next == r1.node);
    CHECK(r1.node->prev == r2.node);
    CHECK(r2.node->prev == sentinel);
    CHECK(r1.node->next == sentinel);

    // ---- 3. Third node, so the round-trip below has a mixed-flag list. ----
    auto* c3 = new_control(1);
    Callable1bdb50 a3{};
    a3.value_00 = 0x7777;
    a3.control_18 = c3;
    Callable1bdb50 b3{};
    b3.value_00 = 0x8888;
    const auto r3 = insert_1bdb50(list, a3, b3, /*flag*/ 9, hooks);
    CHECK(r3.ok);
    CHECK(r3.count_after == 3);
    CHECK(list.next == r3.node);
    handover(c3);

    // ---- 4. Missing allocator: nothing happens at all. ----
    {
        Insert1bdb50Hooks none{};
        none.release = hooks.release;
        const auto bad = insert_1bdb50(list, a1, b1, 0, none);
        CHECK(!bad.ok);
        CHECK(!bad.allocated);
        CHECK(bad.acquires == 0);      // checked before any acquire
        CHECK(list.count == 3);
        CHECK(c1a->load() == 1);
    }

    // ---- 5. Allocation failure: visible failure, refcounts restored, list untouched. ----
    {
        g_log.fail_next = true;
        const auto before = list.count;
        const auto bad = insert_1bdb50(list, a1, b1, 0, hooks);
        CHECK(!bad.ok);
        CHECK(bad.allocated == false);
        CHECK(bad.acquires == 2);
        CHECK(bad.refs_restored);
        CHECK(bad.node == nullptr);
        CHECK(list.count == before);
        CHECK(list.next == r3.node);
        CHECK(c1a->load() == 1 && c1b->load() == 1);  // acquires undone
        g_log.fail_next = false;
    }

    // ---- 6. Round-trip: the destructor consumes the list this inserter built. ----
    // Flags are 7 (r1), 0 (r2), 9 (r3): walking from the tail erases only r2.
    {
        ListDtor1bdeb0Hooks dh{};
        dh.release = hooks.release;
        dh.free_tag2 = dtor_free_tag2;
        g_log.frees = 0;
        g_log.free_tags.clear();
        const auto d = list_dtor_1bdeb0(list, dh);
        CHECK(d.ok);
        CHECK(d.erased == 1);
        CHECK(d.skipped == 2);
        CHECK(d.frees == 1);
        CHECK(d.count_after == 2);
        CHECK(list.count == 2);
        // r2 had no control blocks, so no destroys happened for it; the flagged nodes survive.
        CHECK(list.next == r3.node);
        CHECK(list.prev == r1.node);
        CHECK(r3.node->next == r1.node && r1.node->prev == r3.node);
        CHECK(g_log.free_tags.size() == 1 && g_log.free_tags[0] == 2);
        CHECK(c3->load() == 1);  // untouched: r3 was flagged (node holds the only ref)
    }

    // ---- 7. Tear the remaining nodes down by clearing their flags. ----
    r1.node->flag_50 = 0;
    r3.node->flag_50 = 0;
    {
        ListDtor1bdeb0Hooks dh{};
        dh.release = hooks.release;
        dh.free_tag2 = dtor_free_tag2;
        g_destroyed = 0;
        const auto d = list_dtor_1bdeb0(list, dh);
        CHECK(d.ok);
        CHECK(d.erased == 2);
        CHECK(d.skipped == 0);
        CHECK(d.frees == 2);
        CHECK(d.count_after == 0);
        CHECK(list.next == sentinel && list.prev == sentinel);
        // r1 contributes two destroys (both callables had a live control block); r3 only one,
        // because its B callable never had a control block.
        CHECK(g_destroyed == 3);
        // The control blocks are gone too: a last reference frees them through the release hook, so
        // they must not be touched (or freed again) afterwards.
    }

    std::printf("insert_1bdb50: all checks passed (acquire x2, 0x58 alloc tag1, payload, link, "
                "count++, zeroed-temp releases, failure restore, dtor round-trip)\n");
    return 0;
}
