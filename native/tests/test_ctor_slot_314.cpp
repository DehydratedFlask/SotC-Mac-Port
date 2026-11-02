// CPU-01-STARTUP-SLOT-314-CHARACTERIZATION-01 / -NATIVE-LIFT-01: fixture for startup constructor
// index 314 (0xa41810, the physics debug block: one "PhysXToRenderThread" page, ten console variables,
// ten "Physics" toggles, 22 __cxa_atexit records).
//
// Covers: the pinned constants (table word, object strides, atexit rows, variable/toggle rows, the two
// argument-functor classes and their gate answers), the page's raw store pattern, the three variable
// shapes with every stored field, the ten registrations, the complete atexit order, and the
// fail-visible stages: a missing page hook, a missing callback hook, a spec that does not match its
// pinned class, a failed menu registration, the parent-contract path, and a changed
// __stack_chk_guard.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_314_a41810.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
std::uint32_t dword_at(const T& obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 4);
    return v;
}

template <class T>
std::uint8_t byte_at(const T& obj, std::size_t off) {
    return reinterpret_cast<const std::uint8_t*>(&obj)[off];
}

template <class T>
bool region_poison(const T& obj, std::size_t off, std::size_t len) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < len; ++i) if (b[off + i] != kPoison) return false;
    return true;
}

template <class T>
bool region_zero(const T& obj, std::size_t off, std::size_t len) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < len; ++i) if (b[off + i] != 0) return false;
    return true;
}

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 && v.ctl == nullptr &&
           std::memcmp(v.data, text, len) == 0;
}

// Recording wrapper around the native models.
struct HookLog {
    std::uint32_t page_calls = 0;
    std::uint32_t callback_calls = 0;
    bool fail_page = false;
    bool fail_callback = false;
    bool refuse = false;
    std::uint64_t* guard = nullptr;
};
HookLog g_log{};

Slot314PageResult recording_page_ctor(Slot314Page& page, const SharedStr& name) noexcept {
    ++g_log.page_calls;
    if (g_log.fail_page) return Slot314PageResult{};
    return init_slot314_page_475c0(page, name);
}

Slot314VarResult recording_bool_ctor(Slot314BoolVar& var, const Slot314VarSpec& spec,
                                     ConCmdRegistry& registry) noexcept {
    const std::uint32_t i = g_log.callback_calls++;
    if (g_log.guard != nullptr && i == 0) *g_log.guard ^= 1;   // guard corruption inside the body (once)
    if (g_log.fail_callback) return Slot314VarResult{};
    if (g_log.refuse) return init_slot314_bool_var_callback(var, Slot314VarSpec{}, registry);
    return init_slot314_bool_var_callback(var, spec, registry);
}

// The body's own 22 atexit records, skipping the one 0x268b0 records for the menu root.
std::size_t slot_records(const StartupAtexitLog& log, StartupAtexitEntry* out, std::size_t cap) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < log.size(); ++i) {
        const StartupAtexitEntry& e = log.at(i);
        if (e.dtor_rva == kDebugMenuRootDtorRva) continue;   // recorded by 0x268b0, not by this body
        if (n < cap) out[n] = e;
        ++n;
    }
    return n;
}

