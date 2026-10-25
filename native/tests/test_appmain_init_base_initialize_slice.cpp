// Measured 0xf08 singleton → base Initialize 0x255130 → trivial → registry.
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_base_initialize_slice.hpp"
#include "core/base_initialize_255130.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/registry_init_531d60.hpp"
#include "core/singleton_f08_531d84.hpp"
#include "core/xchg_store_u32_357f0.hpp"

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
};

int g_prologue = 0;
int g_mid = 0;
int g_v40 = 0;

void hook_prologue(void*) { ++g_prologue; }
bool hook_v30_ok(void*) { return true; }
bool hook_v30_fail(void*) { return false; }
void hook_mid(void*) { ++g_mid; }
void hook_v40(void*, void*) { ++g_v40; }

}  // namespace

int main() {
    CHECK(kBaseInitialize255130Size == 79);
    CHECK(kAppVslot534130 == 0x534130);
    CHECK(kAppVslot533100 == 0x533100);
    CHECK(kXchgStore357f0Offset == 0x20);

    // Leaf 0x357f0 unit check.
    alignas(8) std::array<std::uint8_t, 0x40> leaf{};
    std::memset(leaf.data(), 0xCC, leaf.size());
    std::uint32_t old = 0x11223344;
    std::memcpy(leaf.data() + 0x20, &old, 4);
    CHECK(xchg_store_u32_357f0(leaf.data(), 0) == 0x11223344);
    std::uint32_t now = 0;
    std::memcpy(&now, leaf.data() + 0x20, 4);
    CHECK(now == 0);
    CHECK(leaf[0x1f] == 0xCC);
    CHECK(leaf[0x24] == 0xCC);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    InputTable551140 table{};
    init_input_table_551140(table);
    std::array<std::uint8_t, 0x60> app{};
    std::array<std::uint8_t, 0x60> setter{};
    alignas(8) std::array<std::uint8_t, 0x40> flag{};
    std::memset(flag.data(), 0xEE, flag.size());
    old = 0xABADC0DE;
    std::memcpy(flag.data() + 0x20, &old, 4);

    // Fail-visible: vslot_30 false → no mid/v40/357f0; remainder skipped.
    // Singleton still installs (OBS order: before base).
    g_prologue = g_mid = g_v40 = 0;
    singleton_f08_cell_2efc9e0() = nullptr;
    BaseInitialize255130Hooks fail_hooks{};
    fail_hooks.prologue_255180 = hook_prologue;
    fail_hooks.vslot_30 = hook_v30_fail;
    fail_hooks.mid_255340 = hook_mid;
    fail_hooks.vslot_40 = hook_v40;
    SingletonF08Hooks sh{};
    {
        const auto r = run_appmain_init_base_initialize_slice(
            table, front, app.data(), nullptr, fail_hooks, flag.data(), setter.data(), 1, 1, 2, 3,
            sh);
        CHECK(r.singleton.ok);
        CHECK(singleton_f08_cell_2efc9e0() == r.singleton.obj);
        CHECK(!r.base_ok);
        CHECK(r.base_fail_visible);
        CHECK(g_prologue == 1);
        CHECK(g_mid == 0);
        CHECK(g_v40 == 0);
        std::memcpy(&now, flag.data() + 0x20, 4);
        CHECK(now == 0xABADC0DE);  // 0x357f0 not reached
        CHECK(!r.remainder.trivial_ok);
        destroy_singleton_f08_at_2efc9e0(front, sh);
    }

    InputtableNamesFixture fix;
    if (!load_inputtable_names_fixture_default(fix)) {
        std::fprintf(stderr,
                     "appmain_init_base_initialize_slice: SKIP full path "
                     "(private I275 fixture missing)\n");
        std::puts("appmain_init_base_initialize_slice: fail-visible OK");
        return 0;
    }
    CHECK(apply_inputtable_names_fixture(table, fix));

    std::memset(setter.data(), 0xCC, setter.size());
    std::memcpy(flag.data() + 0x20, &old, 4);
    g_prologue = g_mid = g_v40 = 0;
    BaseInitialize255130Hooks ok_hooks{};
    ok_hooks.prologue_255180 = hook_prologue;
    ok_hooks.vslot_30 = hook_v30_ok;
    ok_hooks.mid_255340 = hook_mid;
    ok_hooks.vslot_40 = hook_v40;

    const std::uint64_t allocs_before = heap.alloc_count;
    const auto t0 = std::chrono::steady_clock::now();
    const auto r = run_appmain_init_base_initialize_slice(
        table, front, app.data(), reinterpret_cast<void*>(0x1), ok_hooks, flag.data(),
        setter.data(), 0, 0x11, 0x22, 0x33, sh);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    CHECK(r.singleton.ok);
    CHECK(singleton_f08_cell_2efc9e0() == r.singleton.obj);
    CHECK(r.base_ok);
    CHECK(!r.base_fail_visible);
    CHECK(g_prologue == 1);
    CHECK(g_mid == 1);
    CHECK(g_v40 == 1);
    CHECK(r.flag_prev == 0xABADC0DE);
    CHECK(r.flag_after == 0);
    CHECK(r.remainder.trivial_ok);
    CHECK(r.remainder.registry.status == RegistryInit531d60Status::ok);
    CHECK(r.remainder.registry.entry_count == kInputTable551140Count);
    CHECK(heap.alloc_count > allocs_before);

    std::printf(
        "appmain_init_base_initialize_slice: MEASURED singleton_f08+base+trivial+registry ok "
        "f08=%p flag_prev=%08x flag_after=%u entries=%u alloc_ops=%llu duration_us=%lld "
        "hooks=(prologue/mid/v40 modelled; 0x523bd0/0x525b40/0x255180/0x255340/0x534130/"
        "0x533100 bodies NOT modelled) allocator=tip_AllocatorFront+FakeHeap "
        "pr5_heap_merge=GAP\n",
        r.singleton.obj, r.flag_prev, r.flag_after, r.remainder.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    destroy_singleton_f08_at_2efc9e0(front, sh);
    base_init_flag_cell_2ea4718() = nullptr;
    std::puts("appmain_init_base_initialize_slice: all checks passed");
    return 0;
}
