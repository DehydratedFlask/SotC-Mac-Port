// Measured prologue early+tail with lifted 0x3be050 + lifted 0x1ab550 → registry.
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_ctor_98a90_slice.hpp"
#include "core/ctor_40_1ab550.hpp"
#include "core/ctor_98a90_3be050.hpp"
#include "core/ensure_capacity_3c2f70.hpp"
#include "core/init_sub_3be930.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/mid_255340.hpp"
#include "core/prologue_255180.hpp"
#include "core/prologue_gates_2552d4.hpp"
#include "core/registry_init_531d60.hpp"
#include "core/store_tsc_323d0.hpp"
#include "core/vslot_30_534130.hpp"

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

int g_nonnull = 0;
int g_f590 = 0;
int g_init_tb = 0;
int g_move = 0;
int g_ctor_a30 = 0;
int g_dtor_98a90 = 0;
int g_dtor_3b40 = 0;
int g_dtor_early = 0;
int g_mid_reg = 0;
int g_mid_b60 = 0;
int g_v40 = 0;

void ctor_a30(void* obj) {
    ++g_ctor_a30;
    *static_cast<std::uint32_t*>(obj) = 0xA30A30A3u;
}
void dtor_early(void*) { ++g_dtor_early; }
void dtor_98a90(void*) { ++g_dtor_98a90; }
void dtor_3b40(void*) { ++g_dtor_3b40; }
void hook_nonnull(void*, void*) { ++g_nonnull; }
void hook_f590(void*) { ++g_f590; }
void hook_move(void*, void*) { ++g_move; }
std::uint64_t hook_init_tb() {
    ++g_init_tb;
    return 0xF8F8600000000001ULL;
}

void hook_mid_26ea0(void*, const char*, const char*) { ++g_mid_reg; }
void hook_mid_26b60(const char*) { ++g_mid_b60; }
void hook_v40(void*, void*) { ++g_v40; }

void* loadp(const void* base, std::size_t off) {
    void* p = nullptr;
    std::memcpy(&p, static_cast<const std::uint8_t*>(base) + off, sizeof(void*));
    return p;
}

void reset_cells() {
    ctor_98a90_self_cell_2ef63f8() = nullptr;
    ctor_98a90_list_head_cell_2ef6400() = nullptr;
    store_tsc_guard_cell_2ea4700() = 0;
    store_tsc_global_cell_2ea4708() = 0;
}

}  // namespace

