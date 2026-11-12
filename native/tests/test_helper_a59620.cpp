// CPU-01-AOT-CFG-SCALE-01-BATCH19: fixture for the id + string entry constructor 0xa59620 and the doubling pointer-vector push.
// ELF pins: tools/lift/check_helper_a59620.py. FIXTURE_VERIFIED.
#include <cstdlib>
#include <vector>

#include "core/helper_a59620_id_entry.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

struct Counter {
    int allocs = 0, frees = 0;
    std::size_t last_size = 0;
    std::uint32_t last_tag = 0, last_align = 0, last_flags = 0, last_free_tag = 0;
    int fail_alloc_after = -1;
    bool fail_free = false;
    std::vector<void*> live;
} g;

void* hook_alloc(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g.allocs;
    g.last_size = size;
    g.last_tag = tag;
    g.last_align = align;
    g.last_flags = flags;
    if (g.fail_alloc_after == g.allocs) return nullptr;
    void* p = std::calloc(1, size);
    g.live.push_back(p);
    return p;
}

bool hook_free(void* ptr, std::uint32_t tag) {
    ++g.frees;
    g.last_free_tag = tag;
    if (g.fail_free) return false;
    for (std::size_t i = 0; i < g.live.size(); ++i) {
        if (g.live[i] == ptr) {
            g.live.erase(g.live.begin() + static_cast<std::ptrdiff_t>(i));
            std::free(ptr);
            return true;
        }
    }
    return false;
}

AllocDispatchContract contract() {
    AllocDispatchContract c;
    c.allocate = &hook_alloc;
    c.deallocate = &hook_free;
    return c;
}

std::size_t cap(const PtrVec24& v) { return static_cast<std::size_t>(v.cap - v.begin); }
std::size_t len(const PtrVec24& v) { return static_cast<std::size_t>(v.end - v.begin); }

void release(PtrVec24& v) {
    if (v.begin != nullptr) {
        for (std::size_t i = 0; i < g.live.size(); ++i) {
            if (g.live[i] == v.begin) {
                std::free(v.begin);
                g.live.erase(g.live.begin() + static_cast<std::ptrdiff_t>(i));
                break;
            }
        }
    }
    v = PtrVec24{};
}

void test_entry_copy() {
    PtrVec24 vec{};
    IdEntry entry;
    poison(entry);
    std::int64_t ctl = 5;
    const SharedStr src{reinterpret_cast<const std::uint8_t*>("name"), 4, 7, &ctl};
    const IdEntryRegistration r = register_id_entry_a59620(vec, entry, 0x1d, src, contract());
    CHECK(r.ok && r.pushed && r.grew && r.new_capacity == 1 && r.release_inert && !r.aliased);
    CHECK(entry.id == 0x1d);
    CHECK(entry.gap04[0] == kPoison && entry.gap04[3] == kPoison);           // the four bytes at +4 are never written
    CHECK(entry.str.data == src.data && entry.str.size == 4 && entry.str.aux == 7 && entry.str.ctl == &ctl);
    CHECK(ctl == 6);                                                         // lock inc qword [ctl]
    CHECK(len(vec) == 1 && vec.begin[0] == &entry);
    CHECK(g.last_size == 8 && g.last_tag == 1 && g.last_align == 8 && g.last_flags == 2);   // 0x17090: tag 1, align 8, flags 2
    release(vec);
}

void test_null_ctl_not_incremented() {
    PtrVec24 vec{};
    IdEntry entry;
    poison(entry);
    const SharedStr src{reinterpret_cast<const std::uint8_t*>("x"), 1, 0, nullptr};
    CHECK(register_id_entry_a59620(vec, entry, 0, src, contract()).ok);
    CHECK(entry.str.ctl == nullptr && entry.id == 0);
    release(vec);
}

void test_alias_skips_copy() {
    PtrVec24 vec{};
    IdEntry entry;
    poison(entry);
    const IdEntryRegistration r = register_id_entry_a59620(vec, entry, 9, entry.str, contract());   // src == &entry.str
    CHECK(r.ok && r.aliased);
    CHECK(entry.id == 9 && entry.str.size == 0 && entry.str.ctl == nullptr && entry.str.aux == 0);   // f180 emptied it, nothing copied
    release(vec);
}

