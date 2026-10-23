// Fixture for the native lift of original RVA 0x1290 (Init 0x531d60 callee).
// Asserts the recovered effect order, descriptor contents, the exact vcall argument
// (&A + 0x20) and the fail-visible behaviour when a subsystem hook is absent.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/container_1bdb10.hpp"
#include "core/init_1290.hpp"
#include "core/insert_21cd0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

struct Recorder {
    std::vector<int> order;
    const HandlerRegistration1290* reg_a = nullptr;
    const HandlerRegistration1290* reg_b = nullptr;
    void* self_arg = nullptr;
    void* owner_seen = nullptr;
};

Recorder g;

void hook_insert_a(void* owner, const HandlerRegistration1290& reg) {
    g.order.push_back(1);
    g.owner_seen = owner;
    g.reg_a = &reg;
}

void hook_vcall(void* self_arg) {
    g.order.push_back(2);
    g.self_arg = self_arg;
}

void hook_insert_b(void* owner, const HandlerRegistration1290& reg) {
    g.order.push_back(3);
    (void)owner;
    g.reg_b = &reg;
}

Init1290Hooks full_hooks() {
    Init1290Hooks h{};
    h.insert_1bde30 = hook_insert_a;
    h.vcall_1594060_slot3 = hook_vcall;
    h.insert_21cd0 = hook_insert_b;
    return h;
}

void reset() { g = Recorder{}; }

// Fixed-capacity pool for the 0x60 B node, so the insert body runs without a real allocator.
alignas(16) unsigned char g_pool[0x60];

void* pool_alloc(std::size_t bytes) {
    return (bytes == 0x60) ? static_cast<void*>(g_pool) : nullptr;
}

void* pool_alloc_fail(std::size_t) { return nullptr; }

// Fixed pool for B-insert entries (0x20 B each, bump-allocated vector).
alignas(Entry21cd0) unsigned char g_b_pool[sizeof(Entry21cd0) * 4];
Container21cd0 g_cont_b{};

void setup_cont_b() {
    std::memset(&g_cont_b, 0, sizeof(g_cont_b));
    std::memset(g_b_pool, 0, sizeof(g_b_pool));
    g_cont_b.data = reinterpret_cast<Entry21cd0*>(g_b_pool);
    g_cont_b.count = 0;
    g_cont_b.capacity = 4;
    g_cont_b.lock_handle = 0;
    g_cont_b.sync_bump_end();
}

// No-op begin/end hooks for the B insert (the test does not need to observe them).
void noop_begin(void*, const void*) {}
void noop_end(void*) {}

Insert21cd0Hooks b_insert_hooks() {
    Insert21cd0Hooks h{};
    h.begin = noop_begin;
    h.end = noop_end;
    return h;
}

}  // namespace

