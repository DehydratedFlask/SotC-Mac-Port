// Measured prologue early+tail with lifted 0x3be050 header+buf-grow → base → registry.
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_ctor_98a90_slice.hpp"
#include "core/ctor_98a90_3be050.hpp"
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

int g_3be930 = 0;
int g_323d0 = 0;
int g_ensure = 0;
int g_list = 0;
int g_f590 = 0;
int g_ctor_a30 = 0;
int g_ctor_40 = 0;
int g_dtor_98a90 = 0;
int g_dtor_3b40 = 0;
int g_dtor_early = 0;
int g_v30 = 0;
int g_mid = 0;
int g_v40 = 0;

void ctor_a30(void* obj) {
    ++g_ctor_a30;
    *static_cast<std::uint32_t*>(obj) = 0xA30A30A3u;
}
void ctor_40(void* obj) {
    ++g_ctor_40;
    *static_cast<std::uint32_t*>(obj) = 0x40404040u;
}
void dtor_early(void*) { ++g_dtor_early; }
void dtor_98a90(void*) { ++g_dtor_98a90; }
void dtor_3b40(void*) { ++g_dtor_3b40; }
void hook_3be930(void*) { ++g_3be930; }
void hook_323d0(void* p) {
    ++g_323d0;
    *static_cast<std::uint64_t*>(p) = 0x1234567890ABCDEFULL;
}
void hook_ensure(void*, std::uint32_t n) {
    ++g_ensure;
    CHECK(n == 0x100);
}
void hook_list(void*) { ++g_list; }
void hook_f590(void*) { ++g_f590; }

bool hook_v30(void*) {
    ++g_v30;
    return true;
}
void hook_mid(void*) { ++g_mid; }
void hook_v40(void*, void*) { ++g_v40; }

void* loadp(const void* base, std::size_t off) {
    void* p = nullptr;
    std::memcpy(&p, static_cast<const std::uint8_t*>(base) + off, sizeof(void*));
    return p;
}

}  // namespace

int main() {
    CHECK(kCtor98a90_3be050Size == 627);
    CHECK(kCtor98a90BufBytes == 0x80);
    CHECK(kCtor98a90ElemBytes == 0x20);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    // Unit: header + empty-path 0x80 grow.
    {
        std::vector<std::uint8_t> obj(kPrologue98a90Bytes, 0);
        Ctor98a90_3be050Hooks ch{};
        ch.sub_3be930 = hook_3be930;
        ch.store_tsc_323d0 = hook_323d0;
        ch.ensure_3c2f70 = hook_ensure;
        ch.elem_dtor_f590 = hook_f590;
        ch.list_walk_from_3be1ce = hook_list;
        g_3be930 = g_323d0 = g_ensure = g_list = g_f590 = 0;
        const auto cr = init_98a90_3be050(front, obj.data(), ch);
        CHECK(cr.ok);
        CHECK(cr.nested_48 != nullptr);
        CHECK(cr.ensure_called);
        CHECK(cr.buf_grown);
        CHECK(cr.buf_80 != nullptr);
        CHECK(g_3be930 == 1);
        CHECK(g_323d0 == 1);
        CHECK(g_ensure == 1);
        CHECK(g_list == 1);
        CHECK(g_f590 == 0);  // empty migrate
        void* begin = loadp(obj.data(), kCtor98a90VecBegin);
        void* end = loadp(obj.data(), kCtor98a90VecEnd);
        void* cap = loadp(obj.data(), kCtor98a90VecCap);
        CHECK(begin == cr.buf_80);
        CHECK(end == begin);
        CHECK(static_cast<std::uint8_t*>(cap) == static_cast<std::uint8_t*>(begin) + 0x80);
    }

    InputTable551140 table{};
    init_input_table_551140(table);
    InputtableNamesFixture fix;
    CHECK(load_inputtable_names_fixture_default(fix));
    CHECK(apply_inputtable_names_fixture(table, fix));

    std::vector<std::uint8_t> app(kPrologueAppMinBytes, 0);
    std::array<std::uint8_t, 0x60> setter{};
    std::memset(setter.data(), 0xCC, setter.size());

    g_ctor_a30 = g_ctor_40 = g_dtor_98a90 = g_dtor_3b40 = g_dtor_early = 0;
    g_3be930 = g_323d0 = g_ensure = g_list = g_f590 = g_v30 = g_mid = g_v40 = 0;

    Prologue255180Hooks eh{};
    eh.ctor_a30_264e50 = ctor_a30;
    eh.dtor_vslot08 = dtor_early;

    Prologue255180TailHooks th{};
    th.ctor_40_1ab550 = ctor_40;
    th.dtor_98a90_3be390 = dtor_98a90;
    th.dtor_vslot08_3b40 = dtor_3b40;

    Ctor98a90_3be050Hooks ch{};
    ch.sub_3be930 = hook_3be930;
    ch.store_tsc_323d0 = hook_323d0;
    ch.ensure_3c2f70 = hook_ensure;
    ch.elem_dtor_f590 = hook_f590;
    ch.list_walk_from_3be1ce = hook_list;

    BaseInitialize255130Hooks bh{};
    bh.vslot_30 = hook_v30;
    bh.mid_255340 = hook_mid;
    bh.vslot_40 = hook_v40;

    const std::uint64_t allocs_before = heap.alloc_count;
    const auto t0 = std::chrono::steady_clock::now();
    const auto r = run_appmain_init_ctor_98a90_slice(table, front, app.data(), nullptr, eh, th, ch,
                                                     bh, setter.data(), 0, 0x11, 0x22, 0x33);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    CHECK(r.early.ok);
    CHECK(r.tail.ok);
    CHECK(r.ctor.ok);
    CHECK(r.ctor.nested_48 != nullptr);
    CHECK(g_3be930 == 1);
    CHECK(g_323d0 == 1);
    CHECK(g_ensure == 1);
    CHECK(g_list == 1);
    CHECK(g_ctor_40 == 1);
    CHECK(r.base_ok);
    CHECK(r.flag_after_base == 0);
    CHECK(r.remainder.trivial_ok);
    CHECK(r.remainder.registry.status == RegistryInit531d60Status::ok);
    CHECK(r.remainder.registry.entry_count == kInputTable551140Count);
    // early 2 + 0x98a90 + nested 0x48 + buf 0x80 + tail 0x40 + registry 3 ≥ 9
    CHECK(heap.alloc_count - allocs_before >= 9);

    void* begin = loadp(r.tail.obj_98a90, kCtor98a90VecBegin);
    void* cap = loadp(r.tail.obj_98a90, kCtor98a90VecCap);
    CHECK(begin != nullptr);
    CHECK(static_cast<std::uint8_t*>(cap) == static_cast<std::uint8_t*>(begin) + 0x80);

    std::printf(
        "appmain_init_ctor_98a90_slice: MEASURED lifted_3be050_header+buf80+early+tail+base+"
        "trivial+registry ok nested=%p buf80=%p entries=%u alloc_ops=%llu duration_us=%lld "
        "allocator=tip_AllocatorFront+FakeHeap "
        "list_walk_0x3be1ce=hooked bodies_3be930/323d0/3c2f70/f590=NOT_modelled "
        "pr5_heap_merge=GAP\n",
        r.ctor.nested_48, begin, r.remainder.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    std::puts("appmain_init_ctor_98a90_slice: all checks passed");
    return 0;
}
