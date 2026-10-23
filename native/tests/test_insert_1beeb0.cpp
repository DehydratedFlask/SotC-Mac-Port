// Fixture for the native lift of original RVA 0x1beeb0 (list-3 insert body behind 0x1bde30).
// Asserts the 0x60 B allocation, the 0x20 B field-for-field descriptor copy, the fn!=0
// self-link, the +0x58 store (from descriptor[0x48], not [0x40]), the list-3 push-front
// order, the +0x50 count, and that allocation failure fails visibly.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "core/insert_1beeb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

constexpr std::uintptr_t kGarbage = 0xAAAAAAAAAAAAAAAAull;

// Fixed-capacity pool backing the alloc seam. Pre-filled with garbage so only the fields the
// insert actually writes read back as their intended values.
alignas(Node1beeb0) std::uintptr_t g_pool[0x60 / 8 * 8];  // 8 nodes
std::size_t g_used = 0;
std::size_t g_alloc_size = 0;
int g_alloc_calls = 0;
bool g_fail = false;

void reset_pool() {
    for (std::uintptr_t& w : g_pool) w = kGarbage;
    g_used = 0;
    g_alloc_size = 0;
    g_alloc_calls = 0;
    g_fail = false;
}

void* pool_alloc(std::size_t bytes) {
    ++g_alloc_calls;
    g_alloc_size = bytes;
    if (g_fail) return nullptr;
    if (g_used + bytes > sizeof(g_pool)) return nullptr;
    void* p = reinterpret_cast<std::uintptr_t*>(g_pool) + g_used / sizeof(std::uintptr_t);
    const std::size_t aligned = (bytes + alignof(Node1beeb0) - 1) & ~(alignof(Node1beeb0) - 1);
    g_used += aligned;
    return p;
}

void pool_dealloc(void*) {}

Insert1beeb0Alloc pool_allocator() {
    Insert1beeb0Alloc a{};
    a.alloc = pool_alloc;
    a.dealloc = pool_dealloc;
    return a;
}

void init_list3(Container1beeb0& c) {
    c.list3_prev = reinterpret_cast<Node1beeb0*>(&c.list3_prev);
    c.list3_next = reinterpret_cast<Node1beeb0*>(&c.list3_prev);
    c.count3 = 0;
}

}  // namespace

