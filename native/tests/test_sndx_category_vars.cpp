// CPU-01-SLOT306-CATEGORY-VARS-01: fixture for the native sndxCategory_* model
// (core/engine_category_vars) — the 0xa5d990 loop of the engine init: 32 Mute + 32 Filter
// bool vars at engine+0x8bd0/+0x72a8, stride 0xc8, formatted names, no per-object atexit,
// conditional per-index menu toggles. Static model; no original code runs here. Sections:
//   1. name formatting (both ids, boundary indices)
//   2. engine-vector extraction at the pinned offsets
//   3. full build: counts, pending-list order (interleaved), slot shapes, owned names with
//      refcount 1, no atexit records, menu hook for enabled names only
//   4. fail-closed: hooks_missing, grow failure (first and later), name allocation failure
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/concmd_base.hpp"
#include "core/concmd_vars.hpp"
#include "core/engine_category_vars.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

// ---- default-stand-in hooks (malloc/free; documented stand-ins, NOT the engine allocator) ---------

void* cat_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    return std::malloc(size);
}
bool cat_dealloc(void* ptr, std::uint32_t) {
    std::free(ptr);
    return true;
}

class CatNameDealloc final : public SharedStrDeallocatorF590 {
public:
    std::size_t data_frees = 0;
    std::size_t control_frees = 0;
    void deallocate(const void* allocation, SharedStrDeallocationKindF590 kind) noexcept override {
        if (kind == SharedStrDeallocationKindF590::data) {
            ++data_frees;
        } else {
            ++control_frees;
        }
        std::free(const_cast<void*>(allocation));
    }
};

// Stable stand-in: one allocation of the full 32 slots (no relocation — pointer identity in the
// menu-hook assertions below depends on it). The moving-growth path is covered separately in
// test_relocation.
bool cat_grow(void* /*host*/, SndxCategoryVectorId /*id*/, SndxCategoryVectorSpan& span) {
    if (span.capacity() >= kSndxCategoryCount) return false;
    void* fresh = std::realloc(span.begin, kSndxCategoryCount * kSndxCategoryVarStride);
    if (fresh == nullptr) return false;
    const std::size_t used = span.begin != nullptr && span.end != nullptr
                                 ? static_cast<std::size_t>(static_cast<char*>(span.end) -
                                                            static_cast<char*>(span.begin))
                                 : 0;
    span.begin = fresh;
    span.end = static_cast<char*>(fresh) + used;
    span.cap = static_cast<char*>(fresh) + kSndxCategoryCount * kSndxCategoryVarStride;
    return true;
}

// Doubling stand-in (moves the buffer): exercises the module's relocation fixup.
bool doubling_grow(void* /*host*/, SndxCategoryVectorId /*id*/, SndxCategoryVectorSpan& span) {
    const std::size_t old_cap = span.capacity();
    const std::size_t new_cap = old_cap == 0 ? 4 : old_cap * 2;
    const std::size_t used = span.begin != nullptr && span.end != nullptr
                                 ? static_cast<std::size_t>(static_cast<char*>(span.end) -
                                                            static_cast<char*>(span.begin))
                                 : 0;
    void* fresh = std::realloc(span.begin, new_cap * kSndxCategoryVarStride);
    if (fresh == nullptr) return false;
    span.begin = fresh;   // realloc may move the buffer: re-base end (used slots preserved)
    span.end = static_cast<char*>(fresh) + used;
    span.cap = static_cast<char*>(fresh) + new_cap * kSndxCategoryVarStride;
    return true;
}

struct MenuNote {
    std::size_t index = 0;
    const char* name = nullptr;
    void* mute_var = nullptr;
    void* filter_var = nullptr;
};
MenuNote g_menu[8];
std::size_t g_menu_count = 0;
void menu_hook(void*, std::size_t index, const char* name, void* mute_var, void* filter_var) {
    g_menu[g_menu_count % 8] = MenuNote{index, name, mute_var, filter_var};
    ++g_menu_count;
}

// failure-injecting hooks
std::size_t g_grow_allow = 1000;
bool failing_grow(void* host, SndxCategoryVectorId id, SndxCategoryVectorSpan& span) {
    if (g_grow_allow == 0) return false;
    --g_grow_allow;
    return doubling_grow(host, id, span);
}
std::size_t g_alloc_budget = 1000;
void* budget_alloc(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    if (g_alloc_budget == 0) return nullptr;
    --g_alloc_budget;
    return cat_alloc(size, tag, align, flags);
}

