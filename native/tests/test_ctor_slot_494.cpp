// CPU-01-AOT-CFG-SCALE-01-BATCH20 (slot 494): fixture for startup constructor index 494 (0x26f7c0): two pool registries
// built by 0x26e340 (0x18-byte nodes) and 0x26e770 (0x20-byte nodes), each with an atexit destructor record
// (0x269590 / 0x269610). The allocator hooks are test doubles that poison every block and log every call.
#include "l4_test_util.hpp"

#include <vector>

#include "core/ctor_slot_494_26f7c0.hpp"
#include "core/ctor_tiny_misc_slots.hpp"

namespace {
using namespace l4;

struct Call { bool alloc; std::size_t size; void* pointer; };
std::vector<Call> g_calls;
std::size_t g_fail_after = ~static_cast<std::size_t>(0);   // allocations allowed before the next one returns null

void* test_allocate(std::size_t size) {
    std::size_t allocs = 0;
    for (const Call& c : g_calls) allocs += c.alloc ? 1 : 0;
    if (allocs >= g_fail_after) return nullptr;
    void* p = std::malloc(size);
    std::memset(p, kPoison, size);
    g_calls.push_back({true, size, p});
    return p;
}
void test_release(void* pointer) {
    g_calls.push_back({false, 0, pointer});
    std::free(pointer);
}
Slot494PoolHooks hooks() {
    Slot494PoolHooks h{};
    h.allocate = &test_allocate;
    h.release = &test_release;
    return h;
}
std::size_t count_calls(bool alloc) {
    std::size_t n = 0;
    for (const Call& c : g_calls) n += c.alloc == alloc ? 1 : 0;
    return n;
}

void section_constants() {
    CHECK(kSlot494PoolsRva == 0x26f7c0 && kSlot494PoolsSize == 84);
    CHECK(kSlot494PoolsObj0Rva == 0x2eef038 && kSlot494PoolsObj1Rva == 0x2eef058);
    CHECK(kSlot494PoolsCtor0Rva == 0x26e340 && kSlot494PoolsCtor1Rva == 0x26e770);
    CHECK(kSlot494PoolsDtor0Rva == 0x269590 && kSlot494PoolsDtor1Rva == 0x269610);
    // The older hook-based identity model of this slot names the same facts.
    CHECK(kSlot494PoolsRva == kSlot494Rva && kSlot494PoolsSize == kSlot494Size && kSlot494PoolsObj0Rva == kSlot494Obj0Rva);
    CHECK(kSlot494PoolsObj1Rva == kSlot494Obj1Rva && kSlot494PoolsCtor0Rva == kSlot494Ctor0Rva && kSlot494PoolsCtor1Rva == kSlot494Ctor1Rva);
    CHECK(kSlot494PoolsDtor0Rva == kSlot494Dtor0Rva && kSlot494PoolsDtor1Rva == kSlot494Dtor1Rva);
    CHECK(kSlot494PoolReserve0Rva == 0x26e5f0 && kSlot494PoolReserve1Rva == 0x26ea10);
    CHECK(kSlot494PtrGrow0Rva == 0x26e4d0 && kSlot494PtrGrow1Rva == 0x26e8f0);
    CHECK(kSlot494PoolBlockSize == 0x28 && kSlot494NodeCount == 0x1000 && kSlot494Node0Size == 0x18 && kSlot494Node1Size == 0x20);
    CHECK(sizeof(Slot494PoolBlock) == 0x28 && sizeof(Slot494Registry) == 0x20);
}

template <class Node>
void check_pool(const Slot494Registry& r, std::size_t node_size, bool has_state2) {
    CHECK(r.begin != nullptr && r.end == r.begin + 1 && r.cap == r.begin + 1);   // 1 element, capacity exactly 1
    Slot494PoolBlock* pool = r.begin[0];
    CHECK(pool != nullptr);
    CHECK(pool->word0 == 0xa5a5a5a5a5a5a5a5ull);                                  // +0 is never written
    CHECK(pool->nodes_begin != nullptr);
    CHECK(static_cast<std::size_t>(pool->nodes_end - pool->nodes_begin) == kSlot494NodeCount * node_size);
    CHECK(pool->nodes_cap == pool->nodes_end);                                    // reserved exactly 0x1000 nodes
    CHECK(pool->free_head == pool->nodes_begin);
    const auto* nodes = reinterpret_cast<const Node*>(pool->nodes_begin);
    for (std::size_t i = 0; i < kSlot494NodeCount; ++i) {
        CHECK(nodes[i].state == 0 && nodes[i].aux == 0);
        CHECK(nodes[i].word_0c == 0xa5a5a5a5u);                                   // the dword at +0xc is never written
        CHECK(nodes[i].next == (i + 1 < kSlot494NodeCount ? &nodes[i + 1] : nullptr));
        if (has_state2) CHECK(reinterpret_cast<const Slot494Node1&>(nodes[i]).state2 == kSlot494NodeStateInit);
    }
}

void section_alone() {
    g_calls.clear();
    g_fail_after = ~static_cast<std::size_t>(0);
    ConCmdRegistry creg;
    Slot494Objects o;
    poison(o);
    std::memset(&o.r0, 0, sizeof o.r0);        // .bss statics
    std::memset(&o.r1, 0, sizeof o.r1);
    o.hooks = hooks();
    const Slot494Result r = init_slot494_pools_26f7c0(o, creg);
    CHECK(r.ok && r.built0 && r.atexit0 && r.built1 && r.atexit1);
    // R+0 stays as it was (the zero .bss word): the builder never writes it.
    CHECK(o.r0.word0 == 0 && o.r1.word0 == 0);
    // Allocation order: pool block, node vector, pointer vector -- once per registry; nothing is freed.
    CHECK(g_calls.size() == 6 && count_calls(false) == 0);
    CHECK(g_calls[0].size == 0x28 && g_calls[1].size == 0x1000 * 0x18 && g_calls[2].size == 8);
    CHECK(g_calls[3].size == 0x28 && g_calls[4].size == 0x1000 * 0x20 && g_calls[5].size == 8);
    check_pool<Slot494Node0>(o.r0, kSlot494Node0Size, false);
    check_pool<Slot494Node1>(o.r1, kSlot494Node1Size, true);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x269590, &o.r0);
    check_atexit(creg, 1, 0x269610, &o.r1);

