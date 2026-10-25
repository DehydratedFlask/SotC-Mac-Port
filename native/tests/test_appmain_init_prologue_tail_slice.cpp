// Measured prologue early + tail (0x98a90 / app+0x3b58/+0x3b40) → base → remainder → registry.
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_prologue_tail_slice.hpp"
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
int g_ctor_98a90 = 0;
int g_ctor_40 = 0;
int g_dtor_98a90 = 0;
int g_dtor_3b40 = 0;
int g_dtor_early = 0;
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
void ctor_98a90(void* obj) {
    ++g_ctor_98a90;
    *static_cast<std::uint32_t*>(obj) = 0x98A90001u;
}
void ctor_40(void* obj) {
    ++g_ctor_40;
    *static_cast<std::uint32_t*>(obj) = 0x40404040u;
}
void dtor_early(void*) { ++g_dtor_early; }
void dtor_98a90(void*) { ++g_dtor_98a90; }
void dtor_3b40(void*) { ++g_dtor_3b40; }
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

void*& slot(void* self, std::size_t off) {
    return *reinterpret_cast<void**>(static_cast<std::uint8_t*>(self) + off);
}

}  // namespace

int main() {
    CHECK(kPrologue98a90Bytes == 0x98a90);
    CHECK(kPrologueAppSlot3b58 == 0x3b58);
    CHECK(kPrologueAppSlot3b40 == 0x3b40);
    CHECK(kPrologueAppMinBytes == 0x3b60);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    std::vector<std::uint8_t> app(kPrologueAppMinBytes, 0);

    // Fail-visible: missing 98a90 ctor.
    {
        Prologue255180TailHooks th{};
        th.ctor_40_1ab550 = ctor_40;
        const auto miss = run_prologue_255180_tail(front, app.data(), th);
        CHECK(!miss.ok);
        CHECK(miss.obj_98a90 == nullptr);
    }

    InputTable551140 table{};
    init_input_table_551140(table);
    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        Prologue255180Hooks eh{};
        eh.ctor_a30_264e50 = ctor_a30;
        eh.dtor_vslot08 = dtor_early;
        const auto e = run_prologue_255180_early(front, eh);
        CHECK(e.ok);
        Prologue255180TailHooks th{};
        th.ctor_98a90_3be050 = ctor_98a90;
        th.dtor_98a90_3be390 = dtor_98a90;
        th.ctor_40_1ab550 = ctor_40;
        th.dtor_vslot08_3b40 = dtor_3b40;
        const auto t = run_prologue_255180_tail(front, app.data(), th);
        CHECK(t.ok);
        CHECK(slot(app.data(), kPrologueAppSlot3b58) == t.obj_98a90);
        CHECK(slot(app.data(), kPrologueAppSlot3b40) == t.obj_40);
        std::puts("appmain_init_prologue_tail_slice: early+tail OK (fixture missing)");
        return 0;
    }
    CHECK(apply_inputtable_names_fixture(table, fix));

    std::array<std::uint8_t, 0x60> setter{};
    std::memset(setter.data(), 0xCC, setter.size());
    std::memset(app.data(), 0, app.size());

    g_ctor_a30 = g_ctor_98a90 = g_ctor_40 = g_dtor_98a90 = g_dtor_3b40 = g_dtor_early = 0;
    g_11e70 = g_3df40 = g_335d0 = g_tls = g_v30 = g_mid = g_v40 = 0;

    Prologue255180Hooks eh{};
    eh.ctor_a30_264e50 = ctor_a30;
    eh.dtor_vslot08 = dtor_early;
    eh.call_11e70 = hook_11e70;
    eh.call_3df40 = hook_3df40;
    eh.apply_335d0 = hook_335d0;
    eh.tls_35770 = hook_tls;

    Prologue255180TailHooks th{};
    th.ctor_98a90_3be050 = ctor_98a90;
    th.dtor_98a90_3be390 = dtor_98a90;
    th.ctor_40_1ab550 = ctor_40;
    th.dtor_vslot08_3b40 = dtor_3b40;

    BaseInitialize255130Hooks bh{};
    bh.vslot_30 = hook_v30;
    bh.mid_255340 = hook_mid;
    bh.vslot_40 = hook_v40;

    const std::uint64_t allocs_before = heap.alloc_count;
    const auto t0 = std::chrono::steady_clock::now();
    const auto r = run_appmain_init_prologue_tail_slice(table, front, app.data(), nullptr, eh, th,
                                                        bh, setter.data(), 0, 0x11, 0x22, 0x33);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    CHECK(r.early.ok);
    CHECK(r.early.flag_word_20 == 1);
    CHECK(r.tail.ok);
    CHECK(g_ctor_a30 == 1);
    CHECK(g_ctor_98a90 == 1);
    CHECK(g_ctor_40 == 1);
    CHECK(slot(app.data(), kPrologueAppSlot3b58) == r.tail.obj_98a90);
    CHECK(slot(app.data(), kPrologueAppSlot3b40) == r.tail.obj_40);
    CHECK(r.base_ok);
    CHECK(r.flag_after_base == 0);
    CHECK(g_v30 == 1);
    CHECK(g_mid == 1);
    CHECK(g_v40 == 1);
    CHECK(r.remainder.trivial_ok);
    CHECK(r.remainder.registry.status == RegistryInit531d60Status::ok);
    CHECK(r.remainder.registry.entry_count == kInputTable551140Count);
    CHECK(r.tail.gates.ok);
    CHECK(r.tail.gates.ba40_ran);
    // early: 0xa30+0x40; tail: 0x98a90+0x40; registry: object+2 tables → >= 7
    CHECK(heap.alloc_count - allocs_before >= 7);

    // Re-run tail replaces previous slots (dtor_98a90 + delete_; vslot-only for 3b40).
    const auto r2 = run_prologue_255180_tail(front, app.data(), th);
    CHECK(r2.ok);
    CHECK(r2.gates.ok);
    CHECK(g_dtor_98a90 >= 1);
    CHECK(g_dtor_3b40 >= 1);

    std::printf(
        "appmain_init_prologue_tail_slice: MEASURED early+tail+gates2552d4+base+trivial+registry ok "
        "obj98a90=%p obj40=%p gate_byte=%u flag_after_base=%u entries=%u alloc_ops=%llu "
        "duration_us=%lld allocator=tip_AllocatorFront+FakeHeap "
        "stackchk/ba40_listeners=NOT_modelled pr5_heap_merge=GAP\n",
        r.tail.obj_98a90, r.tail.obj_40, static_cast<unsigned>(r.tail.gates.gate_byte),
        r.flag_after_base, r.remainder.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    std::puts("appmain_init_prologue_tail_slice: all checks passed");
    return 0;
}
