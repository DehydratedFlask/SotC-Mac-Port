// CPU-01-HELPER-CALLEE-MODELS-03: fixture for the named-record registrar 0xa81700 and the vector<T*> reserve 0xa82050.
// ELF pins: tools/lift/check_helper_a81700.py. FIXTURE_VERIFIED.
#include <cstdlib>
#include <vector>

#include "core/helper_a81700_named_record.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

struct Counter {
    int allocs = 0;
    int frees = 0;
    std::size_t last_size = 0;
    std::uint32_t last_tag = 0, last_align = 0, last_flags = 0, last_free_tag = 0;
    int fail_alloc_after = -1;     // allocation number (1-based) that returns null, -1 = never
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

std::size_t cap(const RecordPtrVec& v) { return static_cast<std::size_t>(v.cap - v.begin); }
std::size_t len(const RecordPtrVec& v) { return static_cast<std::size_t>(v.end - v.begin); }

void release() {
    for (void* p : g.live) std::free(p);
    g.live.clear();
}

}  // namespace

int main() {
    CHECK(kNamedRecordCtorRva == 0xa81700 && kNamedRecordCtorSize == 190);
    CHECK(kRecordRegistryPushRva == 0xa81840 && kRecordRegistryPushSize == 265);
    CHECK(kVecReserveRva == 0xa82050 && kVecReserveSize == 245);
    CHECK(kRecordRegistryCtorRva == 0xa81b50 && kRecordRegistryCtorSize == 34);
    CHECK(kRecordRegistryDtorRva == 0xa81b80 && kRecordRegistryObjectRva == 0x32fd398 && kRecordRegistryGuardRva == 0x33053f0);
    CHECK(kNamedRecordStride == 0x38 && sizeof(NamedRecord) == 0x38);

    // --- registry constructor: only the stores the original performs -------------------------------------------------
    {
        RecordRegistry32fd398 r;
        poison(r);
        construct_record_registry_a81b50(r);
        const auto* b = reinterpret_cast<const std::uint8_t*>(&r);
        for (std::size_t i = 0; i < sizeof r; ++i) {
            const bool zeroed = (i >= 0x08 && i < 0x20) || (i >= 0x28 && i < 0x48) || (i >= 0x50 && i < 0x54);
            CHECK(b[i] == (zeroed ? 0 : kPoison));
        }
    }

    // --- first registration: initialises the registry, records the atexit as pending, field layout exact -------------------
    {
        RecordRegistryCell cell{};          // BSS state: zero, guard clear
        NamedRecord rec;
        poison(rec);
        std::int64_t ctl = 5;
        SharedStr name = view("RecordA");
        name.ctl = &ctl;
        name.aux = 0x99;
        g = Counter{};
        const NamedRecordRegistration reg = register_named_record_a81700(cell, rec, name, 0x1d, 1, contract());
        CHECK(reg.ok && reg.refusal == RecordRegistryRefusal::none);
        CHECK(reg.registry_initialised_now && reg.pushed_first && reg.pushed_second && reg.grows == 2);
        CHECK(cell.guard == 1);
        CHECK(rec.name.data == name.data && rec.name.size == name.size && rec.name.aux == 0x99 && rec.name.ctl == &ctl);
        CHECK(ctl == 6);                     // lock inc qword [ctl]
        CHECK(rec.kind == 0x1d && rec.q28 == 0 && rec.b30 == 0 && rec.flag == 1);
        const auto* b = reinterpret_cast<const std::uint8_t*>(&rec);
        for (int i = 0x24; i < 0x28; ++i) CHECK(b[i] == kPoison);       // never written
        for (int i = 0x32; i < 0x38; ++i) CHECK(b[i] == kPoison);
        CHECK(len(cell.registry.first) == 1 && cell.registry.first.begin[0] == &rec);
        CHECK(len(cell.registry.second) == 1 && cell.registry.second.begin[0] == &rec);
        CHECK(cap(cell.registry.first) == 1 && cap(cell.registry.second) == 1);
        CHECK(g.allocs == 2 && g.frees == 0 && g.last_size == 8);
        CHECK(g.last_tag == 1 && g.last_align == 8 && g.last_flags == 1);   // 0x17070 constants
        // the bytes the registry constructor never writes stay untouched (cell started as zero, so check zero)
        CHECK(cell.registry.q00 == 0 && cell.registry.q20 == 0 && cell.registry.d50 == 0 && cell.registry.q40 == 0);
        release();
    }

    // --- null control block: no increment, no fault ----------------------------------------------------------------------
    {
        RecordRegistryCell cell{};
        NamedRecord rec;
        poison(rec);
        g = Counter{};
        const NamedRecordRegistration reg = register_named_record_a81700(cell, rec, view("x"), 7, 0, contract());
        CHECK(reg.ok && rec.name.ctl == nullptr && rec.flag == 0 && rec.kind == 7);
        release();
    }

    // --- capacity growth 1,2,3,4,6,9,13,19 and the record order --------------------------------------------------------------
    {
        RecordRegistryCell cell{};
        std::vector<NamedRecord> recs(40);
        g = Counter{};
        const std::size_t want[] = {1, 2, 3, 4, 6, 9, 13, 19, 28, 42};
        std::size_t seen = 0, expected_grows = 0;
        std::size_t last_cap = 0;
        for (std::size_t i = 0; i < recs.size(); ++i) {
            const NamedRecordRegistration reg = register_named_record_a81700(cell, recs[i], view("n"), static_cast<std::uint32_t>(i), 1, contract());
            CHECK(reg.ok);
            CHECK(reg.registry_initialised_now == (i == 0));
            CHECK(cell.guard == 1);
            if (cap(cell.registry.first) != last_cap) {
                CHECK(seen < sizeof want / sizeof want[0]);
                CHECK(cap(cell.registry.first) == want[seen]);
                ++seen;
                ++expected_grows;
                last_cap = cap(cell.registry.first);
                CHECK(reg.grows == 2);
            } else {
                CHECK(reg.grows == 0);
            }
            CHECK(cap(cell.registry.first) == cap(cell.registry.second));
            CHECK(len(cell.registry.first) == i + 1 && len(cell.registry.second) == i + 1);
        }
        for (std::size_t i = 0; i < recs.size(); ++i) {
            CHECK(cell.registry.first.begin[i] == &recs[i] && cell.registry.second.begin[i] == &recs[i]);
        }
        CHECK(seen == 10 && g.allocs == static_cast<int>(2 * expected_grows));
        CHECK(g.frees == static_cast<int>(2 * (expected_grows - 1)));     // every grow after the first frees the old block
        CHECK(g.last_free_tag == 1);                                       // 0x170b0 passes tag 1
        release();
    }

    // --- reserve directly: no-op when room, exact new size, memmove preserved ----------------------------------------------
    {
        g = Counter{};
        RecordPtrVec v{};
        CHECK(vec_reserve_a82050(v, 0, contract()).ok && !vec_reserve_a82050(v, 0, contract()).grew);
        const VecReserveResult r = vec_reserve_a82050(v, 1, contract());
        CHECK(r.ok && r.grew && r.new_capacity == 1 && cap(v) == 1 && len(v) == 0);
        v.end = v.begin + 1;
        v.begin[0] = reinterpret_cast<NamedRecord*>(0x1111);
        const VecReserveResult r2 = vec_reserve_a82050(v, 1, contract());
        CHECK(r2.ok && r2.new_capacity == 2 && len(v) == 1 && v.begin[0] == reinterpret_cast<NamedRecord*>(0x1111));
        // room available: nothing happens, no allocator call
        const int before = g.allocs;
        CHECK(vec_reserve_a82050(v, 1, contract()).ok && g.allocs == before);
        // a larger request than 1.5x wins: size 1, n = 100 -> want 101
        const VecReserveResult r3 = vec_reserve_a82050(v, 100, contract());
        CHECK(r3.ok && r3.new_capacity == 101 && g.last_size == 101 * 8);
        // length_error / bad_alloc arithmetic
        const AllocDispatchContract none{};
        RecordPtrVec w{};
        const VecReserveResult le = vec_reserve_a82050(w, kVecMaxElements + 1, none);
        CHECK(!le.ok && le.refusal == RecordRegistryRefusal::length_error && !le.grew);
        release();
    }

    // --- refusals leave the failing vector untouched -------------------------------------------------------------------------
    {
        RecordRegistryCell cell{};
        NamedRecord rec;
        poison(rec);
        // missing allocator: no fake success
        const AllocDispatchContract none{};
        const NamedRecordRegistration a = register_named_record_a81700(cell, rec, view("n"), 3, 1, none);
        CHECK(!a.ok && a.refusal == RecordRegistryRefusal::no_allocator && !a.pushed_first && !a.pushed_second);
        CHECK(cell.registry.first.begin == nullptr && cell.registry.first.end == nullptr && cell.registry.second.begin == nullptr);
        CHECK(a.registry_initialised_now && cell.guard == 1);              // the registry is initialised before the push
        CHECK(rec.kind == 3);                                              // the record is always written first
        // allocator that fails the first allocation
        g = Counter{};
        g.fail_alloc_after = 1;
        const NamedRecordRegistration b = register_named_record_a81700(cell, rec, view("n"), 3, 1, contract());
        CHECK(!b.ok && b.refusal == RecordRegistryRefusal::allocation_failed && !b.pushed_first);
        CHECK(cell.registry.first.begin == nullptr && cell.registry.first.end == nullptr);
        CHECK(!b.registry_initialised_now);
        // first vector succeeds, the second allocation fails: first stays pushed (no rollback), second untouched
        g = Counter{};
        g.fail_alloc_after = 2;
        const NamedRecordRegistration c = register_named_record_a81700(cell, rec, view("n"), 3, 1, contract());
        CHECK(!c.ok && c.refusal == RecordRegistryRefusal::allocation_failed && c.pushed_first && !c.pushed_second);
        CHECK(len(cell.registry.first) == 1 && cell.registry.second.begin == nullptr && cell.registry.second.end == nullptr);
        release();
        // missing deallocate while a block exists: refused, vector unchanged
        RecordRegistryCell cell2{};
        std::vector<NamedRecord> recs(2);
        g = Counter{};
        CHECK(register_named_record_a81700(cell2, recs[0], view("n"), 1, 0, contract()).ok);
        AllocDispatchContract no_free = contract();
        no_free.deallocate = nullptr;
        NamedRecord** old_begin = cell2.registry.first.begin;
        const NamedRecordRegistration d = register_named_record_a81700(cell2, recs[1], view("n"), 2, 0, no_free);
        CHECK(!d.ok && d.refusal == RecordRegistryRefusal::no_allocator && !d.pushed_first);
        CHECK(cell2.registry.first.begin == old_begin && len(cell2.registry.first) == 1 && cap(cell2.registry.first) == 1);
        // a failing deallocate hook: the state is updated (original ignores the result), the refusal is reported
        g.fail_free = true;
        const NamedRecordRegistration e = register_named_record_a81700(cell2, recs[1], view("n"), 2, 0, contract());
        CHECK(!e.ok && e.refusal == RecordRegistryRefusal::free_failed && e.pushed_first && e.pushed_second);
        CHECK(len(cell2.registry.first) == 2 && len(cell2.registry.second) == 2);
        g.fail_free = false;
        release();
    }

    // --- the process cell accessor is a stable zero-initialised singleton ------------------------------------------------------
    {
        RecordRegistryCell& p = record_registry_cell_32fd398();
        CHECK(&p == &record_registry_cell_32fd398() && p.guard == 0 && p.registry.first.begin == nullptr);
    }
    std::puts("OK test_helper_a81700");
    return 0;
}
