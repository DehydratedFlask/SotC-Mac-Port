// CPU-01-AOT-CFG-SCALE-01-BATCH19: fixture for the availability entry constructor 0xa42960 (two string views, a formatted-name bool var,
// the V2 doubling push). ELF pins: tools/lift/check_helper_a42960.py. FIXTURE_VERIFIED.
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/helper_a42960_avail_entry.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

struct Counter {
    int allocs = 0, frees = 0;
    std::size_t sizes[8] = {};
    std::uint32_t tags[8] = {}, flags[8] = {};
    int fail_alloc_at = -1;
    std::vector<void*> live;
} g;

void* hook_alloc(std::size_t size, std::uint32_t tag, std::uint32_t, std::uint32_t flags) {
    const int n = ++g.allocs;
    if (n <= 8) {
        g.sizes[n - 1] = size;
        g.tags[n - 1] = tag;
        g.flags[n - 1] = flags;
    }
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
    int data = 0, control = 0;
    void deallocate(const void* allocation, SharedStrDeallocationKindF590 kind) noexcept override {
        (kind == SharedStrDeallocationKindF590::data ? data : control)++;
        hook_free(const_cast<void*>(allocation), 0);
    }
};

NameDealloc name_dealloc;

AvailEntryHooks hooks() {
    AvailEntryHooks h;
    h.alloc.allocate = &hook_alloc;
    h.alloc.deallocate = &hook_free;
    h.name_alloc.alloc = h.alloc;
    h.name_dealloc = &name_dealloc;
    return h;
}

std::size_t cap(const PtrVec24& v) { return static_cast<std::size_t>(v.cap - v.begin); }
std::size_t len(const PtrVec24& v) { return static_cast<std::size_t>(v.end - v.begin); }

void reset() {
    g = Counter{};
    name_dealloc = NameDealloc{};
}

void test_single() {
    reset();
    PtrVec24 vec{};
    AvailEntry entry;
    poison(entry);
    ConCmdRegistry registry;
    std::int64_t ctl_a = 3, ctl_b = 9;
    const SharedStr a{reinterpret_cast<const std::uint8_t*>("FullGame"), 8, 15, &ctl_a};
    const SharedStr b{reinterpret_cast<const std::uint8_t*>("beta"), 4, 7, &ctl_b};
    const AvailEntryResult r = register_avail_entry_a42960(vec, entry, a, b, registry, hooks());
    CHECK(r.ok && r.stage == AvailEntryStage::ok && !r.a_aliased && !r.b_aliased && r.grew && r.new_capacity == 1);
    CHECK(entry.a.data == a.data && entry.a.size == 8 && entry.a.aux == 15 && entry.a.ctl == &ctl_a && ctl_a == 4);
    CHECK(entry.b.data == b.data && entry.b.size == 4 && entry.b.aux == 7 && entry.b.ctl == &ctl_b && ctl_b == 10);
    CHECK(entry.var != nullptr && len(vec) == 1 && vec.begin[0] == &entry);
    ConCmdBoolVar& var = *entry.var;
    CHECK(var.base.vptr == kBoolVarFinalVptr && var.base.flags == 1);
    CHECK(var.value == 0 && var.default_value == 0 && var.value_ref == &var.value);
    CHECK(var.fn_vptr == kAvailVarFnVtbl && var.fn_payload == 0 && var.fn_aux == 0);
    CHECK(var.base.name.size == 22 + 8);
    CHECK(std::memcmp(var.base.name.data, "entitlement_available_FullGame", 30) == 0);
    CHECK(var.base.name.ctl != nullptr && *var.base.name.ctl == 1);          // the caller's reference was released (2 -> 1)
    CHECK(var.base.description.size == 0 && var.base.description.ctl == nullptr);
    CHECK(var.base.registered == 1 && registry.pending.size() == 1);          // queued on the pending list
    CHECK(registry.atexit_log.size() == 0);                                   // vector-owned: no atexit record
    CHECK(g.sizes[0] == 0xc8 && g.tags[0] == 1 && g.flags[0] == 1);           // 0x17070
    CHECK(g.sizes[3] == 8 && g.flags[3] == 2);                                // 0x17090 (the first V2 block)
    CHECK(name_dealloc.data == 0 && name_dealloc.control == 0);               // the var still holds the name
}

