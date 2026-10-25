// Measured prologue 0x255180 early → base Initialize → remainder → registry.
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_prologue_slice.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/prologue_255180.hpp"
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
        bytes += size;
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
    std::uint64_t bytes = 0;
};

int g_ctor_a30 = 0;
int g_dtor = 0;
int g_11e70 = 0;
int g_3df40 = 0;
int g_335d0 = 0;
int g_tls = 0;
int g_v30 = 0;
int g_mid = 0;
int g_v40 = 0;

void ctor_a30(void* obj) {
    ++g_ctor_a30;
    *static_cast<std::uint32_t*>(obj) = 0xA30A30A3u;
}
void dtor_slot(void*) { ++g_dtor; }
void hook_11e70() { ++g_11e70; }
void hook_3df40() { ++g_3df40; }
void hook_335d0(void*, const PrologueFlagConfig255180&) { ++g_335d0; }
void hook_tls(void*, std::uint8_t v) {
    ++g_tls;
    CHECK(v == 1);
}
bool hook_v30(void*) {
    ++g_v30;
    return true;
}
void hook_mid(void*) { ++g_mid; }
void hook_v40(void*, void*) { ++g_v40; }

}  // namespace

int main() {
    CHECK(kPrologue255180Size == 448);
    CHECK(kPrologueA30Bytes == 0xa30);
    CHECK(kFlagObject335a0Bytes == 0x40);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    // Fail-visible: missing a30 ctor.
    {
        Prologue255180Hooks ph{};
        const auto miss = run_prologue_255180_early(front, ph);
        CHECK(!miss.ok);
        CHECK(miss.a30_obj == nullptr);
    }

    InputTable551140 table{};
    init_input_table_551140(table);
    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        // Still exercise prologue-only success without registry names.
        Prologue255180Hooks ph{};
        ph.ctor_a30_264e50 = ctor_a30;
        ph.dtor_vslot08 = dtor_slot;
        ph.call_11e70 = hook_11e70;
        ph.call_3df40 = hook_3df40;
        ph.apply_335d0 = hook_335d0;
        ph.tls_35770 = hook_tls;
        const auto p = run_prologue_255180_early(front, ph);
        CHECK(p.ok);
        CHECK(p.flag_word_20 == 1);
        CHECK(prologue_a30_cell_2dfd4b8() == p.a30_obj);
        CHECK(base_init_flag_cell_2ea4718() == p.flag_obj);
        std::puts("appmain_init_prologue_slice: prologue-only OK (fixture missing)");
        return 0;
    }
    CHECK(apply_inputtable_names_fixture(table, fix));

    std::array<std::uint8_t, 0x60> app{};
    std::array<std::uint8_t, 0x60> setter{};
    std::memset(setter.data(), 0xCC, setter.size());

    g_ctor_a30 = g_dtor = g_11e70 = g_3df40 = g_335d0 = g_tls = g_v30 = g_mid = g_v40 = 0;
    Prologue255180Hooks ph{};
    ph.ctor_a30_264e50 = ctor_a30;
    ph.dtor_vslot08 = dtor_slot;
    ph.call_11e70 = hook_11e70;
    ph.call_3df40 = hook_3df40;
    ph.apply_335d0 = hook_335d0;
    ph.tls_35770 = hook_tls;

    BaseInitialize255130Hooks bh{};
    bh.vslot_30 = hook_v30;
    bh.mid_255340 = hook_mid;
    bh.vslot_40 = hook_v40;

    const std::uint64_t allocs_before = heap.alloc_count;
    const auto t0 = std::chrono::steady_clock::now();
    const auto r = run_appmain_init_prologue_slice(table, front, app.data(), nullptr, ph, bh,
                                                   setter.data(), 0, 0x11, 0x22, 0x33);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    CHECK(r.prologue.ok);
    CHECK(r.prologue.flag_word_20 == 1);
    CHECK(g_ctor_a30 == 1);
    CHECK(g_11e70 == 1);
    CHECK(g_3df40 == 1);
    CHECK(g_335d0 == 1);
    CHECK(g_tls == 1);
    CHECK(r.base_ok);
    CHECK(r.flag_after_base == 0);  // orchestrator xchg(0)
    CHECK(g_v30 == 1);
    CHECK(g_mid == 1);
    CHECK(g_v40 == 1);
    CHECK(r.remainder.trivial_ok);
    CHECK(r.remainder.registry.status == RegistryInit531d60Status::ok);
    CHECK(r.remainder.registry.entry_count == kInputTable551140Count);
    // prologue: 0xa30 + 0x40; registry: object + 2 tables
    CHECK(heap.alloc_count - allocs_before >= 5);

    // Re-run prologue replaces previous a30/flag (dtor twice).
    const auto r2 = run_prologue_255180_early(front, ph);
    CHECK(r2.ok);
    CHECK(g_dtor >= 2);

    std::printf(
        "appmain_init_prologue_slice: MEASURED prologue+base+trivial+registry ok "
        "a30=%p flag=%p flag20_pre_base=1 flag_after_base=%u entries=%u alloc_ops=%llu "
        "duration_us=%lld allocator=tip_AllocatorFront+FakeHeap "
        "prologue_tail_0x255264=NOT_modelled pr5_heap_merge=GAP\n",
        r.prologue.a30_obj, r.prologue.flag_obj, r.flag_after_base,
        r.remainder.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    std::puts("appmain_init_prologue_slice: all checks passed");
    return 0;
}