SndxCategoryHooks make_hooks(CatNameDealloc& dealloc) {
    SndxCategoryHooks hooks;
    hooks.name_alloc.alloc.allocate = &cat_alloc;
    hooks.name_alloc.alloc.deallocate = &cat_dealloc;
    hooks.name_dealloc = &dealloc;
    hooks.grow = &cat_grow;
    hooks.menu_toggle = &menu_hook;
    return hooks;
}

const ConCmdBase* base_of_pending(const ConCmdListNode* node) {
    return reinterpret_cast<const ConCmdBase*>(reinterpret_cast<const char*>(node) -
                                               offsetof(ConCmdBase, pending));
}

void check_slot(const ConCmdBoolVar& var, SndxCategoryVectorId id, std::size_t i) {
    char text[kSndxCategoryNameCapacity];
    CHECK(sndx_format_category_name(text, id, i));
    CHECK(var.base.vptr == kBoolVarFinalVptr);
    CHECK(var.base.flags == 1);
    CHECK(var.base.registered != 0);
    CHECK(var.base.name.size == static_cast<std::int64_t>(std::strlen(text)));
    CHECK(std::memcmp(var.base.name.data, text, std::strlen(text)) == 0);
    CHECK(var.base.name.ctl != nullptr);   // the formatted name is OWNED
    CHECK(std::atomic_ref<std::int64_t>(*var.base.name.ctl).load() == 1);  // object's only ref
    CHECK(var.base.description.size == 0);  // the empty literal view
    CHECK(var.value_ref == &var.value);
    CHECK(var.default_value == 0 && var.value == 0);
    CHECK(var.fn_payload == 0 && var.fn_vptr == kSndxCategoryVarFnVtbl && var.fn_aux == 0);
}

void test_format_names() {
    char text[kSndxCategoryNameCapacity];
    CHECK(sndx_format_category_name(text, SndxCategoryVectorId::mute, 0));
    CHECK(std::strcmp(text, "sndxCategory_Mute_0") == 0);
    CHECK(sndx_format_category_name(text, SndxCategoryVectorId::filter, 0));
    CHECK(std::strcmp(text, "sndxCategory_Filter_0") == 0);
    CHECK(sndx_format_category_name(text, SndxCategoryVectorId::mute, 31));
    CHECK(std::strcmp(text, "sndxCategory_Mute_31") == 0);
    CHECK(sndx_format_category_name(text, SndxCategoryVectorId::filter, 31));
    CHECK(std::strcmp(text, "sndxCategory_Filter_31") == 0);
}

void test_vectors_from_engine() {
    std::uint8_t engine[0x20000] = {};
    void* mute_begin = engine + 0x100;
    void* mute_end = engine + 0x100 + 3 * kSndxCategoryVarStride;
    void* mute_cap = engine + 0x100 + 8 * kSndxCategoryVarStride;
    void* filter_begin = engine + 0x8000;
    void* filter_end = engine + 0x8000 + 5 * kSndxCategoryVarStride;
    void* filter_cap = engine + 0x8000 + 8 * kSndxCategoryVarStride;
    std::memcpy(engine + kSndxMuteVectorOffset, &mute_begin, 8);
    std::memcpy(engine + kSndxMuteVectorOffset + 8, &mute_end, 8);
    std::memcpy(engine + kSndxMuteVectorCapOffset, &mute_cap, 8);
    std::memcpy(engine + kSndxFilterVectorOffset, &filter_begin, 8);
    std::memcpy(engine + kSndxFilterVectorOffset + 8, &filter_end, 8);
    std::memcpy(engine + kSndxFilterVectorCapOffset, &filter_cap, 8);
    const SndxCategoryVectorSpan mute = sndx_category_vectors_from_engine(engine, SndxCategoryVectorId::mute);
    const SndxCategoryVectorSpan filter = sndx_category_vectors_from_engine(engine, SndxCategoryVectorId::filter);
    CHECK(mute.begin == mute_begin && mute.end == mute_end && mute.cap == mute_cap);
    CHECK(mute.var_count() == 3 && mute.capacity() == 8);
    CHECK(filter.begin == filter_begin && filter.end == filter_end && filter.cap == filter_cap);
    CHECK(filter.var_count() == 5 && filter.capacity() == 8);
    const SndxCategoryVectorSpan none = sndx_category_vectors_from_engine(nullptr, SndxCategoryVectorId::mute);
    CHECK(none.begin == nullptr && none.var_count() == 0);
}

