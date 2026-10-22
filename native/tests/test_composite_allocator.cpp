// CompositeAllocator contract tests (OBS 0x47e30 dispatch + bootstrap thresholds).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
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

struct CountingHeap : IAllocator {
    // When `owners` is set, blocks are registered like FakeHeap so AllocatorFront::free works.
    CountingHeap(char tag, PageOwnerMap* owners = nullptr, std::uint32_t id = 0)
        : tag(tag), owners(owners), id(id) {}
    ~CountingHeap() override {
        for (void* p : live) {
            if (owners) owners->remove(reinterpret_cast<std::uintptr_t>(p));
            std::free(p);
        }
    }
    void* allocate(std::size_t size, std::size_t align, const AllocDesc& d) override {
        ++alloc_calls;
        last_size = size;
        last_align = align;
        last_desc = d;
        if (fail) return nullptr;
        void* p = nullptr;
        const std::size_t a = align < sizeof(void*) ? sizeof(void*) : align;
        if (posix_memalign(&p, a, size) != 0) return nullptr;
        std::memset(p, static_cast<int>(tag), size);
        if (owners) {
            CHECK(owners->add(reinterpret_cast<std::uintptr_t>(p), size, id) ==
                  PageOwnerMap::Status::ok);
        }
        live.push_back(p);
        return p;
    }
    void deallocate(void* p, const AllocDesc& d) override {
        ++free_calls;
        last_free_desc = d;
        for (std::size_t i = 0; i < live.size(); ++i) {
            if (live[i] == p) {
                if (owners) owners->remove(reinterpret_cast<std::uintptr_t>(p));
                std::free(p);
                live.erase(live.begin() + static_cast<std::ptrdiff_t>(i));
                return;
            }
        }
        CHECK(false);
    }

    char tag;
    PageOwnerMap* owners = nullptr;
    std::uint32_t id = 0;
    bool fail = false;
    int alloc_calls = 0, free_calls = 0;
    std::size_t last_size = 0, last_align = 0;
    AllocDesc last_desc, last_free_desc;
    std::vector<void*> live;
};

static void test_threshold_dispatch() {
    CompositeAllocator c;
    CountingHeap bucket('B'), heap('H'), page('P');
    CHECK(install_default_hierarchy(c, &bucket, &heap, &page));
    CHECK(c.child_count() == 3);
    CHECK(c.children()[0].threshold == kCompositeBucketThreshold);
    CHECK(c.children()[1].threshold == kCompositeHeapThreshold);
    CHECK(c.children()[2].threshold == kCompositePageThreshold);

    AllocDesc desc{kAllocFlagNew, 0x11};

    // size < 0x400 -> bucket
    auto* p0 = static_cast<unsigned char*>(c.allocate(0x3ff, 8, desc));
    CHECK(p0 && p0[0] == 'B' && bucket.alloc_calls == 1 && heap.alloc_calls == 0);
    CHECK(c.bytes_dispatched() == 0x3ff);

    // size == 0x400 -> heap (strict <)
    auto* p1 = static_cast<unsigned char*>(c.allocate(0x400, 16, desc));
    CHECK(p1 && p1[0] == 'H' && heap.alloc_calls == 1 && page.alloc_calls == 0);
    CHECK(heap.last_align == 16 && heap.last_desc.flags == kAllocFlagNew);

    // size == 0x100000 -> page
    auto* p2 = static_cast<unsigned char*>(c.allocate(0x100000, 8, desc));
    CHECK(p2 && p2[0] == 'P' && page.alloc_calls == 1);

    // mid heap band
    auto* p3 = static_cast<unsigned char*>(c.allocate(0x80000, 8, desc));
    CHECK(p3 && p3[0] == 'H' && heap.alloc_calls == 2);

    c.deallocate(p0, AllocDesc{kAllocFlagNew, 0});
    c.deallocate(p1, AllocDesc{kAllocFlagNew, 0});
    c.deallocate(p2, AllocDesc{kAllocFlagNew, 0});
    c.deallocate(p3, AllocDesc{kAllocFlagNew, 0});
    CHECK(bucket.free_calls == 1 && heap.free_calls == 2 && page.free_calls == 1);
}

static void test_fallback_next_child() {
    CompositeAllocator c;
    CountingHeap bucket('B'), heap('H'), page('P');
    CHECK(install_default_hierarchy(c, &bucket, &heap, &page));
    bucket.fail = true;
    // size in bucket band, but bucket fails -> try heap (size 0x100 < 0x100000)
    auto* p = static_cast<unsigned char*>(c.allocate(0x100, 8, AllocDesc{}));
    CHECK(p && p[0] == 'H');
    CHECK(bucket.alloc_calls == 1 && heap.alloc_calls == 1);
    c.deallocate(p, AllocDesc{});
}

static void test_no_zero_fill_and_front() {
    AllocatorFront front;
    CompositeAllocator composite;
    // OBS free path: owner id from page table -> heap table slot (not composite walk).
    CountingHeap bucket('B', &front.owners(), 2);
    CountingHeap heap('H', &front.owners(), 3);
    PageAllocator page(front.owners(), 4);
    CHECK(install_default_hierarchy(composite, &bucket, &heap, &page));
    CHECK(front.set_heap(1, &composite));
    CHECK(front.set_heap(2, &bucket));
    CHECK(front.set_heap(3, &heap));
    CHECK(front.set_heap(4, &page));

    // Small alloc through front -> composite -> bucket; memory keeps fill tag (no zero).
    auto* p = static_cast<unsigned char*>(front.new_(0x20));
    CHECK(p && p[0] == 'B' && bucket.alloc_calls == 1);
    front.delete_(p);
    CHECK(bucket.free_calls == 1);

    // Large -> page allocator (registers owner 4); free via front owner map.
    void* big = front.alloc(0x200000, 1, 8, kAllocFlagNew);
    CHECK(big != nullptr && page.stats().live_blocks == 1);
    front.free(big, kAllocFlagNew);
    CHECK(page.stats().live_blocks == 0);
}

static void test_add_child_guards() {
    CompositeAllocator c;
    CountingHeap h('X');
    CHECK(!c.add_child(1, nullptr));
    for (std::size_t i = 0; i < CompositeAllocator::kMaxChildren; ++i)
        CHECK(c.add_child(i + 1, &h));
    CHECK(!c.add_child(99, &h));
}

int main() {
    test_threshold_dispatch();
    test_fallback_next_child();
    test_no_zero_fill_and_front();
    test_add_child_guards();
    std::puts("OK composite_allocator");
    return 0;
}
