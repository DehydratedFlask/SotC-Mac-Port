// Lazy bootstrap latch tests (OBS byte 0x2dfaaa0 / gate 0x1284a in AllocatorFront::alloc).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/allocator_bootstrap.hpp"
#include "core/allocator_front.hpp"
#include "core/bucket_allocator.hpp"
#include "core/composite_allocator.hpp"
#include "core/heap_allocator.hpp"
#include "core/page_allocator.hpp"

#define CHECK(c)                                                              \
    do {                                                                      \
        if (!(c)) {                                                           \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);  \
            std::exit(1);                                                     \
        }                                                                     \
    } while (0)

using namespace sotc;

struct BootState {
    CompositeAllocator composite;
    BucketAllocator bucket;
    HeapAllocator heap;
    PageAllocator page;
    int calls = 0;

    BootState(AllocatorFront& front)
        : bucket(front.owners(), kBootstrapBucketHeapSlot),
          heap(front.owners(), kBootstrapHeapHeapSlot),
          page(front.owners(), kBootstrapPageHeapSlot) {}
};

static bool install_boot(AllocatorFront& front, void* user) {
    auto* s = static_cast<BootState*>(user);
    ++s->calls;
    auto r = bootstrap_allocator_hierarchy(front, s->composite, s->bucket, s->heap, s->page);
    return r.has_value();
}

static void test_lazy_installs_on_first_alloc() {
    AllocatorFront front;
    BootState state(front);
    CHECK(!front.bootstrap_latch());
    CHECK(front.heap(1) == nullptr);
    front.set_lazy_bootstrap({install_boot, &state});

    void* p = front.alloc(0x20, 1, 8, kAllocFlagNew);
    CHECK(p);
    CHECK(state.calls == 1);
    CHECK(front.heap(1) == &state.composite);
    CHECK(front.heap(2) == &state.bucket);
    CHECK(state.bucket.stats().live_blocks == 1);
    // Latch stays clear (OBS: 0x13380 does not set 0x2dfaaa0); skip via tbl[1].
    CHECK(!front.bootstrap_latch());

    void* q = front.alloc(0x20, 1, 8, kAllocFlagNew);
    CHECK(q);
    CHECK(state.calls == 1);  // second alloc must not re-enter hook
    front.free(p, kAllocFlagNew);
    front.free(q, kAllocFlagNew);
}

static void test_latch_skips_even_when_empty() {
    AllocatorFront front;
    BootState state(front);
    front.set_lazy_bootstrap({install_boot, &state});
    front.set_bootstrap_latch(true);  // OBS 0x110d3 analogue
    CHECK(front.alloc(0x20, 1, 8, kAllocFlagNew) == nullptr);
    CHECK(state.calls == 0);
    CHECK(front.heap(1) == nullptr);

    front.set_bootstrap_latch(false);  // OBS 0x11f94 analogue
    void* p = front.alloc(0x20, 1, 8, kAllocFlagNew);
    CHECK(p && state.calls == 1);
    front.free(p, kAllocFlagNew);
}

static void test_no_hook_still_silent_null() {
    AllocatorFront front;
    CHECK(!front.bootstrap_latch());
    CHECK(front.alloc(8, 1, 8, kAllocFlagNew) == nullptr);  // no hook, empty tbl[1]
}

static void test_overrides_caller_heap_id_after_boot() {
    // OBS: after 0x13380, heap id becomes returned[+0x1c] (slot 1). Caller asked for 5.
    AllocatorFront front;
    BootState state(front);
    front.set_lazy_bootstrap({install_boot, &state});
    void* p = front.alloc(0x400, /*heap*/ 5, 8, kAllocFlagNew);
    CHECK(p);
    CHECK(state.calls == 1);
    // Mid-band went to HeapAllocator via composite@1, not missing slot 5.
    CHECK(state.heap.stats().live_blocks == 1);
    CHECK(front.heap(5) == nullptr);
    front.free(p, kAllocFlagNew);
}

int main() {
    test_no_hook_still_silent_null();
    test_lazy_installs_on_first_alloc();
    test_latch_skips_even_when_empty();
    test_overrides_caller_heap_id_after_boot();
    std::puts("OK allocator_lazy_latch");
    return 0;
}