void test_full_build() {
    CatNameDealloc dealloc;
    SndxCategoryHooks hooks = make_hooks(dealloc);
    ConCmdRegistry registry{};

    const char* names[kSndxCategoryCount] = {};
    names[0] = "Ambient";
    names[13] = "Music";
    names[31] = "Voice";
    g_menu_count = 0;
    SndxCategoryVectorSpan mute{};   // empty: the first append grows
    SndxCategoryVectorSpan filter{};

    const SndxCategoriesResult r =
        build_sndx_category_vars(mute, filter, names, registry, hooks);
    CHECK(r.ok);
    CHECK(r.stage == SndxCategoriesStage::ok);
    CHECK(r.mute_built == kSndxCategoryCount && r.filter_built == kSndxCategoryCount);
    CHECK(r.grow_calls >= 1);
    CHECK(r.menu_toggles == 3);
    CHECK(mute.var_count() == kSndxCategoryCount && filter.var_count() == kSndxCategoryCount);

    // slot shapes
    for (std::size_t i = 0; i < kSndxCategoryCount; ++i) {
        auto* const m = reinterpret_cast<const ConCmdBoolVar*>(
            static_cast<char*>(mute.begin) + i * kSndxCategoryVarStride);
        auto* const f = reinterpret_cast<const ConCmdBoolVar*>(
            static_cast<char*>(filter.begin) + i * kSndxCategoryVarStride);
        check_slot(*m, SndxCategoryVectorId::mute, i);
        check_slot(*f, SndxCategoryVectorId::filter, i);
    }

    // pending list: 64 entries, interleaved exactly as the loop builds them
    CHECK(registry.pending.size() == 2 * kSndxCategoryCount);
    const ConCmdListNode* node = registry.pending.first();
    for (std::size_t i = 0; i < kSndxCategoryCount; ++i) {
        const void* want_mute = static_cast<char*>(mute.begin) + i * kSndxCategoryVarStride;
        const void* want_filter = static_cast<char*>(filter.begin) + i * kSndxCategoryVarStride;
        CHECK(base_of_pending(node) == want_mute);
        node = node->next;
        CHECK(base_of_pending(node) == want_filter);
        node = node->next;
    }
    CHECK(node == &registry.pending.head);

    // vector-owned: NO atexit records
    CHECK(registry.atexit_log.size() == 0);

    // menu hook fired for the enabled names only, with the fresh vars
    CHECK(g_menu_count == 3);
    CHECK(g_menu[0].index == 0 && std::strcmp(g_menu[0].name, "Ambient") == 0);
    CHECK(g_menu[1].index == 13 && std::strcmp(g_menu[1].name, "Music") == 0);
    CHECK(g_menu[2].index == 31 && std::strcmp(g_menu[2].name, "Voice") == 0);
    CHECK(g_menu[0].mute_var == mute.begin);
    CHECK(g_menu[0].filter_var == filter.begin);
    CHECK(g_menu[2].mute_var == static_cast<char*>(mute.end) - kSndxCategoryVarStride);
    CHECK(g_menu[2].filter_var == static_cast<char*>(filter.end) - kSndxCategoryVarStride);

    // no names were freed while the objects hold them (only the stack refs were released)
    CHECK(dealloc.data_frees == 0 && dealloc.control_frees == 0);

    // a null names table builds the same 64 vars without any toggle
    ConCmdRegistry second{};
    SndxCategoryVectorSpan m2{};
    SndxCategoryVectorSpan f2{};
    g_menu_count = 0;
    const SndxCategoriesResult r2 = build_sndx_category_vars(m2, f2, nullptr, second, hooks);
    CHECK(r2.ok && r2.menu_toggles == 0 && g_menu_count == 0);
    CHECK(second.pending.size() == 2 * kSndxCategoryCount);

    std::free(mute.begin);
    std::free(filter.begin);
    std::free(m2.begin);
    std::free(f2.begin);
}