    // Destructors: node vector, pool block, then the pointer vector; the registry is cleared (R+0 untouched).
    const std::size_t before = g_calls.size();
    Slot494PoolBlock* pool0 = o.r0.begin[0];
    std::byte* nodes0 = pool0->nodes_begin;
    Slot494PoolBlock** vec0 = o.r0.begin;
    CHECK(slot494_destroy_registry(o.r0, o.hooks));
    CHECK(g_calls.size() == before + 3 && !g_calls[before].alloc && !g_calls[before + 1].alloc && !g_calls[before + 2].alloc);
    CHECK(g_calls[before].pointer == nodes0 && g_calls[before + 1].pointer == pool0 && g_calls[before + 2].pointer == vec0);
    CHECK(o.r0.begin == nullptr && o.r0.end == nullptr && o.r0.cap == nullptr && o.r0.word0 == 0);
    CHECK(slot494_destroy_registry(o.r1, o.hooks));
    CHECK(g_calls.size() == before + 6 && count_calls(false) == 6 && count_calls(true) == 6);
    // A second destruction of a cleared registry frees nothing.
    CHECK(slot494_destroy_registry(o.r0, o.hooks) && g_calls.size() == before + 6);
}

void section_destroy_shapes() {
    g_calls.clear();
    // Empty (begin == end non-null): only the vector block is freed, then everything is cleared.
    Slot494Registry r{};
    r.begin = r.end = static_cast<Slot494PoolBlock**>(std::malloc(8));
    r.cap = r.begin + 1;
    CHECK(slot494_destroy_registry(r, hooks()));
    CHECK(g_calls.size() == 1 && r.begin == nullptr && r.end == nullptr && r.cap == nullptr);
    // A null pool entry is skipped; a pool without nodes frees only itself.
    g_calls.clear();
    auto* vec = static_cast<Slot494PoolBlock**>(std::malloc(16));
    auto* pool = static_cast<Slot494PoolBlock*>(std::malloc(sizeof(Slot494PoolBlock)));
    pool->nodes_begin = nullptr;
    vec[0] = nullptr;
    vec[1] = pool;
    r.begin = vec; r.end = vec + 2; r.cap = vec + 2;
    CHECK(slot494_destroy_registry(r, hooks()));
    CHECK(g_calls.size() == 2 && g_calls[0].pointer == pool && g_calls[1].pointer == vec);
    // A fully empty registry (begin == nullptr): nothing is freed; end = begin, cap untouched.
    g_calls.clear();
    Slot494Registry z{};
    z.cap = reinterpret_cast<Slot494PoolBlock**>(0x1000);
    CHECK(slot494_destroy_registry(z, hooks()) && g_calls.empty() && z.end == nullptr && z.cap == reinterpret_cast<Slot494PoolBlock**>(0x1000));
    // A missing release hook fails closed.
    auto* v2 = static_cast<Slot494PoolBlock**>(std::malloc(8));
    r.begin = r.end = v2; r.cap = v2 + 1;
    Slot494PoolHooks none{};
    CHECK(!slot494_destroy_registry(r, none) && r.begin == v2);
    std::free(v2);
}

