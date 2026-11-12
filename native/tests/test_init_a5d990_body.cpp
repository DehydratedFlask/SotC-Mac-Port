// CPU-01-INIT-A5D990-BODY-01: fixture for the native 0xa5d990 composition model
// (core/init_a5d990_body) — the SoundSystemSndx category block of the engine init.
//
// Sections:
//   1. full run with no category names: 32 Mute + 32 Filter vars, the static cluster
//      (2 separators + 4 command items, all registered, all destroyed), the dead
//      "Unnamed Categories" gate never taken
//   2. per-index toggles: three enabled names build 6 items and destroy 6
//   3. fail-closed: state_missing, menu_parent_contract_missing, category_loop_failed,
//      separator_ctor_failed
//
// The model never executes an original RVA; every callee is the existing native model or a
// caller-owned hook. `cat_alloc` below is a malloc stand-in and every allocation it hands out is
// released at the end of the fixture, so the test is leak-clean under ASan.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/concmd_base.hpp"
#include "core/concmd_vars.hpp"
#include "core/debug_menu_item.hpp"
#include "core/engine_category_vars.hpp"
#include "core/init_a5d990_body.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

std::vector<void*> g_allocs;

void* cat_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    void* p = std::malloc(size);
    if (p != nullptr) g_allocs.push_back(p);
    return p;
}
bool cat_dealloc(void* ptr, std::uint32_t) {
    for (auto it = g_allocs.begin(); it != g_allocs.end(); ++it) {
        if (*it == ptr) { g_allocs.erase(it); break; }
    }
    std::free(ptr);
    return true;
}
void release_all() {
    for (void* p : g_allocs) std::free(p);
    g_allocs.clear();
}

class NameDealloc final : public SharedStrDeallocatorF590 {
public:
    void deallocate(const void* allocation, SharedStrDeallocationKindF590) noexcept override {
        cat_dealloc(const_cast<void*>(allocation), 0);
    }
};
NameDealloc g_name_dealloc;

bool no_grow(void*, SndxCategoryVectorId, SndxCategoryVectorSpan&) noexcept { return false; }

bool destroy_ok(void* host, DebugMenuItem&) noexcept {
    auto* count = static_cast<int*>(host);
    if (count != nullptr) ++*count;
    return true;
}

bool menu_insert_ok(void* parent, DebugMenuItem&) noexcept { return parent != nullptr; }

struct Fixture {
    std::uint8_t engine[0xa500] = {};
    std::vector<std::uint8_t> mute =
        std::vector<std::uint8_t>(kSndxCategoryCount * kSndxCategoryVarStride, 0);
    std::vector<std::uint8_t> filter =
        std::vector<std::uint8_t>(kSndxCategoryCount * kSndxCategoryVarStride, 0);
    const char* names[kSndxCategoryCount] = {};
    ConCmdRegistry registry{};
    StartupAtexitLog atexit{};
    DebugMenuRegistry menu{};
    SndxCategoryHooks categories{};
    InitA5d990Hooks hooks{};
    int destroys = 0;

    Fixture() {
        std::uint8_t* mbegin = mute.data();
        std::uint8_t* mcap = mbegin + mute.size();
        std::memcpy(engine + 0x8bd0, &mbegin, sizeof mbegin);
        std::memcpy(engine + 0x8bd8, &mbegin, sizeof mbegin);
        std::memcpy(engine + 0x8be0, &mcap, sizeof mcap);
        std::uint8_t* fbegin = filter.data();
        std::uint8_t* fcap = fbegin + filter.size();
        std::memcpy(engine + 0x72a8, &fbegin, sizeof fbegin);
        std::memcpy(engine + 0x72b0, &fbegin, sizeof fbegin);
        std::memcpy(engine + 0x72b8, &fcap, sizeof fcap);

        categories.name_alloc.alloc.allocate = &cat_alloc;
        categories.name_alloc.alloc.deallocate = &cat_dealloc;
        categories.name_dealloc = &g_name_dealloc;
        categories.grow = &no_grow;

        menu.parent.parent = &menu;
        menu.parent.insert = &menu_insert_ok;

        hooks.engine = engine;
        hooks.category_names = names;
        hooks.registry = &registry;
        hooks.categories = &categories;
        hooks.menu = &menu;
        hooks.atexit = &atexit;
        hooks.separator_alloc.alloc.allocate = &cat_alloc;
        hooks.separator_alloc.alloc.deallocate = &cat_dealloc;
        hooks.destroy_item = &destroy_ok;
        hooks.host = &destroys;
    }
};

