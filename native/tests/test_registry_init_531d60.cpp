// Init 0x531d60 registry fragment: alloc 0x98 → fill 0x52a220 → store cell 0x2efd160.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/registry_fill_52a220.hpp"
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
    CHECK(kRegistryInitObjectBytes == 0x98);
    CHECK(kRegistryObject2efd160Bytes == 0x98);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    // Clean slate for the process cell.
    destroy_registry_at_2efd160(front);
    CHECK(registry_cell_2efd160().load() == nullptr);

    InputTable551140 table{};
    init_input_table_551140(table);

    // Fail-visible without names.
    CHECK(init_registry_fragment_531d60(table, front) ==
          RegistryInit531d60Status::missing_names);
    CHECK(registry_cell_2efd160().load() == nullptr);

    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        std::fprintf(stderr,
                     "registry_init_531d60: SKIP full install (private I275 fixture missing)\n");
        std::puts("registry_init_531d60: missing-fixture fail-visible OK");
        return 0;
    }
    CHECK(apply_inputtable_names_fixture(table, fix));

    CHECK(init_registry_fragment_531d60(table, front) == RegistryInit531d60Status::ok);
    RegistryObject2efd160* first = registry_cell_2efd160().load();
    CHECK(first != nullptr);
    CHECK(first->input_table == table.data());
    CHECK(first->float_5 == kRegistryObjectFloat5);
    CHECK(first->float_60 == kRegistryObjectFloat60);
    auto* a_begin = static_cast<RegistryEntryA*>(first->table_a_begin);
    auto* a_end = static_cast<RegistryEntryA*>(first->table_a_end);
    CHECK(a_begin && static_cast<std::size_t>(a_end - a_begin) == kInputTable551140Count);

    // Re-init replaces previous object (destroy + store).
    CHECK(init_registry_fragment_531d60(table, front) == RegistryInit531d60Status::ok);
    RegistryObject2efd160* second = registry_cell_2efd160().load();
    CHECK(second != nullptr);
    CHECK(second != first);

    destroy_registry_at_2efd160(front);
    CHECK(registry_cell_2efd160().load() == nullptr);

    std::puts("registry_init_531d60: all checks passed");
    return 0;
}
