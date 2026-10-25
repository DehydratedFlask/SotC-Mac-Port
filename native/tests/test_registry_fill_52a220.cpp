// Fill-loop deepen for registry ctor 0x52a220 (hash names → insert A/B).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/registry_fill_52a220.hpp"
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
};

}  // namespace

int main() {
    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    InputTable551140 table{};
    init_input_table_551140(table);

    // Fail-visible: names still null → missing_names.
    {
        RegistryObject2efd160 obj;
        std::memset(&obj, 0xa5, sizeof(obj));
        CHECK(fill_registry_object_52a220(obj, table, front, 1) ==
              RegistryFill52a220Status::missing_names);
    }

    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        std::fprintf(stderr,
                     "registry_fill_52a220: SKIP full fill (private name fixture missing)\n");
        std::puts("registry_fill_52a220: missing-fixture fail-visible OK");
        return 0;
    }
    CHECK(apply_inputtable_names_fixture(table, fix));

    RegistryObject2efd160 obj;
    std::memset(&obj, 0xa5, sizeof(obj));
    CHECK(fill_registry_object_52a220(obj, table, front) == RegistryFill52a220Status::ok);

    CHECK(obj.float_5 == kRegistryObjectFloat5);
    CHECK(obj.float_60 == kRegistryObjectFloat60);
    CHECK(obj.input_table == table.data());
    CHECK(obj.listeners_begin == nullptr);

    auto* a_begin = static_cast<RegistryEntryA*>(obj.table_a_begin);
    auto* a_end = static_cast<RegistryEntryA*>(obj.table_a_end);
    auto* a_cap = static_cast<RegistryEntryA*>(obj.table_a_cap);
    auto* b_begin = static_cast<RegistryEntryB*>(obj.table_b_begin);
    auto* b_end = static_cast<RegistryEntryB*>(obj.table_b_end);
    CHECK(a_begin && a_end && a_cap);
    CHECK(static_cast<std::size_t>(a_end - a_begin) == kInputTable551140Count);
    CHECK(static_cast<std::size_t>(a_cap - a_begin) >= kInputTable551140Count);
    CHECK(static_cast<std::size_t>(b_end - b_begin) == kInputTable551140Count);

    // Sorted ascending by key; vals zero; every entry hash_a matches fixture.
    for (std::size_t i = 0; i != kInputTable551140Count; ++i) {
        CHECK(table[i].hash_a == fix.pairs[i].hash_a);
        CHECK(table[i].hash_b == fix.pairs[i].hash_b);
        if (i + 1 < kInputTable551140Count) {
            CHECK(a_begin[i].key < a_begin[i + 1].key);
            CHECK(b_begin[i].key < b_begin[i + 1].key);
        }
        CHECK(a_begin[i].val == 0);
        CHECK(b_begin[i].pad == 0);
        CHECK(b_begin[i].ptr != nullptr);
    }

    // Spot-check: table B ptr for key==table[0].hash_a points at &table[0].
    bool found0 = false;
    for (std::size_t i = 0; i != kInputTable551140Count; ++i) {
        if (b_begin[i].key == table[0].hash_a) {
            CHECK(b_begin[i].ptr == &table[0]);
            found0 = true;
            break;
        }
    }
    CHECK(found0);

    clear_registry_object_tables_52a220(obj, front);
    CHECK(obj.table_a_begin == nullptr && obj.table_b_begin == nullptr);

    std::puts("registry_fill_52a220: all checks passed");
    return 0;
}
