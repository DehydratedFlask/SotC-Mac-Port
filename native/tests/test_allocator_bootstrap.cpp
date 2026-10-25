// Allocator bootstrap wiring tests (OBS 0x13380).
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

static void test_first_free_slot_scan() {
    AllocatorFront front;
    CHECK(first_free_heap_slot(front, 1) == 1u);
    // Occupy 1 and 2; next free from 1 is 3 (OBS scan start index 1, bound 0x100).
    PageAllocator placeholder(front.owners(), 2);
    CHECK(front.set_heap(1, &placeholder));
    CHECK(front.set_heap(2, &placeholder));
    CHECK(first_free_heap_slot(front, 1) == 3u);
    CHECK(!first_free_heap_slot(front, 0));
    CHECK(!first_free_heap_slot(front, 256));
}

static void test_empty_table_bootstrap_slots() {
    AllocatorFront front;
    BucketAllocator bucket(front.owners(), kBootstrapBucketHeapSlot);
    HeapAllocator heap(front.owners(), kBootstrapHeapHeapSlot);
    PageAllocator page(front.owners(), kBootstrapPageHeapSlot);
    CompositeAllocator composite;

    auto r = bootstrap_allocator_hierarchy(front, composite, bucket, heap, page);
    CHECK(r);
    CHECK(!r->already_installed);
    CHECK(r->composite_slot == kBootstrapCompositeHeapSlot);
    CHECK(r->bucket_slot == kBootstrapBucketHeapSlot);
    CHECK(r->heap_slot == kBootstrapHeapHeapSlot);
    CHECK(r->page_slot == kBootstrapPageHeapSlot);

    CHECK(front.heap(1) == &composite);
    CHECK(front.heap(2) == &bucket);
    CHECK(front.heap(3) == &heap);
    CHECK(front.heap(4) == &page);
    CHECK(composite.child_count() == 3);
    CHECK(composite.children()[0].threshold == kCompositeBucketThreshold);
    CHECK(composite.children()[1].threshold == kCompositeHeapThreshold);
    CHECK(composite.children()[2].threshold == kCompositePageThreshold);
    CHECK(composite.children()[0].heap == &bucket);
    CHECK(composite.children()[1].heap == &heap);
    CHECK(composite.children()[2].heap == &page);

    // Mid-band via default heap 1 (composite).
    void* mid = front.alloc(0x400, 1, 8, kAllocFlagNew);
    CHECK(mid && heap.stats().live_blocks == 1 && bucket.stats().live_blocks == 0);
    front.free(mid, kAllocFlagNew);

    void* small = front.alloc(0x20, 1, 8, kAllocFlagNew);
    CHECK(small && bucket.stats().live_blocks == 1);
    front.free(small, kAllocFlagNew);

    void* big = front.alloc(0x100000, 1, 8, kAllocFlagNew);
    CHECK(big && page.stats().live_blocks == 1);
    front.free(big, kAllocFlagNew);
}

static void test_idempotent_early_out() {
    AllocatorFront front;
    BucketAllocator bucket(front.owners(), kBootstrapBucketHeapSlot);
    HeapAllocator heap(front.owners(), kBootstrapHeapHeapSlot);
    PageAllocator page(front.owners(), kBootstrapPageHeapSlot);
    CompositeAllocator composite;
    CHECK(bootstrap_allocator_hierarchy(front, composite, bucket, heap, page));

    // Second call must not re-add children or move slots.
    auto r2 = bootstrap_allocator_hierarchy(front, composite, bucket, heap, page);
    CHECK(r2 && r2->already_installed);
    CHECK(composite.child_count() == 3);
    CHECK(front.heap(1) == &composite);
    CHECK(front.heap(2) == &bucket);
}

static void test_incomplete_bootstrap_does_not_mutate() {
    AllocatorFront front;
    PageAllocator placeholder(front.owners(), 2);
    for (std::uint32_t slot = 2; slot < AllocatorFront::kHeapSlots; ++slot) {
        CHECK(front.set_heap(slot, &placeholder));
    }

    BucketAllocator bucket(front.owners(), kBootstrapBucketHeapSlot);
    HeapAllocator heap(front.owners(), kBootstrapHeapHeapSlot);
    PageAllocator page(front.owners(), kBootstrapPageHeapSlot);
    CompositeAllocator composite;
    CHECK(!bootstrap_allocator_hierarchy(front, composite, bucket, heap, page));
    CHECK(front.heap(kBootstrapCompositeHeapSlot) == nullptr);
    CHECK(composite.child_count() == 0);
}