void section_constants() {
    CHECK(kSlot314Rva == 0xa41810 && kSlot314BodySize == 4329 && kSlot314EndRva == 0xa428f9);
    CHECK(kSlot314ReachableBytes == 4328 && kSlot314InstructionCount == 779);
    CHECK(kSlot314DirectCallSites == 84 && kSlot314IndirectCallSites == 8);
    CHECK(kSlot314RipOperands == 196 && kSlot314RipTargets == 119);
    CHECK(kSlot314StoreSites == 63 && kSlot314StoreTargets == 56);
    // 0x2dd70e0 - 314*8 == 0x2dd6710, the one word that references the body.
    CHECK(kSlot314TableTopRva - 8 * kSlot314TableIndex == kSlot314TableWordRva);
    CHECK(kSlot314TableWordRva == 0x2dd6710 && kSlot314TableIndex == 314);
    CHECK(kSlot314NextUnrecoveredIndex == 316 && kSlot314NextUnrecoveredRva == 0xa33280);
    CHECK(kSlot314CanaryCellRva == 0x15fe518);
    CHECK(kSlot314StackChkFailRva == 0xf8f3a0 && kSlot314CanaryFrameOffset == 0xc0);
    CHECK(kSlot314BranchRva == 0xa428e2 && kSlot314BranchTargetRva == 0xa428f3);
    CHECK(kSlot314FrameBytes == 0xe0);
    CHECK(kSlot314AtexitRva == 0xf8f390 && kSlot314DsoRva == 0x1604218 && kSlot314ReleaseRva == 0xf590);
    CHECK(kSlot314PageCtorRva == 0x475c0 && kSlot314PageCtorSize == 87);
    CHECK(kSlot314BoolCallbackCtorRva == 0xa41570 && kSlot314BoolCallbackCtorSize == 669);
    CHECK(kSlot314FunctorDispatches == 63 && kSlot314BoolCallbackDispatches == 11);

    // Identity tags.
    CHECK(kSlot314PageVptr == 0x1594ca0 && kSlot314BoolMidVptr == 0x15940b0);
    CHECK(kSlot314BoolFinalVptr == 0x1594218 && kSlot314FloatFinalVptr == 0x15942b0);
    CHECK(kSlot314FloatFnVtbl == 0x1594318 && kSlot314BoolCallbackFnVtbl == 0x1594280);
    CHECK(kSlot314BoolArgFnVtbl == 0x15d1750 && kSlot314FloatArgFnVtbl == 0x15d1770);
    CHECK(kSlot314BoolArgDestroyRva == 0xc2f0 && kSlot314FloatArgDestroyRva == 0xc4f0);
    CHECK(kSlot314BoolGate3 == 1 && kSlot314BoolGate6 == 0);
    CHECK(kSlot314FloatGate3 == 1 && kSlot314FloatGate6 == 3);

    // The ten variables, in body order.
    CHECK(kSlot314Vars.size() == 10);
    for (std::size_t i = 0; i < kSlot314Vars.size(); ++i) {
        CHECK(kSlot314Vars[i].index == i);
        CHECK(std::strlen(kSlot314Vars[i].name) == kSlot314Vars[i].name_len);
    }
    CHECK(kSlot314Vars[0].shape == Slot314VarShape::bool_inline &&
          kSlot314Vars[1].shape == Slot314VarShape::bool_inline);
    CHECK(kSlot314Vars[2].shape == Slot314VarShape::ranged_float &&
          kSlot314Vars[3].shape == Slot314VarShape::ranged_float &&
          kSlot314Vars[4].shape == Slot314VarShape::ranged_float);
    for (int i = 5; i < 10; ++i) CHECK(kSlot314Vars[i].shape == Slot314VarShape::bool_callback);
    CHECK(kSlot314Vars[0].bool_value == 1 && kSlot314Vars[0].bool_default == 1);
    CHECK(kSlot314Vars[1].bool_value == 0 && kSlot314Vars[1].bool_default == 0);
    CHECK(kSlot314Vars[2].float_min_bits == 0 && kSlot314Vars[2].float_max_bits == 0x3f800000u &&
          kSlot314Vars[2].float_value_bits == 0x3f800000u);
    CHECK(kSlot314Vars[3].float_min_bits == 0xbf800000u && kSlot314Vars[3].float_max_bits == 0x3f800000u &&
          kSlot314Vars[3].float_value_bits == 0xbf800000u);
    CHECK(kSlot314Vars[4].float_min_bits == 0 && kSlot314Vars[4].float_max_bits == 0x42c80000u &&
          kSlot314Vars[4].float_value_bits == 0x3f800000u);
    const std::uint64_t want_q3[5] = {0xa40160, 0xa40230, 0xa40300, 0xa403c0, 0xa40480};
    for (int i = 5; i < 10; ++i) {
        CHECK(kSlot314Vars[i].arg_q1 == 0x274710 && kSlot314Vars[i].arg_q3 == want_q3[i - 5]);
        CHECK(kSlot314Vars[i].flags == kSlot314BoolCallbackFlags);
        CHECK(kSlot314Vars[i].bool_value == 0 && kSlot314Vars[i].bool_default == 0);
    }

    // The ten toggles, each pointing at its variable under the shared "Physics" caption.
    CHECK(kSlot314Items.size() == 10);
    for (std::size_t i = 0; i < kSlot314Items.size(); ++i) {
        CHECK(kSlot314Items[i].index == i && kSlot314Items[i].target_var == i);
        CHECK(kSlot314Items[i].parent_rva == kSlot314MenuParentRva && kSlot314Items[i].parent_len == 7);
        CHECK(std::strlen(kSlot314Items[i].caption) == kSlot314Items[i].caption_len);
        CHECK(kSlot314Items[i].flags == kSlot314ItemFlags);
    }

    // The 22 atexit rows, in body order.
    CHECK(kSlot314AtexitRows.size() == 22);
    for (std::size_t i = 0; i < kSlot314AtexitRows.size(); ++i) CHECK(kSlot314AtexitRows[i].step == i + 1);
    CHECK(kSlot314AtexitRows[0].dtor_rva == kSlot314PageDtorRva && kSlot314AtexitRows[0].object_rva == 0x32f5af0);
    CHECK(kSlot314AtexitRows[11].dtor_rva == kSlot314CellDtorRva && kSlot314AtexitRows[11].object_rva == 0x32f6538);
    CHECK(kSlot314AtexitRows[1].dtor_rva == kSlot314BoolVarDtorRva);
    CHECK(kSlot314AtexitRows[5].dtor_rva == kSlot314FloatVarDtorRva);
    CHECK(kSlot314AtexitRows[2].dtor_rva == kDebugMenuItemDtorRva);
    CHECK(kSlot314AtexitRows[21].object_rva == 0x32f6c70);

    // Object strides, the aggregate, and the adjacency to constructor index 313.
    CHECK(offsetof(Slot314Objects, page) == 0x000 && offsetof(Slot314Objects, var0) == 0x2e8);
    CHECK(offsetof(Slot314Objects, item0) == 0x3b0 && offsetof(Slot314Objects, var1) == 0x458);
    CHECK(offsetof(Slot314Objects, var2) == 0x5c8 && offsetof(Slot314Objects, var3) == 0x748);
    CHECK(offsetof(Slot314Objects, var4) == 0x8c8 && offsetof(Slot314Objects, cell) == 0xa48);
    CHECK(offsetof(Slot314Objects, var5) == 0xaf8 && offsetof(Slot314Objects, item5) == 0xbc0);
    CHECK(offsetof(Slot314Objects, var9) == 0x10b8 && offsetof(Slot314Objects, item9) == 0x1180);
    CHECK(sizeof(Slot314Objects) == 0x1228);
    CHECK(sizeof(Slot314Page) == 0x2e8 && sizeof(Slot314BoolVar) == 0xc8 && sizeof(Slot314Cell) == 0xb0);
    CHECK(kSlot314NextCtorFirstObjectRva == 0x32f6d18);   // slot 313's cell
    CHECK(kSlot314LastObjectRva + 0xa8 == kSlot314NextCtorFirstObjectRva);

    // Stage names.
    CHECK(std::strcmp(slot314_stage_name(Slot314Stage::none), "none") == 0);
    CHECK(std::strcmp(slot314_stage_name(Slot314Stage::page_ctor), "page_ctor") == 0);
    CHECK(std::strcmp(slot314_stage_name(Slot314Stage::variable_registration), "variable_registration") == 0);
    CHECK(std::strcmp(slot314_stage_name(Slot314Stage::cell_atexit), "cell_atexit") == 0);
    CHECK(std::strcmp(slot314_stage_name(Slot314Stage::canary), "canary") == 0);
}

