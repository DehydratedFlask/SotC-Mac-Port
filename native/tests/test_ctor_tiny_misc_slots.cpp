// CPU-01-TINY-MISC-SLOTS-01: fixture for constructor slots 404, 406, 409, 414, 494, 517, 519, 535, 541, 553,
// 562 and 563. Every state is pre-filled with 0xA5 guard bytes so that a write outside the documented
// ranges, a missing write, a wrong order of effects (hook versus atexit record) or a wrong constant fails.
// The literal values below are an independent copy of the decoded ELF facts (tools/lift/
// check_ctor_tiny_misc_slots.py pins the same values against the pinned ELF).
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "core/concmd_base.hpp"
#include "core/ctor_tiny_misc_slots.hpp"
#include "core/store_tsc_323d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kFill = 0xA5;

template <class Arr>
void fill_arr(Arr& a) { a.fill(kFill); }

template <class Arr>
bool range_is(const Arr& a, std::size_t lo, std::size_t hi, std::uint8_t v) {
    for (std::size_t i = lo; i < hi; ++i)
        if (a[i] != v) return false;
    return true;
}

std::uint64_t load_u64(const std::uint8_t* p) { std::uint64_t v; std::memcpy(&v, p, 8); return v; }

// ---- hooks with observable ordering -------------------------------------------------------------
StartupAtexitLog* g_log = nullptr;
std::size_t g_log_size_at_mutex = 99;
std::size_t g_mutex_calls = 0;
bool g_mutex_result = true;

bool mutex_hook(void* obj) noexcept {
    ++g_mutex_calls;
    g_log_size_at_mutex = g_log->size();
    const std::uint64_t handle = 0x1122334455667788ull;
    std::memcpy(obj, &handle, 8);
    return g_mutex_result;
}

std::size_t g_log_at_c0 = 99, g_log_at_c1 = 99;
bool g_c0_result = true, g_c1_result = true;
void* g_c0_obj = nullptr;
void* g_c1_obj = nullptr;
bool ctor0(void* obj) noexcept {
    g_log_at_c0 = g_log->size();
    g_c0_obj = obj;
    std::memset(obj, 0x11, 0x20);
    return g_c0_result;
}
bool ctor1(void* obj) noexcept {
    g_log_at_c1 = g_log->size();
    g_c1_obj = obj;
    std::memset(obj, 0x22, 0x20);
    return g_c1_result;
}

alignas(16) std::uint8_t g_alloc_block[0x40];
std::size_t g_alloc_calls = 0, g_alloc_size = 0;
void* counting_alloc(std::size_t size) noexcept {
    ++g_alloc_calls;
    g_alloc_size = size;
    std::memset(g_alloc_block, kFill, sizeof(g_alloc_block));
    return g_alloc_block;
}
void* null_alloc(std::size_t) noexcept { return nullptr; }

std::uint64_t timebase_hook() { return 12345; }

}  // namespace

