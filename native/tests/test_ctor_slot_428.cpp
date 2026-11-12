// CPU-01-AOT-CFG-SCALE-01-BATCH19: fixture for startup slot 428 (0x52ec10): the static store table, five save descriptors with FNV hashes, 37 id entries
// (0xa59620) and 6 availability entries (0xa42960). ELF pins: tools/lift/check_ctor_slot_428.py. FIXTURE_VERIFIED.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include "core/ctor_slot_428_52ec10.hpp"

using namespace sotc;

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

struct Counter {
    int allocs = 0, frees = 0;
    int fail_alloc_at = -1;
    std::vector<void*> live;
} g;

void* hook_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    const int n = ++g.allocs;
    if (g.fail_alloc_at == n) return nullptr;
    void* p = std::calloc(1, size);
    g.live.push_back(p);
    return p;
}

bool hook_free(void* ptr, std::uint32_t) {
    ++g.frees;
    for (std::size_t i = 0; i < g.live.size(); ++i) {
        if (g.live[i] == ptr) {
            g.live.erase(g.live.begin() + static_cast<std::ptrdiff_t>(i));
            std::free(ptr);
            return true;
        }
    }
    return false;
}

struct NameDealloc final : SharedStrDeallocatorF590 {
    void deallocate(const void* allocation, SharedStrDeallocationKindF590) noexcept override { hook_free(const_cast<void*>(allocation), 0); }
};
NameDealloc name_dealloc;

std::unique_ptr<Slot428Objects> fresh() {
    g = Counter{};
    auto o = std::make_unique<Slot428Objects>();
    std::memset(static_cast<void*>(o.get()), 0, sizeof *o);
    o->alloc.allocate = &hook_alloc;
    o->alloc.deallocate = &hook_free;
    o->name_dealloc = &name_dealloc;
    return o;
}

std::uint32_t u32(const Slot428Objects& o, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, o.statics.bytes + off, 4);
    return v;
}
std::uint64_t u64(const Slot428Objects& o, std::size_t off) {
    std::uint64_t v;
    std::memcpy(&v, o.statics.bytes + off, 8);
    return v;
}
const char* cptr(const Slot428Objects& o, std::size_t off) { return reinterpret_cast<const char*>(static_cast<std::uintptr_t>(u64(o, off))); }

void test_full_run() {
    auto o = fresh();
    ConCmdRegistry concmd;
    const Slot428Result r = init_slot428_52ec10(*o, concmd);
    CHECK(r.ok && r.statics_applied && r.saves_done == 5 && r.ids_done == 37 && r.avails_done == 6 && r.atexit_recorded == 49);

    // atexit log: statics (null object), 5 records, 37 id entries, 6 availability entries, in the original order.
    CHECK(concmd.atexit_log.size() == kSlot428AtexitCount);
    CHECK(concmd.atexit_log.at(0).dtor_rva == 0x52e980 && concmd.atexit_log.at(0).object == nullptr);
    for (std::size_t i = 0; i < 5; ++i) {
        CHECK(concmd.atexit_log.at(1 + i).dtor_rva == 0x52eaa0 && concmd.atexit_log.at(1 + i).object == &o->saves[i]);
    }
    for (std::size_t i = 0; i < 37; ++i) {
        CHECK(concmd.atexit_log.at(6 + i).dtor_rva == 0x52eb00 && concmd.atexit_log.at(6 + i).object == &o->ids[i]);
    }
    for (std::size_t i = 0; i < 6; ++i) {
        CHECK(concmd.atexit_log.at(43 + i).dtor_rva == 0x52eb10 && concmd.atexit_log.at(43 + i).object == &o->avails[i]);
    }

    // A. spot checks of the static object: the first dwords, the first string view, a withheld 256 B string.
    CHECK(u32(*o, 0x0) == 0x72 && u32(*o, 0x4) == 0x100072);
    CHECK(std::strcmp(cptr(*o, 0x10), "GLOBAL") == 0 && u64(*o, 0x18) == 6);
    CHECK(std::strcmp(cptr(*o, 0x50), "CUSA08034") == 0 && u64(*o, 0x58) == 9);
    const char* withheld = cptr(*o, 0x70);
    CHECK(u64(*o, 0x78) == kSlot428WithheldLength && std::strlen(withheld) == kSlot428WithheldLength);
    for (std::size_t i = 0; i < kSlot428WithheldLength; ++i) CHECK(withheld[i] == 'X');
    CHECK(u32(*o, 0x48) == 0x7ffdfee && u32(*o, 0x4c) == 0x72);

    // B. save descriptors
    CHECK(o->saves[0].id == 2 && std::strcmp(reinterpret_cast<const char*>(o->saves[0].name.data), "CollectionProfile") == 0 && o->saves[0].name.size == 17);
    CHECK(o->saves[0].name.aux == 0 && o->saves[0].name.ctl == nullptr && o->saves[0].flag == 1);
    CHECK(o->saves[0].hash[0] == 0x65746b20u && o->saves[0].hash[1] == 0x2142cb33u && o->saves[0].hash[2] == 0x8ab16654u && o->saves[0].hash[3] == 0xecc2ca37u);
    CHECK(o->saves[2].id == 4 && o->saves[2].flag == 339 && o->saves[2].hash[0] == 0xcbd57a10u);
    for (std::size_t i = 0; i < 5; ++i) {
        CHECK(std::memcmp(o->saves[i].gap04, "\0\0\0\0", 4) == 0 && std::memcmp(o->saves[i].gap3c, "\0\0\0\0", 4) == 0 && o->saves[i].zero50 == 0);
    }

    // C. id entries and V1
    CHECK(o->id_vec.begin != nullptr && static_cast<std::size_t>(o->id_vec.end - o->id_vec.begin) == 37 && static_cast<std::size_t>(o->id_vec.cap - o->id_vec.begin) == 64);
    for (std::size_t i = 0; i < 37; ++i) {
        CHECK(o->id_vec.begin[i] == &o->ids[i]);
        CHECK(o->ids[i].id == i + 1);
        CHECK(o->ids[i].str.size == static_cast<std::int64_t>(kSlot428IdSpecs[i].text_length));
        CHECK(std::memcmp(o->ids[i].str.data, kSlot428IdSpecs[i].text, kSlot428IdSpecs[i].text_length) == 0 && o->ids[i].str.ctl == nullptr);
    }
    CHECK(std::strcmp(kSlot428IdSpecs[36].text, "Kill Colossus 16") == 0);

    // D. availability entries, V2 and the six queued variables
    CHECK(static_cast<std::size_t>(o->avail_vec.end - o->avail_vec.begin) == 6 && static_cast<std::size_t>(o->avail_vec.cap - o->avail_vec.begin) == 8);
    for (std::size_t i = 0; i < 6; ++i) {
        CHECK(o->avail_vec.begin[i] == &o->avails[i]);
        CHECK(o->avails[i].var != nullptr);
        CHECK(std::memcmp(o->avails[i].a.data, kSlot428AvailSpecs[i].a, kSlot428AvailSpecs[i].a_length) == 0);
        CHECK(std::memcmp(o->avails[i].b.data, kSlot428AvailSpecs[i].b, kSlot428AvailSpecs[i].b_length) == 0);
    }
    char want[64];
    CHECK(format_entitlement_name(want, sizeof want, "ANCIENTBOW000001"));
    CHECK(std::strcmp(reinterpret_cast<const char*>(o->avails[0].var->base.name.data), want) == 0);
    CHECK(concmd.pending.size() == 6);

    // allocations: V1 growth 1,2,4,8,16,32,64 = 7 allocs / 6 frees; V2 growth 1,2,4,8 = 4 allocs / 3 frees; 6 variables; 6 owned names (data + control block).
    CHECK(g.allocs == 7 + 4 + 6 + 12);
    CHECK(g.frees == 6 + 3);
}