void section_functor_and_target() {
    // The eight row-local `callq *0x18(%rax)`: inert only when the pinned body is a bare ret.
    CHECK(kSlot314FunctorSites.size() == 8);
    for (const auto& site : kSlot314FunctorSites) CHECK(slot314_inert_functor_destroy(site));
    Slot314FunctorSite unlifted = kSlot314FunctorSites[0];
    unlifted.slot3_is_bare_ret = false;
    CHECK(!slot314_inert_functor_destroy(unlifted));

    // The 0x26ea0 gate: {1, 0} for the bool class, {1, 3} for the float class, unknown otherwise.
    ConCmdBoolVar b{};
    b.base.vptr = kSlot314BoolFinalVptr;
    DebugMenuTarget t = slot314_var_menu_target(b.base);
    CHECK(t.known && t.object == &b.base && t.slot3 == 1 && t.slot6 == 0);
    ConCmdRangedVar f{};
    f.base.vptr = kSlot314FloatFinalVptr;
    t = slot314_var_menu_target(f.base);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 3);
    b.base.vptr = kSlot314BoolMidVptr;      // the transient tag must not answer
    CHECK(!slot314_var_menu_target(b.base).known);
}

void section_page() {
    Slot314Page page;
    poison(page);
    const Slot314PageResult r = init_slot314_page_475c0(page, l2::literal_view(kSlot314PageName));
    CHECK(r.ok);
    CHECK(page.vptr == kSlot314PageCtorVptr && page.byte08 == 0 && page.byte09 == 1);
    CHECK(page.name == reinterpret_cast<std::uint64_t>(kSlot314PageName));
    CHECK(page.word18 == 0);
    CHECK(region_poison(page, 0x0a, 6));                        // +0x0a never written
    CHECK(region_zero(page, 0x1c, 0x24));                       // +0x1c..+0x3f zeroed, incl. the +0x38 head
    CHECK(page.link40 == reinterpret_cast<std::uintptr_t>(&page) + 0x38);
    CHECK(page.link48 == page.link40);
    CHECK(page.word50 == 0 && page.word58 == 0 && page.word60 == 0 && page.word70 == 0);
    CHECK(region_poison(page, 0x68, 8));                        // +0x68 never written
    CHECK(region_poison(page, 0x78, 8));                        // +0x78 never written
    CHECK(region_poison(page, 0x80, 0xb0 - 0x80));              // the body's stores are not in the ctor

    // A name that is not the pinned literal is refused.
    CHECK(!init_slot314_page_475c0(page, SharedStr{}).ok);

    init_slot314_page_stores(page);
    CHECK(page.vptr == kSlot314PageVptr);                       // the body's derived tag
    CHECK(page.word80 == 0 && page.word88 == 0);
    CHECK(region_zero(page, 0x90, 0x20));
    for (std::size_t k = 0; k < page.nodes.size(); ++k) {
        const Slot314Node& node = page.nodes[k];
        CHECK(node.next == &node && node.prev == &node);
        CHECK(region_zero(node, 0x20, 0x10));
        CHECK(region_poison(node, 0x10, 0x10));                 // +0x10 never written
    }
    CHECK(page.last_node.next == &page.last_node && page.last_node.prev == &page.last_node);
    CHECK(region_poison(page.last_node, 0x10, 0x10));
    CHECK(region_poison(page, 0x2e4, 4));                       // +0x2e4 never written
    CHECK(byte_at(page, 0x2e0) == kPoison);                     // the tail word is the first variable's
}