void section_reserve() {
    g_calls.clear();
    // Enough spare capacity: nothing happens.
    std::byte* store = static_cast<std::byte*>(std::malloc(64));
    std::byte *b = store, *e = store + 24, *c = store + 32;
    CHECK(slot494_reserve_more(b, e, c, 8, 100, 1, hooks()) && b == store && g_calls.empty());
    // Growth: size 3, capacity 4, +2 -> max(5, 4 + 2) = 6 elements; live bytes move; the old block is released.
    for (int i = 0; i < 24; ++i) store[i] = static_cast<std::byte>(i + 1);
    CHECK(slot494_reserve_more(b, e, c, 8, 100, 2, hooks()));
    CHECK(g_calls.size() == 2 && g_calls[0].alloc && g_calls[0].size == 48 && !g_calls[1].alloc && g_calls[1].pointer == store);
    CHECK(e - b == 24 && c - b == 48);
    for (int i = 0; i < 24; ++i) CHECK(b[i] == static_cast<std::byte>(i + 1));
    // Growth wants more than 1.5x: size 3, capacity 3, +10 -> 13.
    g_calls.clear();
    std::byte* s2 = b;
    b = s2; e = s2 + 24; c = s2 + 24;
    CHECK(slot494_reserve_more(b, e, c, 8, 100, 10, hooks()) && c - b == 13 * 8 && e - b == 24);
    // Length error: size + count above the maximum.
    g_calls.clear();
    CHECK(!slot494_reserve_more(b, e, c, 8, 4, 20, hooks()) && g_calls.empty());
    // Allocation failure fails closed and leaves the triple alone.
    std::byte *ob = b, *oe = e, *oc = c;
    g_fail_after = 0;
    CHECK(!slot494_reserve_more(b, e, c, 8, 1000, 100, hooks()) && b == ob && e == oe && c == oc);
    g_fail_after = ~static_cast<std::size_t>(0);
    Slot494PoolHooks none{};
    CHECK(!slot494_reserve_more(b, e, c, 8, 1000, 100, none) && b == ob);
    std::free(ob);
}

void section_failures() {
    ConCmdRegistry creg;
    Slot494Objects o;
    poison(o);
    o.hooks = Slot494PoolHooks{};
    // No allocator at all: the first builder fails after clearing its vector; nothing is recorded.
    const Slot494Result r = init_slot494_pools_26f7c0(o, creg);
    CHECK(!r.ok && !r.built0 && !r.atexit0 && !r.built1 && creg.atexit_log.size() == 0);
    CHECK(o.r0.begin == nullptr && o.r0.end == nullptr && o.r0.cap == nullptr && o.r0.word0 == 0xa5a5a5a5a5a5a5a5ull);
    CHECK(o.r1.begin == reinterpret_cast<Slot494PoolBlock**>(0xa5a5a5a5a5a5a5a5ull));
    // Allocation failure at each of the six allocations: the slot reports failure at the right stage.
    for (std::size_t fail = 0; fail < 6; ++fail) {
        g_calls.clear();
        g_fail_after = fail;
        ConCmdRegistry c2;
        Slot494Objects p;
        poison(p);
        p.hooks = hooks();
        const Slot494Result q = init_slot494_pools_26f7c0(p, c2);
        CHECK(!q.ok);
        CHECK(q.built0 == (fail >= 3) && q.atexit0 == (fail >= 3) && q.built1 == false);
        CHECK(c2.atexit_log.size() == (fail >= 3 ? 1u : 0u));
    }
    g_fail_after = ~static_cast<std::size_t>(0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_destroy_shapes();
    section_reserve();
    section_failures();
    std::puts("OK test_ctor_slot_494");
    return 0;
}