static void test_two_free_slots_do_not_partially_install() {
    AllocatorFront front;
    PageAllocator placeholder(front.owners(), 2);
    for (std::uint32_t slot = 2; slot < AllocatorFront::kHeapSlots - 2; ++slot) {
        CHECK(front.set_heap(slot, &placeholder));
    }

    BucketAllocator bucket(front.owners(), kBootstrapBucketHeapSlot);
    HeapAllocator heap(front.owners(), kBootstrapHeapHeapSlot);
    PageAllocator page(front.owners(), kBootstrapPageHeapSlot);
    CompositeAllocator composite;
    CHECK(!bootstrap_allocator_hierarchy(front, composite, bucket, heap, page));
    CHECK(front.heap(kBootstrapCompositeHeapSlot) == nullptr);
    CHECK(front.heap(254) == nullptr);
    CHECK(front.heap(255) == nullptr);
    CHECK(composite.child_count() == 0);

    for (std::uint32_t slot = 2; slot < AllocatorFront::kHeapSlots; ++slot) {
        front.clear_heap(slot);
    }
    auto retry = bootstrap_allocator_hierarchy(front, composite, bucket, heap, page);
    CHECK(retry && !retry->already_installed);
    CHECK(retry->bucket_slot == kBootstrapBucketHeapSlot);
    CHECK(retry->heap_slot == kBootstrapHeapHeapSlot);
    CHECK(retry->page_slot == kBootstrapPageHeapSlot);
    CHECK(composite.child_count() == 3);
}

static void test_preoccupied_table_bootstrap_is_idempotent() {
    AllocatorFront front;
    PageAllocator placeholder(front.owners(), 2);
    CHECK(front.set_heap(2, &placeholder));
    BucketAllocator bucket(front.owners(), 3);
    HeapAllocator heap(front.owners(), 4);
    PageAllocator page(front.owners(), 5);
    CompositeAllocator composite;

    auto first = bootstrap_allocator_hierarchy(front, composite, bucket, heap, page);
    CHECK(first && !first->already_installed);
    CHECK(first->composite_slot == kBootstrapCompositeHeapSlot);
    CHECK(first->bucket_slot == 3);
    CHECK(first->heap_slot == 4);
    CHECK(first->page_slot == 5);
    CHECK(front.heap(3) == &bucket);
    CHECK(front.heap(4) == &heap);
    CHECK(front.heap(5) == &page);

    auto retry = bootstrap_allocator_hierarchy(front, composite, bucket, heap, page);
    CHECK(retry && retry->already_installed);
    CHECK(retry->bucket_slot == first->bucket_slot);
    CHECK(retry->heap_slot == first->heap_slot);
    CHECK(retry->page_slot == first->page_slot);
}

static void test_unrelated_or_incomplete_slot_one_is_not_installed() {
    {
        AllocatorFront front;
        PageAllocator placeholder(front.owners(), 2);
        CHECK(front.set_heap(kBootstrapCompositeHeapSlot, &placeholder));
        BucketAllocator bucket(front.owners(), kBootstrapBucketHeapSlot);
        HeapAllocator heap(front.owners(), kBootstrapHeapHeapSlot);
        PageAllocator page(front.owners(), kBootstrapPageHeapSlot);
        CompositeAllocator composite;
        CHECK(!bootstrap_allocator_hierarchy(front, composite, bucket, heap, page));
        CHECK(front.heap(kBootstrapCompositeHeapSlot) == &placeholder);
        CHECK(composite.child_count() == 0);
    }

    {
        AllocatorFront front;
        BucketAllocator bucket(front.owners(), kBootstrapBucketHeapSlot);
        HeapAllocator heap(front.owners(), kBootstrapHeapHeapSlot);
        PageAllocator page(front.owners(), kBootstrapPageHeapSlot);
        CompositeAllocator composite;
        CHECK(front.set_heap(kBootstrapCompositeHeapSlot, &composite));
        CHECK(!bootstrap_allocator_hierarchy(front, composite, bucket, heap, page));
        CHECK(front.heap(kBootstrapCompositeHeapSlot) == &composite);
        CHECK(composite.child_count() == 0);
    }
}

static void test_constants() {
    CHECK(kBootstrapCompositeHeapSlot == AllocatorFront::kDefaultHeap);
    CHECK(kBootstrapHeapSlotCount == 0x100u);
    CHECK(kHeapPageBytes == 0x100000u);
    CHECK(kHeapAllocatorCapBytes == 0x9600000u);
    CHECK(kBucketPageBytes == 0x10000u);
}

int main() {
    test_constants();
    test_first_free_slot_scan();
    test_empty_table_bootstrap_slots();
    test_idempotent_early_out();
    test_incomplete_bootstrap_does_not_mutate();
    test_two_free_slots_do_not_partially_install();
    test_preoccupied_table_bootstrap_is_idempotent();
    test_unrelated_or_incomplete_slot_one_is_not_installed();
    std::puts("OK allocator_bootstrap");
    return 0;
}