void section_native_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot314Objects o;
    poison(o);                       // the original objects live in zero-fill .bss; every unwritten hole is poison here
    const Slot314Hooks hooks = default_slot314_hooks();
    CHECK(hooks.page_ctor == &init_slot314_page_475c0 &&
          hooks.bool_callback_ctor == &init_slot314_bool_var_callback && hooks.stack_guard == nullptr);
    const Slot314Result r = init_slot314_a41810(o, creg, menu, hooks);

    CHECK(r.ok && r.blocked == Slot314Stage::none && r.blocked_index == 0);
    CHECK(r.atexit_records == 22 && r.functors_destroyed == 8);
    CHECK(r.functor_dispatches == 5 * kSlot314BoolCallbackDispatches);
    CHECK(r.vars_built == 10 && r.menu_items == 10 && r.views_inert);
    CHECK(!r.canary_checked && !r.canary_matched);
    CHECK(r.page.ok);

    // The page: the ctor fields plus the body's raw store pattern and the first variable's tail word.
    CHECK(o.page.vptr == kSlot314PageVptr && o.page.name == reinterpret_cast<std::uint64_t>(kSlot314PageName));
    CHECK(o.page.tail2e0 == 1);
    for (std::size_t k = 0; k < o.page.nodes.size(); ++k) CHECK(o.page.nodes[k].next == &o.page.nodes[k]);

    // Variables 0 and 1: the inline bool shape (class 0x1594218, functor tag 0x15d1750).
    const ConCmdBoolVar* const bools[2] = {&o.var0, &o.var1};
    const std::uint8_t want_value[2] = {1, 0};
    for (int i = 0; i < 2; ++i) {
        const ConCmdBoolVar& v = *bools[i];
        CHECK(v.base.vptr == kSlot314BoolFinalVptr && v.base.flags == kSlot314BoolVarFlags);
        CHECK(v.base.registered == 1);
        CHECK(same_view(v.base.name, kSlot314Vars[i].name, kSlot314Vars[i].name_len));
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
        CHECK(v.value_ref == &v.value);
        CHECK(v.value == want_value[i] && v.default_value == want_value[i]);
        CHECK(v.fn_vptr == kSlot314BoolArgFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(r.var_results[i].ok && r.var_results[i].registration.ok);
    }

    // Variables 2..4: the bounded float shape through 0xc300 (class 0x15942b0, tag 0x1594318).
    const ConCmdRangedVar* const floats[3] = {&o.var2, &o.var3, &o.var4};
    for (int i = 0; i < 3; ++i) {
        const ConCmdRangedVar& v = *floats[i];
        const Slot314VarSpec& spec = kSlot314Vars[2 + i];
        CHECK(v.base.vptr == kSlot314FloatFinalVptr && v.base.flags == 1 && v.base.registered == 1);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.value_ref == &v.value);
        CHECK(dword_at(v, 0x6c) == spec.float_min_bits && dword_at(v, 0x74) == spec.float_max_bits);
        CHECK(v.min_engaged == 1 && v.max_engaged == 1);
        CHECK(dword_at(v, 0xd0) == spec.float_value_bits && dword_at(v, 0x68) == spec.float_value_bits);
        CHECK(v.fn_vptr == kSlot314FloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(v.args[0] == 0 && v.args[1] == 0 && v.args[2] == 0 && v.args[3] == 0);   // q[3] == 0
        CHECK(r.var_results[2 + i].ok && r.var_results[2 + i].registration.ok);
        CHECK(region_poison(v, 0x71, 3) && region_poison(v, 0x79, 7) && region_poison(v, 0xa0, 0x10));
        CHECK(region_poison(v, 0xb8, 8) && region_poison(v, 0xd4, 4));
    }

    // Variables 5..9: the callback bool shape through 0xa41570.
    const Slot314BoolVar* const cbs[5] = {&o.var5, &o.var6, &o.var7, &o.var8, &o.var9};
    for (int i = 0; i < 5; ++i) {
        const Slot314BoolVar& v = *cbs[i];
        const Slot314VarSpec& spec = kSlot314Vars[5 + i];
        CHECK(v.base.vptr == kSlot314BoolFinalVptr && v.base.flags == kSlot314BoolCallbackFlags);
        CHECK(v.base.registered == 1);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.value_ref == &v.value);
        CHECK(v.value == 0 && v.default_value == 0);
        CHECK(v.fn_vptr == kSlot314BoolCallbackFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]));   // q[3] != 0 -> self pointer
        CHECK(v.args[1] == spec.arg_q1 && v.args[2] == 0 && v.args[3] == spec.arg_q3);
        CHECK(r.var_results[5 + i].ok && r.var_results[5 + i].functor_dispatches == 11);
        CHECK(r.var_results[5 + i].registration.ok && r.var_results[5 + i].description_inert);
        CHECK(region_poison(v, 0x6d, 3) && region_poison(v, 0x90, 0x10) && region_poison(v, 0xa8, 8));
        CHECK(region_poison(v, 0xc1, 0xc8 - 0xc1));
    }

    // The ten toggles: kind 2, flags 0x30, the shared "Physics" caption, each pointing at its variable.
    const DebugMenuItem* const items[10] = {&o.item0, &o.item1, &o.item2, &o.item3, &o.item4,
                                            &o.item5, &o.item6, &o.item7, &o.item8, &o.item9};
    for (int i = 0; i < 10; ++i) {
        const DebugMenuItem& it = *items[i];
        CHECK(it.kind == kDebugMenuItemKind && it.flags == kSlot314ItemFlags);
        CHECK(same_view(it.label, kSlot314MenuParent, 7));
        CHECK(same_view(it.text, kSlot314Items[i].caption, kSlot314Items[i].caption_len));
        CHECK(it.target == &slot314_var_base(o, static_cast<std::uint32_t>(i)));
        // 0x268b0 leaves +0x20 at 0 on the root-list path; only the parent path sets it.
        CHECK(it.registered == 0 && r.item_registrations[i].path == DebugMenuRegisterPath::root_list);
        CHECK(r.item_registrations[i].ok);
        // The float targets answer {1, 3}, so their toggles take the narrowed range {0.1, 0.5}.
        const bool narrowed = (i >= 2 && i <= 4);
        CHECK(dword_at(it, 0x98) == (narrowed ? 0x3dcccccdU : 0x3f800000U));
        CHECK(dword_at(it, 0x9c) == (narrowed ? 0x3f000000U : 0x41200000U));
    }

    // Registration side effects: ten pending ConCmds, ten root-list toggles.
    CHECK(creg.pending.size() == 10);
    CHECK(creg.pending.first() == &o.var0.base.pending);   // the queue is in body order
    CHECK(menu.root_guard_done && menu.root.count == 10);

    // The 22 atexit records in body order, with the menu root's record between var 0 and item 0.
    CHECK(creg.atexit_log.size() == 23);
    CHECK(creg.atexit_log.at(2).dtor_rva == kDebugMenuRootDtorRva);
    CHECK(creg.atexit_log.at(2).object == &menu.root);
    StartupAtexitEntry recs[22]{};
    CHECK(slot_records(creg.atexit_log, recs, 22) == 22);
    const void* const want_object[22] = {
        &o.page, &o.var0, &o.item0, &o.var1, &o.item1, &o.var2, &o.item2, &o.var3, &o.item3, &o.var4,
        &o.item4, &o.cell, &o.var5, &o.item5, &o.var6, &o.item6, &o.var7, &o.item7, &o.var8, &o.item8,
        &o.var9, &o.item9};
    for (std::size_t i = 0; i < 22; ++i) {
        CHECK(recs[i].dtor_rva == kSlot314AtexitRows[i].dtor_rva);
        CHECK(recs[i].object == want_object[i]);
    }
}