int main() {
    // ELF-pinned constants.
    CHECK(kFn1290Rva == 0x1290);
    CHECK(kFn1290Size == 195);
    CHECK(kFn1290EndRva == 0x1353);
    CHECK(kFn1290RegAInvoke == 0x1a00 && kFn1290RegBInvoke == 0x1a10);
    CHECK(kFn1290RegAFn == 0xb10 && kFn1290RegBFn == 0xbb0);
    CHECK(kFn1290Table == 0x1594060);
    CHECK(sizeof(HandlerEntry1290) == 0x20);
    CHECK(offsetof(HandlerRegistration1290, self_arg) == 0x20);
    CHECK(offsetof(HandlerRegistration1290, table) == 0x40);

    int self_obj = 0;
    {
        reset();
        const Init1290Result r = init_1290(&self_obj, full_hooks());
        CHECK(r.ok);
        CHECK(r.insert_1bde30_count == 1);
        CHECK(r.vcall_count == 1);
        CHECK(r.insert_21cd0_count == 1);
        // Observed order: A insert -> vcall -> B insert.
        CHECK(g.order.size() == 3 && g.order[0] == 1 && g.order[1] == 2 && g.order[2] == 3);
        CHECK(g.reg_a == &r.reg_a);
        CHECK(g.reg_b == &r.reg_b);

        // Descriptor A: ctx=&self, invoke 0x1a00, reserved 0, fn 0xb10, table 0x1594060.
        CHECK(r.reg_a.entry.ctx == reinterpret_cast<std::uintptr_t>(&self_obj));
        CHECK(r.reg_a.entry.invoke == 0x1a00);
        CHECK(r.reg_a.entry.reserved == 0);
        CHECK(r.reg_a.entry.fn == 0xb10);
        CHECK(r.reg_a.table == 0x1594060);
        CHECK(r.reg_a.zero_48 == 0);

        // Descriptor B: same ctx, invoke 0x1a10, fn 0xbb0.
        CHECK(r.reg_b.entry.ctx == reinterpret_cast<std::uintptr_t>(&self_obj));
        CHECK(r.reg_b.entry.invoke == 0x1a10);
        CHECK(r.reg_b.entry.reserved == 0);
        CHECK(r.reg_b.entry.fn == 0xbb0);

        // The vcall receives exactly &A + 0x20, not &A.
        const void* self_arg_view = r.reg_a.self_arg;
        const void* reg_a_view = &r.reg_a;
        const void* reg_a_plus_20 =
            static_cast<const void*>(reinterpret_cast<const char*>(&r.reg_a) + 0x20);
        CHECK(g.self_arg == self_arg_view);
        CHECK(g.self_arg == reg_a_plus_20);
        CHECK(g.self_arg != reg_a_view);
        CHECK(r.vcall_self_arg == g.self_arg);
    }

    // A missing subsystem hook is a visible failure, not a fake success, and stops
    // the sequence exactly where the original would fault.
    {
        reset();
        Init1290Hooks h = full_hooks();
        h.insert_1bde30 = nullptr;
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(!r.ok);
        CHECK(r.insert_1bde30_count == 0 && r.vcall_count == 0 && r.insert_21cd0_count == 0);
        CHECK(g.order.empty());
    }
    {
        reset();
        Init1290Hooks h = full_hooks();
        h.vcall_1594060_slot3 = nullptr;
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(!r.ok);
        CHECK(r.insert_1bde30_count == 1 && r.vcall_count == 0 && r.insert_21cd0_count == 0);
        CHECK(g.order.size() == 1 && g.order[0] == 1);
    }
    {
        reset();
        Init1290Hooks h = full_hooks();
        h.insert_21cd0 = nullptr;
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(!r.ok);
        CHECK(r.insert_1bde30_count == 1 && r.vcall_count == 1 && r.insert_21cd0_count == 0);
        CHECK(g.order.size() == 2);
    }

    // Real A-insert path: the lifted 0x1beeb0 body executes the 0x20 B copy and list link,
    // so the insert is an executed effect rather than an observed hook.
    {
        // The container is built by the lifted ctor 0x1bdb10 (E534), not hand-initialised.
        Container1beeb0 cont{};
        init_container_1bdb10(cont);
        Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&cont.list3_prev);
        CHECK(cont.list3_prev == sentinel && cont.list3_next == sentinel);
        CHECK(cont.count3 == 0);

        Init1290Hooks h = full_hooks();
        h.container_a = &cont;
        h.alloc_a.alloc = pool_alloc;

        reset();
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(r.ok);
        CHECK(r.insert_a_executed);
        CHECK(r.inserted_a.ok);
        CHECK(r.inserted_a.node == reinterpret_cast<Node1beeb0*>(g_pool));
        CHECK(r.inserted_a.self_linked);  // fn 0xb10 != 0
        CHECK(r.inserted_a.node->obj ==
              reinterpret_cast<std::uintptr_t>(&r.inserted_a.node->obj));
        CHECK(r.inserted_a.node->invoke == 0x1a00);
        CHECK(r.inserted_a.node->fn == 0xb10);
        CHECK(r.inserted_a.node->vtable == 0x1598ce0);
        CHECK(r.inserted_a.node->slot_58 == 0);  // A+0x48
        CHECK(cont.count3 == 1);
        CHECK(cont.list3_next == r.inserted_a.node);
        CHECK(r.inserted_a.node->prev == sentinel);
        // The insert hook was NOT used: only the vcall and B insert ran.
        CHECK(g.order.size() == 2 && g.order[0] == 2 && g.order[1] == 3);
    }

    // Allocation failure is visible and leaves the list untouched.
    {
        Container1beeb0 cont{};
        Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&cont.list3_prev);
        cont.list3_prev = sentinel;
        cont.list3_next = sentinel;

        Init1290Hooks h = full_hooks();
        h.container_a = &cont;
        h.alloc_a.alloc = pool_alloc_fail;

        reset();
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(!r.ok);
        CHECK(!r.insert_a_executed);
        CHECK(cont.count3 == 0);
        CHECK(cont.list3_next == sentinel);
        CHECK(r.insert_1bde30_count == 0);
        CHECK(g.order.empty());
    }

    // Real B-insert path: the lifted 0x21cd0 body executes the bump-allocator insert
    // and self-link, so both A and B inserts are executed effects.
    {
        setup_cont_b();
        Container1beeb0 cont_a{};
        Node1beeb0* const sentinel_a = reinterpret_cast<Node1beeb0*>(&cont_a.list3_prev);
        cont_a.list3_prev = sentinel_a;
        cont_a.list3_next = sentinel_a;

        Init1290Hooks h = full_hooks();
        h.container_a = &cont_a;
        h.alloc_a.alloc = pool_alloc;
        h.container_b = &g_cont_b;
        h.insert_b_hooks = b_insert_hooks();

        reset();
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(r.ok);
        CHECK(r.insert_a_executed);
        CHECK(r.insert_b_executed);
        CHECK(r.inserted_a.ok);
        CHECK(r.inserted_b.ok);
        // Both inserts ran the lifted bodies; neither hook was used.
        // The A hook was not called (real path), vcall ran, B hook not called.
        CHECK(g.order.size() == 1 && g.order[0] == 2);  // only vcall

        // A insert: list-3, 0x60 B node.
        CHECK(r.inserted_a.node != nullptr);
        CHECK(r.inserted_a.node->invoke == 0x1a00);
        CHECK(cont_a.count3 == 1);

        // B insert: bump-allocator, 0x20 B entry, self-linked.
        CHECK(r.inserted_b.entry != nullptr);
        CHECK(r.inserted_b.entry == &g_cont_b.data[0]);
        CHECK(r.inserted_b.entry->invoke == 0x1a10);
        CHECK(r.inserted_b.entry->fn == 0xbb0);
        CHECK(r.inserted_b.self_linked);
        CHECK(r.inserted_b.entry->ctx ==
              reinterpret_cast<std::uintptr_t>(&r.inserted_b.entry->ctx));
        CHECK(g_cont_b.count == 1);
        CHECK(r.inserted_b.count == 1);
    }

    std::printf("init_1290: all checks passed (order A->vcall->B, 0x20 B entries, &A+0x20 vcall arg, "
                "real A insert (list-3), real B insert (bump-allocator))\n");
    return 0;
}
