// Measured app-main Init→registry slice via tip AllocatorFront (+ FakeHeap).
// Documents PR #5 heap merge gap; does not claim G1 PASSED.
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_registry_slice.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/registry_init_531d60.hpp"
#include "core/registry_sorted_insert.hpp"

#define CHECK(c)                                                                         \
    do {                                                                                 \
        if (!(c)) {                                                                      \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);              \
            std::exit(1);                                                                \
        }                                                                                \
    } while (0)

using namespace sotc;

namespace {

struct FakeHeap : IAllocator {
    FakeHeap(PageOwnerMap& o, std::uint32_t id_in) : owners(o), id(id_in) {}
    ~FakeHeap() override {
        for (void* p : live) {
            owners.remove(reinterpret_cast<std::uintptr_t>(p));
            std::free(p);
        }
    }
    void* allocate(std::size_t size, std::size_t align, const AllocDesc&) override {
        void* p = nullptr;
        if (posix_memalign(&p, align < sizeof(void*) ? sizeof(void*) : align, size) != 0) {
            return nullptr;
        }
        std::memset(p, 0xA5, size);
        CHECK(owners.add(reinterpret_cast<std::uintptr_t>(p), size, id) == PageOwnerMap::Status::ok);
        live.push_back(p);
        bytes_allocated += size;
        ++alloc_count;
        return p;
    }
    void deallocate(void* p, const AllocDesc&) override {
        for (std::size_t i = 0; i < live.size(); ++i) {
            if (live[i] == p) {
                owners.remove(reinterpret_cast<std::uintptr_t>(p));
                std::free(p);
                live.erase(live.begin() + static_cast<std::ptrdiff_t>(i));
                return;
            }
        }
        CHECK(false);
    }
    bool recover_from_failure() override { return false; }

    PageOwnerMap& owners;
    std::uint32_t id;
    std::vector<void*> live;
    std::uint64_t bytes_allocated = 0;
    std::uint64_t alloc_count = 0;
};

}  // namespace

int main() {
    CHECK(kAppmainVtableInitSlot == 0x15c8cf8);
    CHECK(kAppmainInitRva == 0x531d60);
    CHECK(kAppmainRunRva == 0x25a8d0);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    InputTable551140 table{};
    init_input_table_551140(table);

    // Fail-visible without names (honest edge).
    {
        const auto miss = run_appmain_init_registry_slice(table, front);
        CHECK(miss.status == RegistryInit531d60Status::missing_names);
        CHECK(!miss.cell_installed);
        CHECK(miss.entry_count == 0);
        CHECK(registry_cell_2efd160().load() == nullptr);
    }

    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        std::fprintf(stderr,
                     "appmain_init_registry_slice: SKIP measured install "
                     "(private I275 fixture missing)\n");
        std::puts("appmain_init_registry_slice: missing-fixture fail-visible OK");
        return 0;
    }
    CHECK(apply_inputtable_names_fixture(table, fix));

    const std::uint64_t allocs_before = heap.alloc_count;
    const std::uint64_t bytes_before = heap.bytes_allocated;
    const auto t0 = std::chrono::steady_clock::now();
    const AppmainInitRegistrySliceResult r = run_appmain_init_registry_slice(table, front);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    CHECK(r.status == RegistryInit531d60Status::ok);
    CHECK(r.cell_installed);
    CHECK(r.cell != nullptr);
    CHECK(r.object_bytes == 0x98);
    CHECK(r.entry_count == kInputTable551140Count);
    CHECK(r.cell->input_table == table.data());
    CHECK(r.cell->float_5 == kRegistryObjectFloat5);
    CHECK(r.cell->float_60 == kRegistryObjectFloat60);

    // Allocator depth on THIS tip: tip AllocatorFront → FakeHeap (0x17070 contract).
    // PR #5 Composite/Bucket/Heap/bootstrap/latch (E399–E403) NOT present — merge gap.
    const std::uint64_t alloc_delta = heap.alloc_count - allocs_before;
    const std::uint64_t bytes_delta = heap.bytes_allocated - bytes_before;
    CHECK(alloc_delta >= 3);  // object 0x98 + table A + table B (exact reserve)
    CHECK(bytes_delta >= 0x98);
    CHECK(!heap.live.empty());

    std::printf(
        "appmain_init_registry_slice: MEASURED ok entries=%u object_bytes=%u "
        "alloc_ops=%llu bytes=%llu live=%zu duration_us=%lld "
        "allocator=tip_AllocatorFront+FakeHeap "
        "pr5_heap_merge=GAP (cite E399-E403 on 4d7e484/0510d35; not in Drive tip)\n",
        r.entry_count, r.object_bytes, static_cast<unsigned long long>(alloc_delta),
        static_cast<unsigned long long>(bytes_delta), heap.live.size(),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    CHECK(registry_cell_2efd160().load() == nullptr);

    std::puts("appmain_init_registry_slice: all checks passed");
    return 0;
}