void test_missing_contract() {
    auto o = fresh();
    o->alloc.allocate = nullptr;
    ConCmdRegistry concmd;
    const Slot428Result r = init_slot428_52ec10(*o, concmd);
    CHECK(!r.ok && !r.statics_applied && r.atexit_recorded == 0 && concmd.atexit_log.size() == 0);
    CHECK(u32(*o, 0x0) == 0 && o->saves[0].id == 0 && o->ids[0].id == 0);

    o = fresh();
    o->name_dealloc = nullptr;
    ConCmdRegistry c2;
    CHECK(!init_slot428_52ec10(*o, c2).ok && c2.atexit_log.size() == 0);
}

void test_alloc_failure() {
    // Failing the very first allocation (the V1 vector) stops the slot after phases A and B with no id entry registered.
    auto o = fresh();
    g.fail_alloc_at = 1;
    ConCmdRegistry concmd;
    Slot428Result r = init_slot428_52ec10(*o, concmd);
    CHECK(!r.ok && r.statics_applied && r.saves_done == 5 && r.ids_done == 0 && r.avails_done == 0 && r.atexit_recorded == 6);
    CHECK(concmd.atexit_log.size() == 6);

    // Failing the first allocation after the 37 id entries (V1 growth allocations: 7) stops inside phase D.
    o = fresh();
    g.fail_alloc_at = 8;
    ConCmdRegistry c2;
    r = init_slot428_52ec10(*o, c2);
    CHECK(!r.ok && r.ids_done == 37 && r.avails_done == 0 && r.atexit_recorded == 43);
}

void test_default_contract_and_repeat() {
    // The default malloc / free contract runs the whole slot; two independent states produce identical tables.
    auto a = std::make_unique<Slot428Objects>();
    auto b = std::make_unique<Slot428Objects>();
    std::memset(static_cast<void*>(a.get()), 0, sizeof *a);
    std::memset(static_cast<void*>(b.get()), 0, sizeof *b);
    a->alloc = b->alloc = default_slot428_alloc();
    a->name_dealloc = b->name_dealloc = default_slot428_name_dealloc();
    ConCmdRegistry ca, cb;
    CHECK(init_slot428_52ec10(*a, ca).ok && init_slot428_52ec10(*b, cb).ok);
    CHECK(std::memcmp(a->saves[1].hash, b->saves[1].hash, sizeof a->saves[1].hash) == 0 && a->saves[1].id == 13);
    CHECK(std::memcmp(a->statics.bytes + 0x4, b->statics.bytes + 0x4, 4) == 0);
    CHECK(ca.atexit_log.size() == 49 && cb.atexit_log.size() == 49);
}

void test_tables() {
    CHECK(sizeof(Slot428SaveRecord) == 0x58);
    CHECK(kSlot428StoreCount == 98 && kSlot428StoreStringCount == 16);
    // the withheld pool strings carry the original length and no secret text
    std::size_t withheld = 0;
    for (std::size_t i = 0; i < kSlot428StoreStringCount; ++i) {
        const char* s = kSlot428StoreStringBlob + kSlot428StoreStringOffset[i];
        if (std::strlen(s) == kSlot428WithheldLength) {
            ++withheld;
            for (std::size_t k = 0; k < kSlot428WithheldLength; ++k) CHECK(s[k] == 'X');
        }
    }
    CHECK(withheld == 4);
}

}  // namespace

int main() {
    test_tables();
    test_full_run();
    test_missing_contract();
    test_alloc_failure();
    test_default_contract_and_repeat();
    std::puts("test_ctor_slot_428 OK");
    return 0;
}