void test_full_run_without_names() {
    Fixture f;
    const InitA5d990Result r = run_init_a5d990_body(f.hooks);
    CHECK(r.ok);
    CHECK(r.blocked == nullptr);
    CHECK(r.stage == SndxCategoriesStage::ok);
    CHECK(r.mute_built == kSndxCategoryCount);
    CHECK(r.filter_built == kSndxCategoryCount);
    CHECK(r.toggle_items == 0);
    CHECK(r.separators == 2);
    CHECK(r.commands == 4);
    CHECK(r.registered == static_cast<int>(kInitA5d990StaticItems));
    CHECK(r.destroys == static_cast<int>(kInitA5d990StaticItems));
    CHECK(f.destroys == static_cast<int>(kInitA5d990StaticItems));
    CHECK(r.dead_unnamed_block);   // the "Unnamed Categories" gate is never set

    // The two vectors now hold 32 slots each.
    const SndxCategoryVectorSpan mute =
        sndx_category_vectors_from_engine(f.engine, SndxCategoryVectorId::mute);
    const SndxCategoryVectorSpan filter =
        sndx_category_vectors_from_engine(f.engine, SndxCategoryVectorId::filter);
    CHECK(mute.var_count() == kSndxCategoryCount);
    CHECK(filter.var_count() == kSndxCategoryCount);
}

void test_enabled_names_build_toggles() {
    Fixture f;
    f.names[0] = "Ambient";
    f.names[13] = "Music";
    f.names[31] = "Voice";
    const InitA5d990Result r = run_init_a5d990_body(f.hooks);
    CHECK(r.ok);
    CHECK(r.mute_built == kSndxCategoryCount && r.filter_built == kSndxCategoryCount);
    // Two items (Mute + Filter) per enabled category, each registered and destroyed.
    CHECK(r.toggle_items == 6);
    CHECK(r.destroys == static_cast<int>(kInitA5d990StaticItems) + 6);
    CHECK(f.destroys == r.destroys);
}

void test_fail_closed() {
    {
        Fixture f;
        InitA5d990Hooks bad = f.hooks;
        bad.destroy_item = nullptr;
        const InitA5d990Result r = run_init_a5d990_body(bad);
        CHECK(!r.ok);
        CHECK(std::strcmp(r.blocked, "state_missing") == 0);
    }
    {
        Fixture f;
        f.menu.parent.insert = nullptr;
        const InitA5d990Result r = run_init_a5d990_body(f.hooks);
        CHECK(!r.ok);
        CHECK(std::strcmp(r.blocked, "menu_parent_contract_missing") == 0);
    }
    {
        Fixture f;
        // Empty span + a refusing grow hook: the loop fails at the first append.
        std::uint8_t* null_begin = nullptr;
        std::memcpy(f.engine + 0x8bd0, &null_begin, sizeof null_begin);
        std::memcpy(f.engine + 0x8bd8, &null_begin, sizeof null_begin);
        std::memcpy(f.engine + 0x8be0, &null_begin, sizeof null_begin);
        const InitA5d990Result r = run_init_a5d990_body(f.hooks);
        CHECK(!r.ok);
        CHECK(std::strcmp(r.blocked, "category_loop_failed") == 0);
        CHECK(r.stage == SndxCategoriesStage::grow_failed);
    }
    {
        Fixture f;
        f.hooks.separator_alloc.alloc.allocate = nullptr;
        const InitA5d990Result r = run_init_a5d990_body(f.hooks);
        CHECK(!r.ok);
        CHECK(std::strcmp(r.blocked, "separator_ctor_failed") == 0);
    }
}

}  // namespace

int main() {
    test_full_run_without_names();
    test_enabled_names_build_toggles();
    test_fail_closed();
    release_all();
    std::printf("OK test_init_a5d990_body\n");
    return 0;
}
