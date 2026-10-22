// Sorted unique-insert deepen for registry helpers 0x52d530 / 0x52d7e0 (+ grow 0x52d660 / 0x52d920).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
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

    // --- Table A: unique sorted insert, duplicate leaves val unchanged ---
    RegistryVecA a{};
    auto* e1 = registry_insert_unique_a(a, {10, 100}, front);
    CHECK(e1 && e1->key == 10 && e1->val == 100);
    CHECK(a.end - a.begin == 1);

    auto* e2 = registry_insert_unique_a(a, {5, 50}, front);
    CHECK(e2 && e2->key == 5 && a.begin[0].key == 5 && a.begin[1].key == 10);

    auto* dup = registry_insert_unique_a(a, {10, 999}, front);
    CHECK(dup == &a.begin[1]);
    CHECK(dup->val == 100);  // not overwritten
    CHECK(a.end - a.begin == 2);

    // Force grow (capacity 2 after two inserts from empty->1->2); third unique needs double.
    auto* e3 = registry_insert_unique_a(a, {7, 70}, front);
    CHECK(e3 && e3->key == 7);
    CHECK(a.end - a.begin == 3);
    CHECK(a.begin[0].key == 5 && a.begin[1].key == 7 && a.begin[2].key == 10);
    CHECK(a.cap - a.begin >= 4);  // doubled from 2

    registry_vec_a_clear(a, front);
    CHECK(a.begin == nullptr);

    // --- Table B: 16-B entries, ptr at +8, pad zeroed ---
    RegistryVecB b{};
    int obj_x = 1, obj_y = 2, obj_z = 3;
    auto* b1 = registry_insert_unique_b(b, {20, 0xdeadbeefu, &obj_x}, front);
    CHECK(b1 && b1->key == 20 && b1->pad == 0 && b1->ptr == &obj_x);

    auto* b2 = registry_insert_unique_b(b, {10, 1, &obj_y}, front);
    CHECK(b2 && b.begin[0].key == 10 && b.begin[1].key == 20);

    auto* bdup = registry_insert_unique_b(b, {20, 0, &obj_z}, front);
    CHECK(bdup == &b.begin[1]);
    CHECK(bdup->ptr == &obj_x);  // unchanged
    CHECK(b.end - b.begin == 2);

    auto* b3 = registry_insert_unique_b(b, {15, 0, &obj_z}, front);
    CHECK(b3 && b.end - b.begin == 3);
    CHECK(b.begin[0].key == 10 && b.begin[1].key == 15 && b.begin[2].key == 20);
    CHECK(b.begin[1].ptr == &obj_z);

    registry_vec_b_clear(b, front);

    std::puts("registry_sorted_insert: all checks passed");
    return 0;
}