int main() {
    // ELF-pinned constants.
    CHECK(kInsert1beeb0Rva == 0x1beeb0);
    CHECK(kInsert1beeb0Size == 284);
    CHECK(kInsert1beeb0EndRva == 0x1befcc);
    CHECK(kInsert1beeb0NodeVtable == 0x1598ce0);
    CHECK(kInsert1beeb0DescriptorVtable == 0x1594060);
    CHECK(kInsert1beeb0OperatorNew == 0x17090);
    CHECK(kInsert1beeb0NodeSize == 0x60);
    CHECK(kInsert1beeb0CopySize == 0x20);
    CHECK(kInsert1beeb0CopyOffset == 0x10);
    CHECK(kInsert1beeb0List3Offset == 0x40);
    CHECK(kInsert1beeb0Count3Offset == 0x50);
    CHECK(sizeof(Node1beeb0) == 0x60);
    CHECK(sizeof(Descriptor1beeb0) == 0x50);
    CHECK(sizeof(Container1beeb0) == 0x60);
    CHECK(offsetof(Container1beeb0, list3_prev) == 0x40);
    CHECK(offsetof(Container1beeb0, list3_next) == 0x48);
    CHECK(offsetof(Container1beeb0, count3) == 0x50);

    // --- Single insert, fn != 0: alloc, copy, self-link, +0x58 store, links, count. ---
    {
        reset_pool();
        Container1beeb0 c{};
        init_list3(c);
        Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&c.list3_prev);

        Descriptor1beeb0 d{};
    d.context[2] = 0xCAFEF00Dull;  // E549: lands in node+0x40
        d.ctx = 0x1111111111111111ull;
        d.invoke = 0x2222222222222222ull;
        d.offset = 0x3333333333333333ull;
        d.fn = 0x4444444444444444ull;
        d.vtable = 0x1594060ull;              // read for vcalls, NOT stored into node[0x58]
        d.slot_48 = 0x5555555555555555ull;    // moved to node[0x58]

        const Insert1beeb0Result r = insert_1beeb0(c, d, pool_allocator());
        CHECK(r.ok);
        CHECK(r.node != nullptr);
        CHECK(g_alloc_calls == 1);
        CHECK(g_alloc_size == 0x60);          // allocation size 0x60

        // 0x20 B field-for-field copy (raw, before self-link).
        CHECK(r.copied_obj == d.ctx);
        CHECK(r.copied_invoke == d.invoke);
        CHECK(r.copied_offset == d.offset);
        CHECK(r.copied_fn == d.fn);

        // Self-link when fn != 0.
        CHECK(r.self_linked);
        CHECK(r.node->invoke == d.invoke);
        CHECK(r.node->offset == d.offset);
        CHECK(r.node->fn == d.fn);
        CHECK(r.node->obj == reinterpret_cast<std::uintptr_t>(&r.node->obj));

        // Fixed field stores.
        CHECK(r.node->vtable == 0x1598ce0);   // node[0x50]
        CHECK(r.node->callable_value == d.context[2]);  // node[0x40] (E549 copy-and-swap)

        // +0x58 store = descriptor[0x48], NOT descriptor[0x40].
        CHECK(r.node->slot_58 == d.slot_48);
        CHECK(r.node->slot_58 != d.vtable);

        // List-3 push-front into an empty list.
        CHECK(r.prev == sentinel);
        CHECK(r.next == sentinel);            // old head was the sentinel itself
        CHECK(r.node->prev == sentinel);
        CHECK(r.node->next == sentinel);
        CHECK(c.list3_next == r.node);
        CHECK(c.list3_prev == r.node);

        // Count at +0x50.
        CHECK(r.count == 1);
        CHECK(c.count3 == 1);
    }

    // --- Single insert, fn == 0: no self-link. ---
    {
        reset_pool();
        Container1beeb0 c{};
        init_list3(c);

        Descriptor1beeb0 d{};
        d.ctx = 0x1111111111111111ull;
        d.invoke = 0x2222222222222222ull;
        d.offset = 0x3333333333333333ull;
        d.fn = 0;                            // fn == 0
        d.vtable = 0x1594060ull;
        d.slot_48 = 0x5555555555555555ull;

        const Insert1beeb0Result r = insert_1beeb0(c, d, pool_allocator());
        CHECK(r.ok);
        CHECK(!r.self_linked);
        CHECK(r.node->obj == d.ctx);          // obj keeps the copied ctx, no self-link
        CHECK(r.node->fn == 0);
        CHECK(c.count3 == 1);
    }

    // --- Two inserts: LIFO push-front order, count == 2. ---
    {
        reset_pool();
        Container1beeb0 c{};
        init_list3(c);
        Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&c.list3_prev);

        Descriptor1beeb0 a{};
        a.ctx = 0xAA00000000000001ull;
        a.fn = 1;
        a.slot_48 = 0xAAAA000000000001ull;
        Descriptor1beeb0 b{};
        b.ctx = 0xBB00000000000002ull;
        b.fn = 1;
        b.slot_48 = 0xBBBB000000000002ull;

        const Insert1beeb0Result ra = insert_1beeb0(c, a, pool_allocator());
        CHECK(ra.ok && c.count3 == 1);
        Node1beeb0* const node_a = ra.node;

        const Insert1beeb0Result rb = insert_1beeb0(c, b, pool_allocator());
        CHECK(rb.ok && c.count3 == 2);
        Node1beeb0* const node_b = rb.node;
        CHECK(node_b != node_a);

        // Head is the most-recently inserted (B); order sentinel -> B -> A -> sentinel.
        CHECK(c.list3_next == node_b);
        CHECK(node_b->prev == sentinel);
        CHECK(node_b->next == node_a);
        CHECK(node_a->prev == node_b);
        CHECK(node_a->next == sentinel);
        CHECK(c.list3_prev == node_a);        // tail is the first inserted (A)
        CHECK(node_a->slot_58 == a.slot_48);
        CHECK(node_b->slot_58 == b.slot_48);
    }

    // --- Allocation failure: visible failure, container untouched. ---
    {
        reset_pool();
        Container1beeb0 c{};
        init_list3(c);
        Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&c.list3_prev);

        Descriptor1beeb0 d{};
        d.ctx = 1; d.invoke = 2; d.fn = 3; d.slot_48 = 4;

        Insert1beeb0Alloc failing = pool_allocator();
        g_fail = true;
        const Insert1beeb0Result r = insert_1beeb0(c, d, failing);
        CHECK(!r.ok);
        CHECK(r.node == nullptr);
        CHECK(c.count3 == 0);                 // count NOT incremented
        CHECK(c.list3_next == sentinel);      // list NOT mutated
        CHECK(c.list3_prev == sentinel);
    }

    // --- Null alloc hook: visible failure, no allocation attempt. ---
    {
        reset_pool();
        Container1beeb0 c{};
        init_list3(c);
        Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&c.list3_prev);

        Descriptor1beeb0 d{};
        d.ctx = 1; d.fn = 1;

        const Insert1beeb0Result r = insert_1beeb0(c, d, Insert1beeb0Alloc{});
        CHECK(!r.ok);
        CHECK(r.node == nullptr);
        CHECK(g_alloc_calls == 0);
        CHECK(c.count3 == 0);
        CHECK(c.list3_next == sentinel);
    }

    std::printf("insert_1beeb0: all checks passed (0x60 alloc, 0x20 copy, fn!=0 self-link, "
                "+0x48->+0x58 store, LIFO list order, +0x50 count, fail-visible alloc)\n");
    return 0;
}
