// HeapAllocator contract tests (OBS 0x48c70 / 0x48c80 / 0x488f0 / 0x48910).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

static void test_math_and_usable_size() {
    CHECK(heap_round_page_quantum(0x100000) == 0x100000u);  // OBS 0x488f0 already 64 KiB-aligned
    CHECK(heap_round_page_quantum(0x100001) == 0x110000u);  // (x+0xffff) & ~0xffff
    CHECK(heap_request_bytes(1, 1) == 0x10u);
    CHECK(heap_request_bytes(0x401, 8) == 0x410u);
    CHECK(heap_request_bytes(0x100000, 8) == 0x100000u);
    CHECK(!heap_request_bytes(0x100001, 8));
    CHECK(kHeapPageBytes == 0x100000u);
    CHECK(kHeapAllocatorCapBytes == 0x9600000u);
    CHECK(kHeapReserveAlign == 0x10000u);

    PageOwnerMap owners;
    HeapAllocator h(owners, 3);
    void* p = h.allocate(0x123, 8, AllocDesc{});
    CHECK(p);
    CHECK(heap_usable_size(p) == 0x130u);  // 0x123 → 16-byte round
    CHECK(reinterpret_cast<std::uintptr_t>(p) % 8 == 0);
    h.deallocate(p, AllocDesc{});
}

static void test_cap_reject_and_no_zero() {
    PageOwnerMap owners;
    HeapAllocator h(owners, 3, /*cap=*/0x1800, /*page=*/0x100000);
    AllocDesc d{};
    void* a = h.allocate(0x1000, 8, d);
    CHECK(a);
    std::memset(a, 0x3C, 0x1000);
    void* b = h.allocate(0x1000, 8, d);  // 0x1000+0x1000 > 0x1800
    CHECK(b == nullptr);
    CHECK(h.stats().cap_rejects == 1);
    CHECK(static_cast<unsigned char*>(a)[0] == 0x3C);
    h.deallocate(a, d);
    b = h.allocate(0x1000, 8, d);
    CHECK(b);
    h.deallocate(b, d);

    int stack = 0;
    h.deallocate(&stack, d);
    CHECK(h.stats().ignored_frees == 1);
}

static void test_composite_mid_band() {
    AllocatorFront front;
    BucketAllocator bucket(front.owners(), 2);
    HeapAllocator heap(front.owners(), 3);
    PageAllocator page(front.owners(), 4);
    CompositeAllocator composite;
    CHECK(install_default_hierarchy(composite, &bucket, &heap, &page));
    CHECK(front.set_heap(1, &composite));
    CHECK(front.set_heap(2, &bucket));
    CHECK(front.set_heap(3, &heap));
    CHECK(front.set_heap(4, &page));

    // size == 0x400 → heap (composite strict < 0x400 for bucket)
    void* mid = front.alloc(0x400, 1, 8, kAllocFlagNew);
    CHECK(mid && heap.stats().live_blocks == 1 && bucket.stats().live_blocks == 0);
    CHECK(heap_usable_size(mid) >= 0x400);
    front.free(mid, kAllocFlagNew);
    CHECK(heap.stats().live_blocks == 0);

    // size == 0x100000 → page
    void* big = front.alloc(0x100000, 1, 8, kAllocFlagNew);
    CHECK(big && page.stats().live_blocks == 1 && heap.stats().live_blocks == 0);
    front.free(big, kAllocFlagNew);
}

int main() {
    test_math_and_usable_size();
    test_cap_reject_and_no_zero();
    test_composite_mid_band();
    std::puts("OK heap_allocator");
    return 0;
}