void section_no_page_hook() {
    // Without the 0x475c0 contract nothing is built and nothing is recorded.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot314Objects o;
    poison(o);
    const Slot314Result r = init_slot314_a41810(o, creg, menu);
    CHECK(!r.ok && r.blocked == Slot314Stage::page_ctor);
    CHECK(r.atexit_records == 0 && r.vars_built == 0 && r.menu_items == 0);
    CHECK(creg.atexit_log.size() == 0);
    CHECK(region_poison(o.page, 0x08, 0x2e0));
}

void section_no_callback_hook() {
    // The page and the first five variables are built; the sixth variable has no contract.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot314Objects o;
    poison(o);
    Slot314Hooks hooks{};
    hooks.page_ctor = &init_slot314_page_475c0;
    const Slot314Result r = init_slot314_a41810(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot314Stage::variable_build && r.blocked_index == 5);
    CHECK(r.atexit_records == 12 && r.vars_built == 5 && r.menu_items == 5);
    CHECK(region_poison(o.var5, 0x00, 0xc8));
}

void section_foreign_spec_refused() {
    // A spec that does not match its pinned class row must make the native models refuse and write
    // nothing.
    ConCmdRegistry creg;
    Slot314BoolVar boolvar;
    poison(boolvar);
    Slot314VarSpec broken = kSlot314Vars[5];
    broken.shape = Slot314VarShape::bool_inline;         // the callback ctor only takes bool_callback
    CHECK(!init_slot314_bool_var_callback(boolvar, broken, creg).ok);
    broken = kSlot314Vars[5];
    broken.fn_tag = kSlot314FloatArgFnVtbl;              // the float argument class under a bool shape
    CHECK(!init_slot314_bool_var_callback(boolvar, broken, creg).ok);

    ConCmdRangedVar floatvar;
    poison(floatvar);
    Slot314VarSpec broken_float = kSlot314Vars[2];
    broken_float.fn_tag = kSlot314BoolArgFnVtbl;         // the bool argument class under 0xc300
    CHECK(!init_slot314_float_var_c300(floatvar, broken_float));
    CHECK(creg.pending.size() == 0);

    // A refusing hook stops the slot at that variable's stage.
    g_log = HookLog{};
    g_log.refuse = true;
    Slot314Hooks hooks = default_slot314_hooks();
    hooks.bool_callback_ctor = &recording_bool_ctor;
    DebugMenuRegistry menu;
    Slot314Objects o;
    poison(o.var5);
    const Slot314Result r = init_slot314_a41810(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot314Stage::variable_build && r.blocked_index == 5);
    CHECK(g_log.callback_calls == 1);
    CHECK(region_poison(o.var5, 0x00, 0xc8));
}

