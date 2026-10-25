// Fixture for the native lift of original RVA 0x21cd0 (bump-allocator B-insert body
// behind Init 0x1290). Asserts the copy width/offset, fn!=0 self-link, bump-pointer
// advance, capacity-exhaustion grow path, the begin/end hook arguments (including
// the {code,code,int 0xea} aggregate), and that allocation failure fails visibly.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/insert_21cd0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

// Fixed-capacity pool for the vector entries.
alignas(Entry21cd0) Entry21cd0 g_pool[8];
std::size_t g_pool_used = 0;
bool g_data_is_heap = false;  // tracks if container.data is heap-allocated

void reset_pool() {
    std::memset(g_pool, 0xFE, sizeof(g_pool));
    g_pool_used = 0;
    g_data_is_heap = false;
}

// Begin/end hook recording.
struct BeginRecord {
    void* lock_handle = nullptr;
    std::uintptr_t code1 = 0;
    std::uintptr_t code2 = 0;
    std::uint32_t exception_code = 0;
};
BeginRecord g_begin{};
int g_begin_calls = 0;
int g_end_calls = 0;
void* g_end_lock = nullptr;
bool g_grow_ok = true;
int g_grow_calls = 0;
const Descriptor21cd0* g_grow_descriptor = nullptr;

void hook_begin(void* lock_handle, const void* aggregate) {
    ++g_begin_calls;
    g_begin.lock_handle = lock_handle;
    const auto* agg = static_cast<const std::uintptr_t*>(aggregate);
    g_begin.code1 = agg[0];
    g_begin.code2 = agg[1];
    g_begin.exception_code = static_cast<std::uint32_t>(agg[2]);
}

void hook_end(void* lock_handle) {
    ++g_end_calls;
    g_end_lock = lock_handle;
}

bool hook_grow(Container21cd0& container, const Descriptor21cd0& descriptor) {
    ++g_grow_calls;
    g_grow_descriptor = &descriptor;
    if (!g_grow_ok) return false;
    // Double capacity (min 1), allocate new storage, copy old entries.
    std::size_t new_cap = container.capacity == 0 ? 1 : container.capacity * 2;
    auto* new_data = new Entry21cd0[new_cap];
    for (std::size_t i = 0; i < container.count; ++i) {
        new_data[i] = container.data[i];
    }
    if (g_data_is_heap) {
        delete[] container.data;
    }
    g_data_is_heap = true;
    container.data = new_data;
    container.capacity = new_cap;
    // Now insert the new entry at position `count`.
    Entry21cd0* slot = container.data + container.count;
    std::memset(slot, 0, sizeof(Entry21cd0));
    slot->ctx = descriptor.ctx;
    slot->invoke = descriptor.invoke;
    slot->offset = descriptor.reserved;
    slot->fn = descriptor.fn;
    ++container.count;
    container.sync_bump_end();
    return true;
}

void reset_hooks() {
    g_begin = BeginRecord{};
    g_begin_calls = 0;
    g_end_calls = 0;
    g_end_lock = nullptr;
    g_grow_ok = true;
    g_grow_calls = 0;
    g_grow_descriptor = nullptr;
}

Insert21cd0Hooks full_hooks() {
    Insert21cd0Hooks h{};
    h.begin = hook_begin;
    h.end = hook_end;
    h.grow = hook_grow;
    return h;
}

void init_container(Container21cd0& c, std::size_t cap) {
    std::memset(&c, 0, sizeof(c));
    reset_pool();
    c.data = g_pool;
    c.count = 0;
    c.capacity = cap;
    c.lock_handle = 0xBEEFCAFEull;
    c.sync_bump_end();
    g_data_is_heap = false;
}

}  // namespace