void test_growth_doubling() {
    PtrVec24 vec{};
    IdEntry entries[40];
    const SharedStr src{reinterpret_cast<const std::uint8_t*>("n"), 1, 0, nullptr};
    const std::size_t want[] = {1, 2, 4, 4, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16, 16, 32};
    int grows = 0;
    for (std::size_t i = 0; i < 17; ++i) {
        const IdEntryRegistration r = register_id_entry_a59620(vec, entries[i], static_cast<std::uint32_t>(i), src, contract());
        CHECK(r.ok && r.pushed);
        CHECK(cap(vec) == want[i]);
        CHECK(len(vec) == i + 1);
        if (r.grew) ++grows;
        for (std::size_t k = 0; k <= i; ++k) CHECK(vec.begin[k] == &entries[k]);        // the memmove preserved the order
    }
    CHECK(grows == 6);                       // capacities 1, 2, 4, 8, 16, 32
    CHECK(g.frees == 5);                     // every old block but the first (null begin) went through 0x170c0
    CHECK(g.last_free_tag == 2);
    release(vec);
}

void test_refusals() {
    IdEntry entry;
    const SharedStr src{reinterpret_cast<const std::uint8_t*>("n"), 1, 0, nullptr};
    {
        PtrVec24 vec{};
        const AllocDispatchContract none{};
        const IdEntryRegistration r = register_id_entry_a59620(vec, entry, 3, src, none);
        CHECK(!r.ok && !r.pushed && r.refusal == PtrVecRefusal::no_allocator);
        CHECK(vec.begin == nullptr && vec.end == nullptr && vec.cap == nullptr);
        CHECK(entry.id == 3 && entry.str.data == src.data);         // the entry is written before the push is attempted
    }
    {
        PtrVec24 vec{};
        g.fail_alloc_after = g.allocs + 1;
        const IdEntryRegistration r = register_id_entry_a59620(vec, entry, 3, src, contract());
        CHECK(!r.ok && r.refusal == PtrVecRefusal::allocation_failed && vec.begin == nullptr);
        g.fail_alloc_after = -1;
    }
    {
        PtrVec24 vec{};
        AllocDispatchContract alloc_only;
        alloc_only.allocate = &hook_alloc;                           // growth with an old block needs the free hook
        IdEntry e2;
        CHECK(register_id_entry_a59620(vec, entry, 1, src, alloc_only).ok);   // first growth: begin is null, no free needed
        void** const old_begin = vec.begin;
        const IdEntryRegistration r = register_id_entry_a59620(vec, e2, 2, src, alloc_only);
        CHECK(!r.ok && r.refusal == PtrVecRefusal::no_allocator && vec.begin == old_begin && len(vec) == 1);
        release(vec);
    }
    {
        PtrVec24 vec{};
        IdEntry e2;
        CHECK(register_id_entry_a59620(vec, entry, 1, src, contract()).ok);
        void** const leaked = vec.begin;
        g.fail_free = true;
        const IdEntryRegistration r = register_id_entry_a59620(vec, e2, 2, src, contract());
        g.fail_free = false;
        CHECK(!r.ok && r.pushed && r.refusal == PtrVecRefusal::free_failed && len(vec) == 2 && cap(vec) == 2);   // updated, old block leaked
        CHECK(hook_free(leaked, 2));     // the test reclaims the leaked block
        release(vec);
    }
}

void test_room_means_no_allocation() {
    PtrVec24 vec{};
    void* storage[4] = {};
    vec.begin = storage;
    vec.end = storage;
    vec.cap = storage + 4;
    IdEntry entry;
    const SharedStr src{reinterpret_cast<const std::uint8_t*>("n"), 1, 0, nullptr};
    const int before = g.allocs;
    const AllocDispatchContract none{};
    const IdEntryRegistration r = register_id_entry_a59620(vec, entry, 1, src, none);   // no hooks needed when there is room
    CHECK(r.ok && r.pushed && !r.grew && g.allocs == before && storage[0] == &entry && len(vec) == 1);
}

void test_empty_with_capacity_zero() {
    // begin == end == cap != null: bytes == 0 so the growth is the minimum (one element) and the old block is freed.
    PtrVec24 vec{};
    void* stub = std::calloc(1, 8);
    g.live.push_back(stub);
    vec.begin = vec.end = vec.cap = static_cast<void**>(stub);
    IdEntry entry;
    const SharedStr src{reinterpret_cast<const std::uint8_t*>("n"), 1, 0, nullptr};
    const IdEntryRegistration r = register_id_entry_a59620(vec, entry, 1, src, contract());
    CHECK(r.ok && r.grew && r.new_capacity == 1 && vec.begin != stub && len(vec) == 1);
    release(vec);
}

}  // namespace

int main() {
    test_entry_copy();
    test_null_ctl_not_incremented();
    test_alias_skips_copy();
    test_growth_doubling();
    test_refusals();
    test_room_means_no_allocation();
    test_empty_with_capacity_zero();
    CHECK(g.live.empty());
    std::puts("test_helper_a59620 OK");
    return 0;
}