void section_menu_registration_blocked() {
    // A non-null parent cell with no insert contract fails the first toggle registration visibly,
    // after the first variable was built.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    Slot314Objects o;
    const Slot314Result r = init_slot314_a41810(o, creg, menu, default_slot314_hooks());
    CHECK(!r.ok && r.blocked == Slot314Stage::item_registration && r.blocked_index == 0);
    CHECK(r.vars_built == 1 && r.atexit_records == 2 && r.menu_items == 0);
    CHECK(!r.item_registrations[0].ok &&
          r.item_registrations[0].path == DebugMenuRegisterPath::parent);
    CHECK(menu.root.count == 0 && !menu.root_guard_done);
}

void section_parent_contract() {
    // With a menu parent contract every toggle goes to the parent and the root is never initialised,
    // so the atexit log holds exactly the body's 22 records.
    static int inserted = 0;
    struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
    inserted = 0;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    menu.parent.insert = &P::insert;
    Slot314Objects o;
    const Slot314Result r = init_slot314_a41810(o, creg, menu, default_slot314_hooks());
    CHECK(r.ok && inserted == 10 && menu.root.count == 0 && !menu.root_guard_done);
    CHECK(o.item0.registered == 1 && o.item9.registered == 1);   // the parent path sets +0x20
    CHECK(creg.atexit_log.size() == 22 && r.atexit_records == 22);
    for (std::size_t i = 0; i < creg.atexit_log.size(); ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == kSlot314AtexitRows[i].dtor_rva);
    }
}