void test_aliases_and_growth() {
    reset();
    PtrVec24 vec{};
    ConCmdRegistry registry;
    AvailEntry entries[5];
    const SharedStr a = view("x"), b = view("y");
    const std::size_t want[] = {1, 2, 4, 4, 8};
    int grows = 0;
    for (std::size_t i = 0; i < 5; ++i) {
        poison(entries[i]);
        const AvailEntryResult r = register_avail_entry_a42960(vec, entries[i], a, b, registry, hooks());
        CHECK(r.ok && cap(vec) == want[i] && len(vec) == i + 1);
        if (r.grew) ++grows;
        for (std::size_t k = 0; k <= i; ++k) CHECK(vec.begin[k] == &entries[k]);
    }
    CHECK(grows == 4 && registry.pending.size() == 5);
    CHECK(g.frees == 3);                                                      // every old V2 block but the first
    // aliasing: the inputs are the entry's own strings (emptied by 0xf180 before the compare)
    AvailEntry self;
    poison(self);
    PtrVec24 vec2{};
    const AvailEntryResult r = register_avail_entry_a42960(vec2, self, self.a, self.b, registry, hooks());
    CHECK(r.a_aliased && r.b_aliased);
    CHECK(self.a.size == 0 && self.a.ctl == nullptr && self.b.size == 0 && self.b.ctl == nullptr);
}

void test_refusals() {
    reset();
    PtrVec24 vec{};
    ConCmdRegistry registry;
    const SharedStr a = view("A"), b = view("B");
    {   // no allocator: nothing is written
        AvailEntry entry;
        poison(entry);
        AvailEntryHooks h = hooks();
        h.alloc.allocate = nullptr;
        const AvailEntryResult r = register_avail_entry_a42960(vec, entry, a, b, registry, h);
        CHECK(!r.ok && r.stage == AvailEntryStage::hooks_missing && all_poison(entry));
        h = hooks();
        h.name_dealloc = nullptr;
        const AvailEntryResult r2 = register_avail_entry_a42960(vec, entry, a, b, registry, h);
        CHECK(!r2.ok && r2.stage == AvailEntryStage::hooks_missing && all_poison(entry));
    }
    {   // the 0xc8 B block allocation fails: strings are already written, the var stays null
        reset();
        g.fail_alloc_at = 1;
        AvailEntry entry;
        poison(entry);
        const AvailEntryResult r = register_avail_entry_a42960(vec, entry, a, b, registry, hooks());
        CHECK(!r.ok && r.stage == AvailEntryStage::var_alloc_failed && entry.var == nullptr);
        CHECK(entry.a.data == a.data && entry.b.data == b.data && vec.begin == nullptr && registry.pending.size() == 0);
    }
    {   // the name allocation fails (2nd allocation): the var block exists but is unregistered
        reset();
        g.fail_alloc_at = 2;
        AvailEntry entry;
        poison(entry);
        const AvailEntryResult r = register_avail_entry_a42960(vec, entry, a, b, registry, hooks());
        CHECK(!r.ok && r.stage == AvailEntryStage::name_failed && entry.var == nullptr && registry.pending.size() == 0);
        CHECK(g.live.size() == 1);                       // the leaked 0xc8 block, reported not hidden
        std::free(g.live[0]);
        g.live.clear();
    }
    {   // the V2 vector allocation fails: the var is registered, the push refused, the vector untouched
        reset();
        g.fail_alloc_at = 5;      // var(1), name data(2), name ctl(3), ... the vector block is the 4th hook call; 5 is unused
        g.fail_alloc_at = 4;
        AvailEntry entry;
        poison(entry);
        const AvailEntryResult r = register_avail_entry_a42960(vec, entry, a, b, registry, hooks());
        CHECK(!r.ok && r.stage == AvailEntryStage::push_failed && r.push_refusal == PtrVecRefusal::allocation_failed);
        CHECK(entry.var != nullptr && registry.pending.size() == 1 && vec.begin == nullptr);
    }
}

}  // namespace

int main() {
    test_single();
    test_aliases_and_growth();
    test_refusals();
    std::puts("test_helper_a42960 OK");
    return 0;
}