void test_fail_closed() {
    CatNameDealloc dealloc;
    SndxCategoryHooks hooks = make_hooks(dealloc);

    // incomplete hooks
    SndxCategoryHooks partial = hooks;
    partial.grow = nullptr;
    SndxCategoryVectorSpan m{}, f{};
    ConCmdRegistry registry{};
    SndxCategoriesResult r = build_sndx_category_vars(m, f, nullptr, registry, partial);
    CHECK(!r.ok && r.stage == SndxCategoriesStage::hooks_missing);

    // grow fails on the very first append
    partial = hooks;
    partial.grow = &failing_grow;
    g_grow_allow = 0;
    m = SndxCategoryVectorSpan{};
    f = SndxCategoryVectorSpan{};
    r = build_sndx_category_vars(m, f, nullptr, registry, partial);
    CHECK(!r.ok && r.stage == SndxCategoriesStage::grow_failed && r.failed_index == 0);
    CHECK(r.mute_built == 0 && r.filter_built == 0 && r.grow_calls == 0);

    // grow fails later: the partial run is named, never a success. Budget 3 grows covers
    // mute@0 (cap 0->4), filter@0 (cap 0->4) and mute@4 (cap 4->8); the filter append at
    // index 4 then refuses to grow, so exactly 5 mute + 4 filter slots completed.
    g_grow_allow = 3;
    m = SndxCategoryVectorSpan{};
    f = SndxCategoryVectorSpan{};
    registry = ConCmdRegistry{};
    r = build_sndx_category_vars(m, f, nullptr, registry, partial);
    CHECK(!r.ok && r.stage == SndxCategoriesStage::grow_failed);
    CHECK(r.mute_built == 5 && r.filter_built == 4);
    CHECK(r.grow_calls == 3 && r.failed_index == 4);
    CHECK(registry.pending.size() == 9);
    std::free(m.begin);
    std::free(f.begin);

    // name allocation failure at the first name
    partial = hooks;
    partial.name_alloc.alloc.allocate = &budget_alloc;
    g_alloc_budget = 0;
    m = SndxCategoryVectorSpan{};
    f = SndxCategoryVectorSpan{};
    registry = ConCmdRegistry{};
    r = build_sndx_category_vars(m, f, nullptr, registry, partial);
    CHECK(!r.ok && r.stage == SndxCategoriesStage::name_alloc_failed && r.failed_index == 0);
    CHECK(r.mute_built == 0 && r.filter_built == 0);
}


// The moving-growth path: every relocation must leave value_ref pointing at the slot's own +0xc0
// and the pending list intact, in order, with no stale pointers (ASan would catch any).
void test_relocation() {
    CatNameDealloc dealloc;
    SndxCategoryHooks hooks = make_hooks(dealloc);
    hooks.grow = &doubling_grow;   // moves the buffer at i = 4, 8, 16
    SndxCategoryVectorSpan mute{};
    SndxCategoryVectorSpan filter{};
    ConCmdRegistry registry{};
    const SndxCategoriesResult r = build_sndx_category_vars(mute, filter, nullptr, registry, hooks);
    CHECK(r.ok);
    CHECK(r.grow_calls == 8);   // mute: 0->4->8->16->32 (4) and filter: 4
    for (std::size_t i = 0; i < kSndxCategoryCount; ++i) {
        auto* const m = reinterpret_cast<const ConCmdBoolVar*>(
            static_cast<char*>(mute.begin) + i * kSndxCategoryVarStride);
        auto* const f = reinterpret_cast<const ConCmdBoolVar*>(
            static_cast<char*>(filter.begin) + i * kSndxCategoryVarStride);
        check_slot(*m, SndxCategoryVectorId::mute, i);     // includes value_ref == &var.value
        check_slot(*f, SndxCategoryVectorId::filter, i);
    }
    // the interleaved pending-list order survived the moves
    CHECK(registry.pending.size() == 2 * kSndxCategoryCount);
    const ConCmdListNode* node = registry.pending.first();
    for (std::size_t i = 0; i < kSndxCategoryCount; ++i) {
        const void* want_mute = static_cast<char*>(mute.begin) + i * kSndxCategoryVarStride;
        const void* want_filter = static_cast<char*>(filter.begin) + i * kSndxCategoryVarStride;
        CHECK(base_of_pending(node) == want_mute);
        CHECK(node->prev != nullptr);
        node = node->next;
        CHECK(base_of_pending(node) == want_filter);
        node = node->next;
    }
    CHECK(node == &registry.pending.head);
    CHECK(registry.pending.head.prev != nullptr &&
          base_of_pending(registry.pending.head.prev) ==
              static_cast<const void*>(static_cast<char*>(filter.begin) +
                                        31 * kSndxCategoryVarStride));
    std::free(mute.begin);
    std::free(filter.begin);
}

}  // namespace

int main() {
    test_format_names();
    test_vectors_from_engine();
    test_full_build();
    test_relocation();
    test_fail_closed();
    std::printf("OK sndx_category_vars\n");
    return 0;
}