void section_canary() {
    // The body's only branch is the __stack_chk_guard compare. A guard that changed inside the body
    // takes the original's `jne 0xa428f3` / __stack_chk_fail path, so the model fails at `canary`
    // after every object was built.
    g_log = HookLog{};
    std::uint64_t guard = 0x0123456789abcdefull;
    g_log.guard = &guard;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot314Objects o;
    Slot314Hooks hooks = default_slot314_hooks();
    hooks.bool_callback_ctor = &recording_bool_ctor;
    hooks.stack_guard = &guard;
    const Slot314Result r = init_slot314_a41810(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot314Stage::canary);
    CHECK(r.canary_checked && !r.canary_matched);
    CHECK(r.atexit_records == 22 && r.menu_items == 10 && r.vars_built == 10);
    CHECK(creg.atexit_log.size() == 23);
}

void section_recording_hooks() {
    // The recorded page hook runs exactly once; the recorded callback hook exactly five times.
    g_log = HookLog{};
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot314Objects o;
    Slot314Hooks hooks{};
    hooks.page_ctor = &recording_page_ctor;
    hooks.bool_callback_ctor = &recording_bool_ctor;
    const Slot314Result r = init_slot314_a41810(o, creg, menu, hooks);
    CHECK(r.ok && g_log.page_calls == 1 && g_log.callback_calls == 5);
    CHECK(o.page.vptr == kSlot314PageVptr && o.var9.fn_vptr == kSlot314BoolCallbackFnVtbl);

    // A failing page hook stops before the page atexit record.
    g_log = HookLog{};
    g_log.fail_page = true;
    ConCmdRegistry creg2;
    DebugMenuRegistry menu2;
    Slot314Objects o2;
    Slot314Hooks hooks2{};
    hooks2.page_ctor = &recording_page_ctor;
    hooks2.bool_callback_ctor = &recording_bool_ctor;
    const Slot314Result r2 = init_slot314_a41810(o2, creg2, menu2, hooks2);
    CHECK(!r2.ok && r2.blocked == Slot314Stage::page_ctor && creg2.atexit_log.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_functor_and_target();
    section_page();
    section_native_build();
    section_no_page_hook();
    section_no_callback_hook();
    section_foreign_spec_refused();
    section_menu_registration_blocked();
    section_parent_contract();
    section_canary();
    section_recording_hooks();
    std::puts("OK test_ctor_slot_314");
    return 0;
}