int main() {
    CHECK(kInitSub3be930Size == 826);
    CHECK(kInitBlock1d9c0Size == 73);
    CHECK(kInitSlotF180Size == 28);
    CHECK(kEnsureCapacity3c2f70Size == 313);
    CHECK(ensure_3c2f70_elem_count(0, 0xa800) == 0x100);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    // Unit: sub-init 0x3be930 alone.
    {
        std::vector<std::uint8_t> region(0x98a28, 0xA5);
        init_sub_3be930(region.data());
        CHECK(region[0] == 0);
        void* vt = loadp(region.data(), 0x18);
        CHECK(vt == slot_vtable_2dd9f78());
        std::uint32_t tail_d = 0;
        std::memcpy(&tail_d, region.data() + 0x98a10, 4);
        CHECK(tail_d == 0);
        void* mid = loadp(region.data(), 0x10 + 0x30);
        CHECK(mid == region.data() + 0x10 + 0x58);
    }

    // Unit: ensure empty path alone.
    {
        std::uint8_t vec[0x20]{};
        EnsureCapacity3c2f70Hooks eh{};
        eh.elem_move_1da30 = hook_move;
        eh.elem_dtor_f590 = hook_f590;
        g_move = g_f590 = 0;
        const auto er = ensure_capacity_3c2f70(front, vec, 0x100, eh);
        CHECK(er.ok);
        CHECK(er.allocated);
        CHECK(er.buf != nullptr);
        CHECK(g_move == 0);
        CHECK(g_f590 == 0);
        void* b = loadp(vec, 0x08);
        void* e = loadp(vec, 0x10);
        void* c = loadp(vec, 0x18);
        CHECK(b == er.buf);
        CHECK(e == b);
        CHECK(static_cast<std::uint8_t*>(c) == static_cast<std::uint8_t*>(b) + 0xa800);
    }

    // Unit: ctor with lifted ensure (no override).
    {
        reset_cells();
        std::vector<std::uint8_t> obj(kPrologue98a90Bytes, 0);
        Ctor98a90_3be050Hooks ch{};
        ch.tsc_detail.init_timebase_f8f860 = hook_init_tb;
        ch.ensure_detail.elem_move_1da30 = hook_move;
        ch.elem_dtor_f590 = hook_f590;
        g_init_tb = g_move = g_f590 = 0;
        const auto cr = init_98a90_3be050(front, obj.data(), ch);
        CHECK(cr.ok);
        CHECK(cr.sub_3be930_ok);
        CHECK(cr.ensure_called);
        CHECK(cr.ensure_allocated);
        CHECK(cr.ensure_buf != nullptr);
        CHECK(g_move == 0);
        void* b = loadp(obj.data(), 0x28);
        void* c = loadp(obj.data(), 0x38);
        CHECK(b == cr.ensure_buf);
        CHECK(static_cast<std::uint8_t*>(c) == static_cast<std::uint8_t*>(b) + 0xa800);
        CHECK(cr.tsc_stored);
        CHECK(cr.buf_grown);
        CHECK(cr.list_walk_skipped_null);
    }

    reset_cells();

    InputTable551140 table{};
    init_input_table_551140(table);
    InputtableNamesFixture fix;
    CHECK(load_inputtable_names_fixture_default(fix));
    CHECK(apply_inputtable_names_fixture(table, fix));

    std::vector<std::uint8_t> app(kPrologueAppMinBytes, 0);
    std::array<std::uint8_t, 0x60> setter{};
    std::memset(setter.data(), 0xCC, setter.size());

    g_ctor_a30 = g_dtor_98a90 = g_dtor_3b40 = g_dtor_early = 0;
    g_nonnull = g_f590 = g_init_tb = g_move = g_mid_reg = g_mid_b60 = g_v40 = 0;
    ctor40_cell_2ee6778() = nullptr;
    prologue_gate_flag_byte() = 1;  // skip vslot+0x48
    prologue_debug_state_byte() = 0;
    vslot30_cell_boss_val_325e8b4() = -1;
    vslot30_cell_boss_mode_325e8b8() = -1;
    vslot30_cell_boss_flag_2dcdf14() = 0xFF;
    vslot30_cell_stage_2dd9578() = -1;

    Prologue255180Hooks eh{};
    eh.ctor_a30_264e50 = ctor_a30;
    eh.dtor_vslot08 = dtor_early;

    Prologue255180TailHooks th{};
    // ctor_40_1ab550 null → lifted init_ctor_40_1ab550 (slice also forces null)
    th.dtor_98a90_3be390 = dtor_98a90;
    th.dtor_vslot08_3b40 = dtor_3b40;
    th.run_gates = true;  // E417 default

    Ctor98a90_3be050Hooks ch{};
    ch.elem_dtor_f590 = hook_f590;
    ch.list_walk_nonnull_body = hook_nonnull;
    ch.tsc_detail.init_timebase_f8f860 = hook_init_tb;
    ch.ensure_detail.elem_move_1da30 = hook_move;
    // sub_3be930 override null → lifted init_sub_3be930

    BaseInitialize255130Hooks bh{};
    // vslot_30 / mid null → slice forces lifted 0x534130 + 0x255340
    bh.mid_255340_detail.register_26ea0 = hook_mid_26ea0;
    bh.mid_255340_detail.register_26b60 = hook_mid_26b60;
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
    CHECK(r.ctor.sub_3be930_ok);
    CHECK(r.ctor.ensure_allocated);
    CHECK(r.ctor.tsc_stored);
    CHECK(r.ctor.list_walk_skipped_null);
    CHECK(r.ctor_40.ok);
    CHECK(r.ctor_40.derived_vt);
    CHECK(r.ctor_40.cell_self);
    CHECK(r.ctor_40.field_0a);
    CHECK(r.tail.gates.ok);
    CHECK(r.tail.gates.gate_byte == 0);
    CHECK(!r.tail.gates.took_vslot_48);
    CHECK(r.tail.gates.ba40_ran);
    CHECK(!r.tail.gates.state_updated);  // al==state==0
    // lifted vslot_30: lookups miss → cells untouched; still returns true
    CHECK(vslot30_cell_boss_val_325e8b4() == -1);
    CHECK(vslot30_cell_stage_2dd9578() == -1);
    // lifted mid 0x255340: 4x26ea0 + 1x26b60
    CHECK(g_mid_reg == 4);
    CHECK(g_mid_b60 == 1);
    CHECK(g_move == 0);
    CHECK(g_nonnull == 0);
    CHECK(g_init_tb == 1);
    CHECK(r.base_ok);
    CHECK(r.remainder.trivial_ok);
    CHECK(r.remainder.registry.status == RegistryInit531d60Status::ok);
    CHECK(r.remainder.registry.entry_count == kInputTable551140Count);
    // + ensure 0xa800 alloc
    CHECK(heap.alloc_count - allocs_before >= 10);

    void* eb = loadp(r.tail.obj_98a90, 0x28);
    void* ec = loadp(r.tail.obj_98a90, 0x38);
    CHECK(eb != nullptr);
    CHECK(static_cast<std::uint8_t*>(ec) == static_cast<std::uint8_t*>(eb) + 0xa800);
    CHECK(r.tail.obj_40 != nullptr);
    CHECK(loadp(r.tail.obj_40, 0) == ctor40_derived_vtable_1598ae0());
    CHECK(ctor40_cell_2ee6778() == r.tail.obj_40);

    std::printf(
        "appmain_init_ctor_98a90_slice: MEASURED lifted_3be050_header+sub3be930+ensure3c2f70+"
        "buf80+null_list+tsc323d0+ctor40_1ab550+gates2552d4+vslot534130+mid255340+early+tail+"
        "base+trivial+registry ok nested=%p ensure=%p obj40=%p gate_byte=%u mid_reg=%d "
        "tsc=0x%llx entries=%u alloc_ops=%llu duration_us=%lld "
        "allocator=tip_AllocatorFront+FakeHeap "
        "vslot533100/26ea0/26b60/272d0/255f60/1fc80/stackchk/ba40_listeners/plt/"
        "ensure_migrate/nonnull=NOT_modelled pr5_heap_merge=GAP\n",
        r.ctor.nested_48, eb, r.tail.obj_40, static_cast<unsigned>(r.tail.gates.gate_byte),
        g_mid_reg, static_cast<unsigned long long>(r.ctor.tsc_value),
        r.remainder.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    std::puts("appmain_init_ctor_98a90_slice: all checks passed");
    return 0;
}
