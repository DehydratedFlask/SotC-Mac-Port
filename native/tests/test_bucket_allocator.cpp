// BucketAllocator contract tests (OBS 0x17990 / 0x18750 / 0x179c0 size-class band).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/bucket_allocator.hpp"
#include "core/composite_allocator.hpp"
#include "core/page_allocator.hpp"

#define CHECK(c)                                                              \
    do {                                                                      \
        if (!(c)) {                                                           \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);  \
            std::exit(1);                                                     \
        }                                                                     \
    } while (0)

using namespace sotc;

static void test_class_math() {
    CHECK(bucket_class_bytes(1, 1) == 0x10u);
    CHECK(bucket_class_bytes(0x10, 8) == 0x10u);
    CHECK(bucket_class_bytes(0x11, 8) == 0x20u);
    CHECK(bucket_class_bytes(0x3ff, 8) == 0x400u);
    CHECK(bucket_class_bytes(0x400, 8) == 0x400u);
    CHECK(!bucket_class_bytes(0x401, 8));
    CHECK(bucket_class_bytes(0x10, 0x40) == 0x40u);  // align bumps class
    CHECK(!bucket_class_bytes(0x10, 0x800));           // align beyond band
    CHECK(!bucket_class_bytes(std::numeric_limits<std::size_t>::max(), 8));  // size rounding overflow
    CHECK(bucket_class_index(0x10) == 0);
    CHECK(bucket_class_index(0x400) == 63);
    CHECK(kBucketClassCount == 64);
    CHECK(bucket_slots_per_page(0x10) == (0x10000u - 0x38u) / 0x10u);
    CHECK(bucket_slots_per_page(0x400) == (0x10000u - 0x38u) / 0x400u);
}

static void test_alloc_free_reuse() {
    PageOwnerMap owners;
    BucketAllocator b(owners, 2);
    AllocDesc d{kAllocFlagNew, 0};

    void* p = b.allocate(0x18, 8, d);
    CHECK(p != nullptr);
    CHECK(reinterpret_cast<std::uintptr_t>(p) % 8 == 0);
    CHECK(owners.lookup(p) == 2u);
    CHECK(b.stats().live_blocks == 1);
    CHECK(b.stats().pages_mapped == 1);
    // No zero-fill guarantee: write a pattern and ensure allocate does not clear on reuse path.
    std::memset(p, 0x5A, 0x20);

    b.deallocate(p, d);
    CHECK(b.stats().live_blocks == 0);
    CHECK(!owners.lookup(p));

    void* q = b.allocate(0x18, 8, d);
    CHECK(q == p);  // INF freelist LIFO reuse
    CHECK(static_cast<unsigned char*>(q)[0] == 0x5A);  // OBS: no zero-fill on reuse
    b.deallocate(q, d);
}

static void test_requested_alignments() {
    PageOwnerMap owners;
    BucketAllocator b(owners, 2);
    AllocDesc d{};

    for (const auto& request : {std::pair<std::size_t, std::size_t>{8, 16}, {0x21, 32}}) {
        const auto [size, align] = request;
        std::vector<void*> blocks;
        for (int i = 0; i < 3; ++i) {
            void* p = b.allocate(size, align, d);
            CHECK(p != nullptr);
            CHECK(reinterpret_cast<std::uintptr_t>(p) % align == 0);
            blocks.push_back(p);
        }
        for (void* p : blocks) b.deallocate(p, d);
    }
}

static void test_reject_oversize_and_foreign_free() {
    PageOwnerMap owners;
    BucketAllocator b(owners, 3);
    CHECK(b.allocate(0x401, 8, AllocDesc{}) == nullptr);
    CHECK(b.stats().live_blocks == 0);

    int stack = 0;
    b.deallocate(&stack, AllocDesc{});
    CHECK(b.stats().ignored_frees == 1);
    b.deallocate(nullptr, AllocDesc{});
}

static void test_many_classes_and_composite() {
    AllocatorFront front;
    BucketAllocator bucket(front.owners(), 2);
    PageAllocator page(front.owners(), 4);
    // Mid band still needs a stand-in until HeapAllocator lands.
    struct PassHeap : IAllocator {
        PageOwnerMap& owners;
        std::uint32_t id;
        explicit PassHeap(PageOwnerMap& o, std::uint32_t i) : owners(o), id(i) {}
        void* allocate(std::size_t size, std::size_t align, const AllocDesc&) override {
            void* p = nullptr;
            const std::size_t a = align < sizeof(void*) ? sizeof(void*) : align;
            if (posix_memalign(&p, a, size) != 0) return nullptr;
            CHECK(owners.add(reinterpret_cast<std::uintptr_t>(p), size, id) ==
                  PageOwnerMap::Status::ok);
            return p;
        }
        void deallocate(void* p, const AllocDesc&) override {
            owners.remove(reinterpret_cast<std::uintptr_t>(p));
            std::free(p);
        }
    } heap(front.owners(), 3);

    CompositeAllocator composite;
    CHECK(install_default_hierarchy(composite, &bucket, &heap, &page));
    CHECK(front.set_heap(1, &composite));
    CHECK(front.set_heap(2, &bucket));
    CHECK(front.set_heap(3, &heap));
    CHECK(front.set_heap(4, &page));

    auto* small = static_cast<unsigned char*>(front.new_(0x20));
    CHECK(small && bucket.stats().live_blocks == 1);
    small[0] = 0xA5;
    front.delete_(small);
    CHECK(bucket.stats().live_blocks == 0);

    // Exactly at composite bucket edge goes to heap stand-in, not bucket.
    void* edge = front.alloc(0x400, 1, 8, kAllocFlagNew);
    CHECK(edge && bucket.stats().live_blocks == 0);
    front.free(edge, kAllocFlagNew);
}

int main() {
    test_class_math();
    test_alloc_free_reuse();
    test_requested_alignments();
    test_reject_oversize_and_foreign_free();
    test_many_classes_and_composite();
    std::puts("OK bucket_allocator");
    return 0;
}