int main() {
    // ELF-pinned constants.
    CHECK(kInsert21cd0Rva == 0x21cd0);
    CHECK(kInsert21cd0Size == 204);
    CHECK(kInsert21cd0EndRva == 0x21d9c);
    CHECK(kInsert21cd0BeginThunk == 0x3b390);
    CHECK(kInsert21cd0EndThunk == 0x3b3c0);
    CHECK(kInsert21cd0GrowFn == 0x23210);
    CHECK(kInsert21cd0GuardCell == 0x15fe518);
    CHECK(kInsert21cd0StackChkFail == 0xf8f3a0);
    CHECK(kInsert21cd0Code1 == 0xf95df0);
    CHECK(kInsert21cd0Code2 == 0xf95c06);
    CHECK(kInsert21cd0ExceptionCode == 0xea);
    CHECK(kInsert21cd0EntrySize == 0x20);
    CHECK(kInsert21cd0LockOffset == 0x58);
    CHECK(kInsert21cd0GrowArgOffset == 0x38);
    CHECK(kInsert21cd0BumpOffset == 0x40);
    CHECK(kInsert21cd0EndOffset == 0x48);
    CHECK(kInsert21cd0CopySize == 0x20);
    CHECK(sizeof(Entry21cd0) == 0x20);
    CHECK(sizeof(Descriptor21cd0) == 0x20);
    CHECK(offsetof(Entry21cd0, ctx) == 0x00);
    CHECK(offsetof(Entry21cd0, fn) == 0x18);

    // --- Single insert, fn != 0: begin→insert→self-link→end. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 4);
        Descriptor21cd0 d{};
        d.ctx = 0x1111111111111111ull;
        d.invoke = 0x2222222222222222ull;
        d.reserved = 0x3333333333333333ull;
        d.fn = 0x4444444444444444ull;

        const Insert21cd0Result r = insert_21cd0(c, d, full_hooks());
        CHECK(r.ok);
        CHECK(r.count == 1);
        CHECK(c.count == 1);
        CHECK(r.self_linked);
        CHECK(r.begin_called);
        CHECK(r.end_called);
        CHECK(g_begin_calls == 1);
        CHECK(g_end_calls == 1);
        CHECK(g_grow_calls == 0);

        // Begin hook received the {code,code,int 0xea} aggregate.
        CHECK(g_begin.lock_handle == &c.lock_handle);
        CHECK(g_begin.code1 == kInsert21cd0Code1);
        CHECK(g_begin.code2 == kInsert21cd0Code2);
        CHECK(g_begin.exception_code == kInsert21cd0ExceptionCode);

        // End hook received lock handle.
        CHECK(g_end_lock == &c.lock_handle);

        // Entry: field-level check (copy width = 0x20, self-link, bump position).
        CHECK(r.entry != nullptr);
        CHECK(r.entry == &c.data[0]);
        CHECK(r.entry->invoke == d.invoke);
        CHECK(r.entry->offset == d.reserved);
        CHECK(r.entry->fn == d.fn);
        // Self-link: ctx == &entry->ctx (fn != 0).
        CHECK(r.entry->ctx == reinterpret_cast<std::uintptr_t>(&r.entry->ctx));
        CHECK(r.entry->ctx != d.ctx);  // was overwritten by self-link

        // Bump pointer advanced by 0x20.
        std::uintptr_t bump_after = reinterpret_cast<std::uintptr_t>(c.data + 1);
        CHECK(c._bump == bump_after);
        CHECK(c._end == reinterpret_cast<std::uintptr_t>(c.data + 4));
    }

    // --- Single insert, fn == 0: no self-link, ctx preserved. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 4);
        Descriptor21cd0 d{};
        d.ctx = 0x1111111111111111ull;
        d.invoke = 0x2222222222222222ull;
        d.reserved = 0x3333333333333333ull;
        d.fn = 0;  // no self-link

        const Insert21cd0Result r = insert_21cd0(c, d, full_hooks());
        CHECK(r.ok);
        CHECK(!r.self_linked);
        CHECK(r.entry->ctx == d.ctx);  // preserved, not self-linked
        CHECK(r.entry->fn == 0);
        CHECK(c.count == 1);
    }

    // --- Multiple inserts: bump pointer advances, order preserved. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 8);
        Descriptor21cd0 a{};
        a.ctx = 0xAA00000000000001ull;
        a.invoke = 0x1a00;
        a.fn = 1;
        Descriptor21cd0 b{};
        b.ctx = 0xBB00000000000002ull;
        b.invoke = 0x2222;
        b.fn = 2;

        const Insert21cd0Result ra = insert_21cd0(c, a, full_hooks());
        CHECK(ra.ok && c.count == 1);
        CHECK(ra.entry == &c.data[0]);
        CHECK(ra.entry->invoke == 0x1a00);

        const Insert21cd0Result rb = insert_21cd0(c, b, full_hooks());
        CHECK(rb.ok && c.count == 2);
        CHECK(rb.entry == &c.data[1]);
        CHECK(rb.entry->invoke == 0x2222);

        // Entries are in insertion order (not LIFO; this is a vector, not a linked list).
        CHECK(c.data[0].invoke == 0x1a00);
        CHECK(c.data[1].invoke == 0x2222);
        CHECK(c.data[0].ctx == reinterpret_cast<std::uintptr_t>(&c.data[0].ctx));  // self-linked
        CHECK(c.data[1].ctx == reinterpret_cast<std::uintptr_t>(&c.data[1].ctx));  // self-linked

        std::uintptr_t bump2 = reinterpret_cast<std::uintptr_t>(c.data + 2);
        CHECK(c._bump == bump2);
    }

    // --- Grow path: capacity exhausted triggers grow hook. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 2);  // only 2 slots
        Descriptor21cd0 d{};
        d.ctx = 1; d.invoke = 2; d.fn = 3;

        // Fill the first 2 slots.
        const Insert21cd0Result r1 = insert_21cd0(c, d, full_hooks());
        CHECK(r1.ok && c.count == 1);
        const Insert21cd0Result r2 = insert_21cd0(c, d, full_hooks());
        CHECK(r2.ok && c.count == 2);
        CHECK(g_grow_calls == 0);

        // Third insert triggers grow.
        Descriptor21cd0 d3{};
        d3.ctx = 0xAABBCCDDEEFF0011ull;
        d3.invoke = 0xCAFE;
        d3.fn = 9;
        const Insert21cd0Result r3 = insert_21cd0(c, d3, full_hooks());
        CHECK(r3.ok);
        CHECK(g_grow_calls == 1);
        CHECK(g_grow_descriptor == &d3);
        CHECK(c.count == 3);
        CHECK(c.capacity >= 4);  // doubled
        CHECK(r3.entry->ctx == reinterpret_cast<std::uintptr_t>(&r3.entry->ctx));  // self-linked
        CHECK(r3.entry->invoke == 0xCAFE);
        CHECK(r3.entry->fn == 9);

        // Old entries preserved.
        CHECK(c.data[0].invoke == 2);
        CHECK(c.data[1].invoke == 2);
        CHECK(c.data[2].invoke == 0xCAFE);
    }

    // --- Grow failure: visible failure, container state not observably corrupt. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 1);
        Descriptor21cd0 d{};
        d.ctx = 1; d.fn = 1;

        const Insert21cd0Result r1 = insert_21cd0(c, d, full_hooks());
        CHECK(r1.ok && c.count == 1);

        g_grow_ok = false;
        const Insert21cd0Result r2 = insert_21cd0(c, d, full_hooks());
        CHECK(!r2.ok);
        CHECK(g_grow_calls == 1);
        // Count NOT incremented beyond the successful insert.
        CHECK(c.count == 1);
    }

    // --- No grow hook → visible failure when full. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 1);
        Descriptor21cd0 d{};
        d.ctx = 1; d.fn = 1;

        const Insert21cd0Result r1 = insert_21cd0(c, d, full_hooks());
        CHECK(r1.ok);

        Insert21cd0Hooks no_grow = full_hooks();
        no_grow.grow = nullptr;
        const Insert21cd0Result r2 = insert_21cd0(c, d, no_grow);
        CHECK(!r2.ok);
        CHECK(c.count == 1);
    }

    // --- Null begin/end hooks: insert still works (hooks are optional). ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 4);
        Descriptor21cd0 d{};
        d.ctx = 42; d.invoke = 43; d.fn = 44;

        Insert21cd0Hooks no_begin_end{};
        no_begin_end.grow = hook_grow;
        const Insert21cd0Result r = insert_21cd0(c, d, no_begin_end);
        CHECK(r.ok);
        CHECK(!r.begin_called);
        CHECK(!r.end_called);
        CHECK(g_begin_calls == 0);
        CHECK(g_end_calls == 0);
        CHECK(r.entry->ctx == reinterpret_cast<std::uintptr_t>(&r.entry->ctx));  // self-linked
    }

    // --- Empty container (capacity 0): triggers grow on first insert. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 0);
        Descriptor21cd0 d{};
        d.ctx = 99; d.fn = 99;

        const Insert21cd0Result r = insert_21cd0(c, d, full_hooks());
        CHECK(r.ok);
        CHECK(g_grow_calls == 1);
        CHECK(c.count == 1);
        CHECK(c.capacity >= 1);
    }

    // --- No grow hook, empty container: fails visibly. ---
    {
        reset_hooks();
        Container21cd0 c{};
        init_container(c, 0);
        Descriptor21cd0 d{};
        d.ctx = 1; d.fn = 1;

        Insert21cd0Hooks no_grow{};
        no_grow.begin = hook_begin;
        no_grow.end = hook_end;
        const Insert21cd0Result r = insert_21cd0(c, d, no_grow);
        CHECK(!r.ok);
        CHECK(c.count == 0);
    }

    std::printf("insert_21cd0: all checks passed (0x20 entry, fn!=0 self-link, bump advance, "
                "begin {code,code,int 0xea} args, grow path, fail-visible)\n");
    return 0;
}