int main() {
    // ---- constants: independent copies -----------------------------------------------------------
    CHECK(kSlot404Rva == 0x5e7c10 && kSlot404Size == 19 && kSlot404BufferRva == 0x30061a0 && kSlot404BufferSize == 0x51f8);
    CHECK(kSlot406Rva == 0x5dbe10 && kSlot406Size == 92 && kSlot406Obj0Rva == 0x30015c8 && kSlot406Obj1Rva == 0x30015f0);
    CHECK(kSlot406InitRva == 0x8bd90 && kSlot406DtorRva == 0x8bdb0 && kSlot406ConstRva == 0xf9ebd0);
    CHECK(kSlot409Rva == 0x5b50f0 && kSlot409Size == 89 && kSlot409ObjRva == 0x2fda208 && kSlot409DtorRva == 0x5b3d10);
    CHECK(kSlot409AllocRva == 0x17070 && kSlot409NodeSize == 0x40);
    CHECK(kSlot414Rva == 0x57b9b0 && kSlot414Size == 72 && kSlot414ObjRva == 0x2f35898 &&
          kSlot414MutexInitRva == 0x3b310 && kSlot414DtorRva == 0x3b380);
    CHECK(kSlot494Rva == 0x26f7c0 && kSlot494Size == 84 && kSlot494Obj0Rva == 0x2eef038 && kSlot494Obj1Rva == 0x2eef058);
    CHECK(kSlot494Ctor0Rva == 0x26e340 && kSlot494Ctor1Rva == 0x26e770 && kSlot494Dtor0Rva == 0x269590 &&
          kSlot494Dtor1Rva == 0x269610);
    CHECK(kSlot517Rva == 0x1aa510 && kSlot517Size == 56 && kSlot517CellRva == 0x2ee6720 && kSlot517RecordRva == 0x2ee6728 &&
          kSlot517DtorRva == 0x1aa070 && kSlot517StampCalleeRva == 0x326a0);
    CHECK(kSlot519Rva == 0x19d2d0 && kSlot519Size == 83 && kSlot519ObjRva == 0x2ee4548 && kSlot519BufferRva == 0x2ee4568 &&
          kSlot519CellRva == 0x2ee6570 && kSlot519DtorRva == 0x19a7b0 && kSlot519BufferSize == 0x2000);
    CHECK(kSlot535Rva == 0x138e20 && kSlot535Size == 96 && kSlot535MutexRva == 0x2ee14f0 && kSlot535ObjRva == 0x2ee14f8 &&
          kSlot535MutexDtorRva == 0x3b380 && kSlot535ObjDtorRva == 0x138370);
    CHECK(kSlot541Rva == 0xfd210 && kSlot541Size == 52 && kSlot541CellRva == 0x2edef68 && kSlot541Name0Rva == 0xfa503b &&
          kSlot541Name1Rva == 0xfa5040);
    CHECK(kSlot553Rva == 0x8bd40 && kSlot553Size == 72 && kSlot553ObjRva == 0x2ec0120 && kSlot553SentinelOffset == 0x28 &&
          kSlot553ExternalRva == 0x2ed1f48 && kSlot553DtorRva == 0x8a620);
    CHECK(kSlot562Rva == 0x36100 && kSlot562Size == 50 && kSlot562CellRva == 0x2ea4718 && kSlot562FlagRva == 0x2ea4720 &&
          kSlot562DtorRva == 0x33280 && kSlot562FlagValue == 6);
    CHECK(kSlot563Rva == 0x32c70 && kSlot563Size == 72 && kSlot563StampRva == 0x2ea4710);

    // ---- 404: memset 0x51f8 ----------------------------------------------------------------------
    {
        struct Guarded { std::array<std::uint8_t, 64> pre; Slot404Buffer b; std::array<std::uint8_t, 64> post; };
        auto g = std::make_unique<Guarded>();
        fill_arr(g->pre); fill_arr(g->post); fill_arr(g->b.bytes);
        init_slot404_5e7c10(g->b);
        CHECK(range_is(g->b.bytes, 0, 0x51f8, 0));
        CHECK(range_is(g->pre, 0, 64, kFill) && range_is(g->post, 0, 64, kFill));
        CHECK(g->b.bytes.size() == 0x51f8);
    }

    // ---- 406: two objects, constant, atexit order ------------------------------------------------
    {
        auto s = std::make_unique<Slot406State>();
        for (auto& o : s->obj) fill_arr(o);
        StartupAtexitLog log;
        const TinyMiscResult r = init_slot406_5dbe10(*s, log);
        CHECK(r.ok && r.blocked == nullptr && log.size() == 2);
        const std::uint8_t konst[16] = {0, 0, 0, 0, 0x1d, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff};
        for (std::size_t k = 0; k < 2; ++k) {
            CHECK(range_is(s->obj[k], 0, 0x10, kFill));               // +0..+0xf never written
            CHECK(range_is(s->obj[k], 0x10, 0x18, 0));                // qword 0
            CHECK(std::memcmp(s->obj[k].data() + 0x18, konst, 16) == 0);
            CHECK(log.at(k).dtor_rva == 0x8bdb0 && log.at(k).object == s->obj[k].data());
        }
    }

    // ---- 409: sentinel node via allocator hook ---------------------------------------------------
    {
        g_alloc_calls = 0; g_alloc_size = 0;
        auto s = std::make_unique<Slot409State>();
        s->obj.unwritten0 = 0x1122;
        s->obj.head = reinterpret_cast<void*>(0xdead);
        s->obj.size = 77;
        StartupAtexitLog log;
        Slot409Hooks hooks;
        hooks.alloc_17070 = counting_alloc;
        const TinyMiscResult r = init_slot409_5b50f0(*s, hooks, log);
        CHECK(r.ok && g_alloc_calls == 1 && g_alloc_size == 0x40);
        CHECK(s->node == g_alloc_block && s->obj.head == g_alloc_block && s->obj.size == 0 && s->obj.unwritten0 == 0x1122);
        const std::uintptr_t self = reinterpret_cast<std::uintptr_t>(g_alloc_block);
        CHECK(load_u64(g_alloc_block + 0) == self && load_u64(g_alloc_block + 8) == self && load_u64(g_alloc_block + 16) == self);
        CHECK(g_alloc_block[0x18] == 1 && g_alloc_block[0x19] == 1);
        for (std::size_t i = 0x1a; i < 0x40; ++i) CHECK(g_alloc_block[i] == kFill);
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x5b3d10 && log.at(0).object == &s->obj);

        // no hook: fail closed before any write
        auto t = std::make_unique<Slot409State>();
        t->obj.unwritten0 = 9; t->obj.head = reinterpret_cast<void*>(0xbeef); t->obj.size = 5;
        StartupAtexitLog log2;
        const TinyMiscResult nr = init_slot409_5b50f0(*t, Slot409Hooks{}, log2);
        CHECK(!nr.ok && std::strcmp(nr.blocked, "alloc_hook") == 0 && log2.size() == 0);
        CHECK(t->obj.head == reinterpret_cast<void*>(0xbeef) && t->obj.size == 5 && t->node == nullptr);

        // allocator returns null: zero stores already happened (original order), no node, no atexit
        Slot409Hooks nh;
        nh.alloc_17070 = null_alloc;
        const TinyMiscResult fr = init_slot409_5b50f0(*t, nh, log2);
        CHECK(!fr.ok && std::strcmp(fr.blocked, "alloc") == 0 && log2.size() == 0);
        CHECK(t->obj.head == nullptr && t->obj.size == 0 && t->obj.unwritten0 == 9 && t->node == nullptr);

        // default host allocator
        auto d = std::make_unique<Slot409State>();
        StartupAtexitLog log3;
        const TinyMiscResult dr = init_slot409_5b50f0(*d, default_slot409_hooks(), log3);
        CHECK(dr.ok && d->node != nullptr && d->obj.head == d->node);
        std::free(d->node);
    }

    // ---- 414: mutex hook, atexit, zero tail ------------------------------------------------------
    {
        auto s = std::make_unique<Slot414State>();
        fill_arr(s->obj);
        StartupAtexitLog log;
        g_log = &log; g_mutex_calls = 0; g_log_size_at_mutex = 99; g_mutex_result = true;
        Slot414Hooks hooks;
        hooks.mutex_init_3b310 = mutex_hook;
        CHECK(init_slot414_57b9b0(*s, hooks, log).ok);
        CHECK(g_mutex_calls == 1 && g_log_size_at_mutex == 0);        // hook runs BEFORE the atexit record
        CHECK(load_u64(s->obj.data()) == 0x1122334455667788ull);
        CHECK(range_is(s->obj, 8, 12, 0) && range_is(s->obj, 12, 16, kFill) && range_is(s->obj, 16, 32, 0));
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x3b380 && log.at(0).object == s->obj.data());

        auto t = std::make_unique<Slot414State>();
        fill_arr(t->obj);
        StartupAtexitLog log2;
        const TinyMiscResult n = init_slot414_57b9b0(*t, Slot414Hooks{}, log2);
        CHECK(!n.ok && std::strcmp(n.blocked, "mutex_hook") == 0 && log2.size() == 0 && range_is(t->obj, 0, 32, kFill));
        g_log = &log2; g_mutex_result = false;
        const TinyMiscResult f = init_slot414_57b9b0(*t, hooks, log2);
        CHECK(!f.ok && std::strcmp(f.blocked, "mutex_init") == 0 && log2.size() == 0);
        CHECK(range_is(t->obj, 8, 32, kFill));                         // nothing after the failed hook
    }

    // ---- 494: two hook-built objects with atexit between them ------------------------------------
    {
        auto s = std::make_unique<Slot494State>();
        for (auto& o : s->obj) fill_arr(o);
        StartupAtexitLog log;
        g_log = &log; g_c0_result = g_c1_result = true; g_log_at_c0 = g_log_at_c1 = 99;
        Slot494Hooks hooks;
        hooks.ctor_26e340 = ctor0;
        hooks.ctor_26e770 = ctor1;
        CHECK(init_slot494_26f7c0(*s, hooks, log).ok);
        CHECK(g_log_at_c0 == 0 && g_log_at_c1 == 1);                   // c0, atexit0, c1, atexit1
        CHECK(g_c0_obj == s->obj[0].data() && g_c1_obj == s->obj[1].data());
        CHECK(range_is(s->obj[0], 0, 0x20, 0x11) && range_is(s->obj[1], 0, 0x20, 0x22));
        CHECK(log.size() == 2 && log.at(0).dtor_rva == 0x269590 && log.at(0).object == s->obj[0].data());
        CHECK(log.at(1).dtor_rva == 0x269610 && log.at(1).object == s->obj[1].data());

        auto t = std::make_unique<Slot494State>();
        for (auto& o : t->obj) fill_arr(o);
        StartupAtexitLog l2;
        g_log = &l2;
        Slot494Hooks none;
        CHECK(std::strcmp(init_slot494_26f7c0(*t, none, l2).blocked, "ctor_26e340_hook") == 0);
        none.ctor_26e340 = ctor0;
        CHECK(std::strcmp(init_slot494_26f7c0(*t, none, l2).blocked, "ctor_26e770_hook") == 0);
        CHECK(l2.size() == 0 && range_is(t->obj[0], 0, 0x20, kFill) && range_is(t->obj[1], 0, 0x20, kFill));
        g_c0_result = false;
        const TinyMiscResult f0 = init_slot494_26f7c0(*t, hooks, l2);
        CHECK(!f0.ok && std::strcmp(f0.blocked, "ctor_26e340") == 0 && l2.size() == 0 && range_is(t->obj[1], 0, 0x20, kFill));
        g_c0_result = true; g_c1_result = false;
        const TinyMiscResult f1 = init_slot494_26f7c0(*t, hooks, l2);
        CHECK(!f1.ok && std::strcmp(f1.blocked, "ctor_26e770") == 0 && l2.size() == 1);
    }

    // ---- 517: cell + atexit, then TSC record ------------------------------------------------------
    {
        store_tsc_guard_cell_2ea4700() = 0;
        store_tsc_global_cell_2ea4708() = 0;
        auto s = std::make_unique<Slot517State>();
        s->cell = 0xdead;
        s->record.flag = 0x55; s->record.tsc = 0; s->record.zero = 0x77;
        StartupAtexitLog log;
        StoreTsc323d0Hooks th;
        th.init_timebase_f8f860 = timebase_hook;
        CHECK(init_slot517_1aa510(*s, log, th).ok);
        CHECK(s->cell == 0 && s->record.flag == 0 && s->record.zero == 0 && s->record.tsc != 0);
        CHECK(store_tsc_guard_cell_2ea4700() == 12345 && store_tsc_global_cell_2ea4708() != 0);
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x1aa070 && log.at(0).object == &s->cell);
        // guard already set: the timebase hook must not run again, the stamp is refreshed
        store_tsc_guard_cell_2ea4700() = 999;
        const std::uint64_t first = s->record.tsc;
        s->record.tsc = 0;
        CHECK(init_slot517_1aa510(*s, log, th).ok);
        CHECK(store_tsc_guard_cell_2ea4700() == 999 && s->record.tsc >= first && log.size() == 2);
        store_tsc_guard_cell_2ea4700() = 0;
        store_tsc_global_cell_2ea4708() = 0;
    }

    // ---- 519: 24 B zero, atexit, memset 0x2000, xchg ----------------------------------------------
    {
        auto s = std::make_unique<Slot519State>();
        fill_arr(s->obj); fill_arr(s->buffer);
        s->cell.store(0x1234);
        StartupAtexitLog log;
        CHECK(init_slot519_19d2d0(*s, log).ok);
        CHECK(range_is(s->obj, 0, 0x18, 0) && range_is(s->buffer, 0, 0x2000, 0) && s->cell.load() == 0);
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x19a7b0 && log.at(0).object == s->obj.data());
    }

    // ---- 535: mutex hook, atexit, zero [+8,+0x20), atexit -----------------------------------------
    {
        auto s = std::make_unique<Slot535State>();
        fill_arr(s->mutex); fill_arr(s->obj);
        StartupAtexitLog log;
        g_log = &log; g_mutex_calls = 0; g_log_size_at_mutex = 99; g_mutex_result = true;
        Slot414Hooks hooks;
        hooks.mutex_init_3b310 = mutex_hook;
        CHECK(init_slot535_138e20(*s, hooks, log).ok);
        CHECK(g_mutex_calls == 1 && g_log_size_at_mutex == 0);
        CHECK(load_u64(s->mutex.data()) == 0x1122334455667788ull);
        CHECK(range_is(s->obj, 0, 8, kFill) && range_is(s->obj, 8, 0x20, 0));
        CHECK(log.size() == 2 && log.at(0).dtor_rva == 0x3b380 && log.at(0).object == s->mutex.data());
        CHECK(log.at(1).dtor_rva == 0x138370 && log.at(1).object == s->obj.data());

        auto t = std::make_unique<Slot535State>();
        fill_arr(t->mutex); fill_arr(t->obj);
        StartupAtexitLog l2;
        CHECK(std::strcmp(init_slot535_138e20(*t, Slot414Hooks{}, l2).blocked, "mutex_hook") == 0);
        g_log = &l2; g_mutex_result = false;
        CHECK(std::strcmp(init_slot535_138e20(*t, hooks, l2).blocked, "mutex_init") == 0);
        CHECK(l2.size() == 0 && range_is(t->obj, 0, 0x20, kFill));
    }

    // ---- 541: two seeded path hashes ---------------------------------------------------------------
    {
        Slot541State s;
        s.hash0 = s.hash1 = 0xFFFFFFFFu;
        init_slot541_fd210(s);
        CHECK(s.hash0 == 0x167de847u && s.hash1 == 0xd9e6aae6u);        // FNV-1a("WIND"), FNV-1a("MESH_INSTANCE")
        CHECK(std::strcmp(kSlot541Name0, "wind") == 0 && std::strcmp(kSlot541Name1, "mesh_instance") == 0);
    }

    // ---- 553: self-referential sentinel struct -----------------------------------------------------
    {
        auto s = std::make_unique<Slot553State>();
        fill_arr(s->obj);
        StartupAtexitLog log;
        CHECK(init_slot553_8bd40(*s, log).ok);
        const std::uint64_t sent = reinterpret_cast<std::uintptr_t>(s->obj.data() + 0x28);
        CHECK(load_u64(s->obj.data() + 0x00) == sent && load_u64(s->obj.data() + 0x08) == sent);
        CHECK(load_u64(s->obj.data() + 0x10) == reinterpret_cast<std::uintptr_t>(s->external.data()));
        CHECK(range_is(s->obj, 0x18, 0x20, kFill));
        CHECK(load_u64(s->obj.data() + 0x20) == sent);
        CHECK(range_is(s->obj, 0x28, 0x30, kFill));
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x8a620 && log.at(0).object == s->obj.data());
    }

    // ---- 562: cell + atexit + flag exchange ---------------------------------------------------------
    {
        auto s = std::make_unique<Slot562State>();
        s->cell = 0xabcdef;
        s->flag.store(0x55);
        StartupAtexitLog log;
        CHECK(init_slot562_36100(*s, log).ok);
        CHECK(s->cell == 0 && s->flag.load() == 6);
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x33280 && log.at(0).object == &s->cell);
    }

    // ---- 563: TSC store (shares 0x2ea4700/0x2ea4708 with 0x323d0) -----------------------------------
    {
        store_tsc_guard_cell_2ea4700() = 0;
        store_tsc_global_cell_2ea4708() = 0;
        Slot563Stamp st;
        StoreTsc323d0Hooks th;
        th.init_timebase_f8f860 = timebase_hook;
        init_slot563_32c70(st, th);
        CHECK(st.tsc != 0 && store_tsc_guard_cell_2ea4700() == 12345 && store_tsc_global_cell_2ea4708() != 0);
        store_tsc_guard_cell_2ea4700() = 0;
        store_tsc_global_cell_2ea4708() = 0;
        // an already-initialised guard skips the hook and the base stamp
        store_tsc_guard_cell_2ea4700() = 7;
        store_tsc_global_cell_2ea4708() = 0;
        init_slot563_32c70(st, th);
        CHECK(store_tsc_guard_cell_2ea4700() == 7 && store_tsc_global_cell_2ea4708() == 0);
        // no hook: sentinel 1
        store_tsc_guard_cell_2ea4700() = 0;
        init_slot563_32c70(st, StoreTsc323d0Hooks{});
        CHECK(store_tsc_guard_cell_2ea4700() == 1);
        store_tsc_guard_cell_2ea4700() = 0;
        store_tsc_global_cell_2ea4708() = 0;
    }

    std::puts("OK test_ctor_tiny_misc_slots: 404 406 409 414 494 517 519 535 541 553 562 563");
    return 0;
}
