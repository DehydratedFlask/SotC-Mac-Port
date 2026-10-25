// Measured Init remainder: trivial leaves → registry via tip AllocatorFront (+ FakeHeap).
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_remainder_slice.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/registry_init_531d60.hpp"

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
    std::uint64_t alloc_count = 0;
};

}  // namespace

int main() {
    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    InputTable551140 table{};
    init_input_table_551140(table);

    std::array<std::uint8_t, 0x60> obj{};
    std::memset(obj.data(), 0xCC, obj.size());

    // Fail-visible registry without names; trivial leaves still apply.
    {
        const auto miss =
            run_appmain_init_remainder_slice(table, front, obj.data(), 1, 0x11, 0x22, 0x33);
        CHECK(miss.trivial_ok);
        CHECK(miss.flag_2efa890 == 1);
        CHECK(miss.flag_2efa891 == 1);
        CHECK(miss.setter_at_01 == 0x11);
        CHECK(miss.setter_at_29 == 0x22);
        CHECK(miss.setter_at_51 == 0x33);
        CHECK(miss.registry.status == RegistryInit531d60Status::missing_names);
    }

    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        std::fprintf(stderr,
                     "appmain_init_remainder_slice: SKIP full install "
                     "(private I275 fixture missing)\n");
        std::puts("appmain_init_remainder_slice: trivial+missing-fixture OK");
        return 0;
    }
    CHECK(apply_inputtable_names_fixture(table, fix));

    std::memset(obj.data(), 0xCC, obj.size());
    const std::uint64_t allocs_before = heap.alloc_count;
    const auto t0 = std::chrono::steady_clock::now();
    const auto r =
        run_appmain_init_remainder_slice(table, front, obj.data(), 0, 0xAA, 0xBB, 0xCD);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    CHECK(r.trivial_ok);
    CHECK(r.flag_2efa890 == 0);
    CHECK(r.flag_2efa891 == 1);
    CHECK(r.setter_at_01 == 0xAA);
    CHECK(r.setter_at_29 == 0xBB);
    CHECK(r.setter_at_51 == 0xCD);
    // Poison neighbors untouched.
    CHECK(obj[0x00] == 0xCC);
    CHECK(obj[0x02] == 0xCC);
    CHECK(obj[0x28] == 0xCC);
    CHECK(obj[0x2a] == 0xCC);
    CHECK(obj[0x50] == 0xCC);
    CHECK(obj[0x52] == 0xCC);

    CHECK(r.registry.status == RegistryInit531d60Status::ok);
    CHECK(r.registry.cell_installed);
    CHECK(r.registry.entry_count == kInputTable551140Count);
    CHECK(r.registry.object_bytes == 0x98);
    CHECK(heap.alloc_count > allocs_before);

    std::printf(
        "appmain_init_remainder_slice: MEASURED trivial+registry ok "
        "flag=(%u,%u) setters=(%02x,%02x,%02x) entries=%u alloc_ops=%llu "
        "duration_us=%lld allocator=tip_AllocatorFront+FakeHeap "
        "pr5_heap_merge=GAP base_Initialize=NOT_modelled\n",
        r.flag_2efa890, r.flag_2efa891, r.setter_at_01, r.setter_at_29, r.setter_at_51,
        r.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    std::puts("appmain_init_remainder_slice: all checks passed");
    return 0;
}
