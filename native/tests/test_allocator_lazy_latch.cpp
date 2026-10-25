// Lazy bootstrap latch tests (OBS byte 0x2dfaaa0 / gate 0x1284a in AllocatorFront::alloc).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>

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
    std::atomic<int> calls{0};
    std::atomic<int> waiting{0};
    bool wait_for_two_hooks = false;

    BootState(AllocatorFront& front)
        : bucket(front.owners(), kBootstrapBucketHeapSlot),
          heap(front.owners(), kBootstrapHeapHeapSlot),
          page(front.owners(), kBootstrapPageHeapSlot) {}
};

static bool install_boot(AllocatorFront& front, void* user) {
    auto* s = static_cast<BootState*>(user);
    s->calls.fetch_add(1);
    if (s->wait_for_two_hooks) {
        s->waiting.fetch_add(1);
        while (s->waiting.load() < 2) std::this_thread::yield();
    }
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
    CHECK(state.calls.load() == 1);
    CHECK(front.heap(1) == &state.composite);
    CHECK(front.heap(2) == &state.bucket);
    CHECK(state.bucket.stats().live_blocks == 1);
    // Latch stays clear (OBS: 0x13380 does not set 0x2dfaaa0); skip via tbl[1].
    CHECK(!front.bootstrap_latch());

    void* q = front.alloc(0x20, 1, 8, kAllocFlagNew);
    CHECK(q);
    CHECK(state.calls.load() == 1);  // second alloc must not re-enter hook
    front.free(p, kAllocFlagNew);
    front.free(q, kAllocFlagNew);
}

static void test_latch_skips_even_when_empty() {
    AllocatorFront front;
    BootState state(front);
    front.set_lazy_bootstrap({install_boot, &state});
    front.set_bootstrap_latch(true);  // OBS 0x110d3 analogue
    CHECK(front.alloc(0x20, 1, 8, kAllocFlagNew) == nullptr);
    CHECK(state.calls.load() == 0);
    CHECK(front.heap(1) == nullptr);

    front.set_bootstrap_latch(false);  // OBS 0x11f94 analogue
    void* p = front.alloc(0x20, 1, 8, kAllocFlagNew);
    CHECK(p && state.calls.load() == 1);
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
    CHECK(state.calls.load() == 1);
    // Mid-band went to HeapAllocator via composite@1, not missing slot 5.
    CHECK(state.heap.stats().live_blocks == 1);
    CHECK(front.heap(5) == nullptr);
    front.free(p, kAllocFlagNew);
}

static void test_concurrent_first_allocations_publish_only_complete_hierarchy() {
    AllocatorFront front;
    BootState state(front);
    state.wait_for_two_hooks = true;
    front.set_lazy_bootstrap({install_boot, &state});
    void* allocations[2] = {nullptr, nullptr};

    std::thread first([&] { allocations[0] = front.alloc(0x20, 1, 8, kAllocFlagNew); });
    std::thread second([&] { allocations[1] = front.alloc(0x20, 1, 8, kAllocFlagNew); });
    first.join();
    second.join();

    CHECK(state.calls.load() == 2);  // both callers observed the empty slot before either hook ran
    CHECK(allocations[0] != nullptr && allocations[1] != nullptr);
    CHECK(front.heap(kBootstrapCompositeHeapSlot) == &state.composite);
    CHECK(front.heap(kBootstrapBucketHeapSlot) == &state.bucket);
    CHECK(front.heap(kBootstrapHeapHeapSlot) == &state.heap);
    CHECK(front.heap(kBootstrapPageHeapSlot) == &state.page);
    CHECK(state.composite.child_count() == 3);
    CHECK(state.composite.tracked_allocation_count() == 2);

    front.free(allocations[0], kAllocFlagNew);
    front.free(allocations[1], kAllocFlagNew);
    CHECK(state.composite.tracked_allocation_count() == 0);
}

int main() {
    test_no_hook_still_silent_null();
    test_lazy_installs_on_first_alloc();
    test_latch_skips_even_when_empty();
    test_overrides_caller_heap_id_after_boot();
    test_concurrent_first_allocations_publish_only_complete_hierarchy();
    std::puts("OK allocator_lazy_latch");
    return 0;
}
