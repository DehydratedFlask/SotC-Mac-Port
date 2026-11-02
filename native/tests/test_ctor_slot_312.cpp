// CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01 / -NATIVE-LIFT-01: fixture for startup constructor
// index 312 (0xa46130, the "Platform Services" / "TitleInfo" entitlement block).
//
// Covers: the pinned constants (table word, object strides, atexit table, class rows, functor tags),
// the two row-local `callq *0x18(%rax)` sites, the two 0x26ea0 gate answers of the int classes, the
// full 8-object build with every stored field and the atexit order, and the fail-visible stages:
// a missing var hook, a failing var hook, a spec that does not match its pinned class row, a failed
// menu registration, the parent-contract path, and a changed __stack_chk_guard.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_312_a46130.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
bool all_poison(const T& obj) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < sizeof obj; ++i) if (b[i] != kPoison) return false;
    return true;
}

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

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 && v.ctl == nullptr &&
           std::memcmp(v.data, text, len) == 0;
}

template <class T>
bool region_poison(const T& obj, std::size_t off, std::size_t len) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < len; ++i) if (b[off + i] != kPoison) return false;
    return true;
}

// Recording wrapper around the native int-variable model: proves the slot hands the pinned spec over,
// and can fail or corrupt the guard on demand.
struct HookLog {
    std::uint32_t calls = 0;
    std::uint64_t wrappers[2]{};
    bool fail_on_call = false;
    bool refuse = false;
    std::uint64_t* guard = nullptr;
};
HookLog g_log{};

Slot312IntVarResult recording_var_ctor(ConCmdRangedVar& var, const Slot312IntVarSpec& spec,
                                       ConCmdRegistry& registry) noexcept {
    const std::uint32_t i = g_log.calls++;
    if (i < 2) g_log.wrappers[i] = spec.wrapper_rva;
    if (g_log.guard != nullptr && i == 0) *g_log.guard ^= 1;   // guard corruption inside the body (once)
    if (g_log.fail_on_call) return Slot312IntVarResult{};  // visible failure, nothing claimed
    if (g_log.refuse) return init_slot312_int_var(var, Slot312IntVarSpec{}, registry);  // unpinned spec
    return init_slot312_int_var(var, spec, registry);
}

// The slot's own eight atexit records, skipping the one `0x268b0` records for the menu root.
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
    CHECK(kSlot312Rva == 0xa46130 && kSlot312BodySize == 1250 && kSlot312EndRva == 0xa46612);
    CHECK(kSlot312ReachableBytes == 1249 && kSlot312InstructionCount == 238);
    CHECK(kSlot312DirectCallSites == 30 && kSlot312IndirectCallSites == 2);
    CHECK(kSlot312RipOperands == 49 && kSlot312StoreSites == 8);
    // 0x2dd70e0 - 312*8 == 0x2dd6720, the one RELATIVE row that references the body.
    CHECK(kSlot312TableTopRva - 8 * kSlot312TableIndex == kSlot312TableWordRva);
    CHECK(kSlot312NextUnrecoveredIndex == 314 && kSlot312NextUnrecoveredRva == 0xa41810);
    CHECK(kSlot312CanaryCellRva == 0x15fe518 && kSlot312StackChkFailRva == 0xf8f3a0);
    CHECK(kSlot312BranchRva == 0xa465f8 && kSlot312BranchTargetRva == 0xa4660c);
    CHECK(kSlot312FrameBytes == 0x88 && kSlot312CanaryFrameOffset == 0x30);
    CHECK(kSlot312AtexitRva == 0xf8f390 && kSlot312DsoRva == 0x1604218 && kSlot312ReleaseRva == 0xf590);
    CHECK(kSlot312WrapperSize == 442 && kSlot312LeafSize == 323);

    // The eight original objects, in atexit order, and their exact strides.
    const std::uint64_t want_rva[8] = {0x32f6f58, 0x32f70a0, 0x32f7178, 0x32f7250,
                                       0x32f72f8, 0x32f73a0, 0x32f7448, 0x32f74f0};
    const std::uint64_t want_dtor[8] = {0x23c90, 0x18a60, 0xa44a80, 0x272d0,
                                        0x272d0, 0x272d0, 0x272d0, 0xa44ac0};
    const std::uint64_t want_site[8] = {0xa461d3, 0xa46287, 0xa46338, 0xa463c6,
                                        0xa46447, 0xa464cf, 0xa46557, 0xa465e5};
    CHECK(kSlot312AtexitRows.size() == 8);
    for (std::size_t i = 0; i < kSlot312AtexitRows.size(); ++i) {
        CHECK(kSlot312AtexitRows[i].step == i + 1);
        CHECK(kSlot312AtexitRows[i].object_rva == want_rva[i]);
        CHECK(kSlot312AtexitRows[i].dtor_rva == want_dtor[i]);
        CHECK(kSlot312AtexitRows[i].site_rva == want_site[i]);
    }

    // The two int-variable class rows (identity tags only).
    CHECK(kSlot312IntVarClassCount == 2 && kSlot312IntVars.size() == 2);
    CHECK(kSlot312IntVarClasses[0].wrapper_rva == 0xa45ba0 && kSlot312IntVarClasses[0].leaf_rva == 0x3a980);
    CHECK(kSlot312IntVarClasses[1].wrapper_rva == 0xa45de0 && kSlot312IntVarClasses[1].leaf_rva == 0xa45fa0);
    CHECK(kSlot312IntVarClasses[0].final_vptr == 0x1594c08 && kSlot312IntVarClasses[0].mid_vptr == 0x15941a0);
    CHECK(kSlot312IntVarClasses[1].final_vptr == 0x15d18f0 && kSlot312IntVarClasses[1].mid_vptr == 0x15d1818);
    CHECK(kSlot312IntVarClasses[0].seed_fn_tag == 0x1595660 && kSlot312IntVarClasses[0].final_fn_tag == 0x15944a0);
    CHECK(kSlot312IntVarClasses[1].seed_fn_tag == 0x15d18c0 && kSlot312IntVarClasses[1].final_fn_tag == 0x15d18c0);
    CHECK(kSlot312IntVarClasses[0].dtor_rva == 0x18a60 && kSlot312IntVarClasses[1].dtor_rva == 0xa44a80);
    CHECK(kSlot312IntVarClasses[0].slot3 == 1 && kSlot312IntVarClasses[0].slot6 == 2);
    CHECK(kSlot312IntVarClasses[1].slot3 == 1 && kSlot312IntVarClasses[1].slot6 == 1);
    CHECK(kSlot312IntVars[0].value == 0xffffffffu && kSlot312IntVars[1].value == 0u);
    CHECK(kSlot312IntVars[0].arg_q1 == 0x52dab0 && kSlot312IntVars[0].arg_q3 == 0xa44a40);
    CHECK(kSlot312IntVars[1].arg_q1 == 0xa45dc0 && kSlot312IntVars[1].arg_q3 == 0xa44a60);
    CHECK(kSlot312IntVars[0].name_len == 0x13 && kSlot312IntVars[1].name_len == 0xb);
    for (const auto& v : kSlot312IntVars) CHECK(std::strlen(v.name) == v.name_len);
    // The two row-local functor sites and their (bare `ret`) slot-3 bodies.
    CHECK(kSlot312FunctorSites.size() == 2);
    CHECK(kSlot312FunctorSites[0].site_rva == 0xa4626c && kSlot312FunctorSites[0].tag_rva == 0x15d18a0 &&
          kSlot312FunctorSites[0].slot3_rva == 0xc770);
    CHECK(kSlot312FunctorSites[1].site_rva == 0xa46320 && kSlot312FunctorSites[1].tag_rva == 0x15d18c0 &&
          kSlot312FunctorSites[1].slot3_rva == 0xa45db0);
    for (const auto& site : kSlot312FunctorSites) CHECK(site.slot3_is_bare_ret);

    // The record's raw stores.
    CHECK(kSlot312RecordWord20 == 0 && kSlot312RecordWord34 == 0 && kSlot312RecordWord38 == 0xffffffffu &&
          kSlot312RecordWord3c == 0x72);
    CHECK(kSlot312RecordBlock24[0] == 1 && kSlot312RecordBlock24[1] == 7 &&
          kSlot312RecordBlock24[2] == 0xffffffffu && kSlot312RecordBlock24[3] == 0xffffffffu);
    CHECK(kSlot312RecordBlockRva == 0x12c2e60 && kSlot312RecordNameRva == 0x12c2e8b);

    // Object strides and the aggregate offsets.
    CHECK(offsetof(Slot312Objects, age_restriction) == 0x148 && offsetof(Slot312Objects, region) == 0x220);
    CHECK(offsetof(Slot312Objects, group) == 0x2f8 && offsetof(Slot312Objects, toggle_sku) == 0x3a0);
    CHECK(offsetof(Slot312Objects, toggle_age) == 0x448 && offsetof(Slot312Objects, toggle_region) == 0x4f0);
    CHECK(offsetof(Slot312Objects, record) == 0x598 && sizeof(Slot312Objects) == 0x618);
    CHECK(std::strcmp(slot312_stage_name(Slot312Stage::none), "none") == 0);
    CHECK(std::strcmp(slot312_stage_name(Slot312Stage::age_restriction_build), "age_restriction_build") == 0);
    CHECK(std::strcmp(slot312_stage_name(Slot312Stage::canary), "canary") == 0);
}

void section_functor_and_target() {
    // The lifted `callq *0x18(%rax)`: inert only when the pinned body is a bare ret.
    for (const auto& site : kSlot312FunctorSites) CHECK(slot312_inert_functor_destroy(site));
    Slot312FunctorSite unlifted = kSlot312FunctorSites[0];
    unlifted.slot3_is_bare_ret = false;
    CHECK(!slot312_inert_functor_destroy(unlifted));

    // The 0x26ea0 gate: {1, 2} for class A, {1, 1} for class B, unknown for anything else.
    ConCmdRangedVar var{};
    var.base.vptr = kSlot312IntVarClasses[0].final_vptr;
    DebugMenuTarget t = slot312_int_var_menu_target(var);
    CHECK(t.known && t.object == &var && t.slot3 == 1 && t.slot6 == 2);
    var.base.vptr = kSlot312IntVarClasses[1].final_vptr;
    t = slot312_int_var_menu_target(var);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 1);
    var.base.vptr = kSlot312IntVarClasses[0].mid_vptr;      // the transient tag must not answer
    CHECK(!slot312_int_var_menu_target(var).known);
}

void section_native_build() {
    // The production path: `default_slot312_hooks()` installs the native wrapper+leaf model.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot312Objects o;
    poison(o.age_restriction);
    poison(o.region);
    const Slot312Hooks hooks = default_slot312_hooks();
    CHECK(hooks.var_ctor == &init_slot312_int_var && hooks.stack_guard == nullptr);
    const Slot312Result r = init_slot312_a46130(o, creg, menu, hooks);

    CHECK(r.ok && r.blocked == Slot312Stage::none);
    CHECK(r.atexit_records == 8 && r.functors_destroyed == 2 && r.var_functor_calls == 22);
    CHECK(r.vars_built == 3 && r.menu_items == 4 && r.views_inert);
    CHECK(!r.canary_checked && !r.canary_matched);

    // 1. `titleskuid`: the shared string-variable model, value and default are both "DEFAULT".
    const ConCmdStringVar& sku = o.title_sku;
    CHECK(sku.base.vptr == kStringVarFinalVptr && sku.base.flags == 1 && sku.base.registered == 1);
    CHECK(same_view(sku.base.name, "titleskuid", 0xa));
    CHECK(same_view(sku.value, "DEFAULT", 7) && same_view(sku.default_value, "DEFAULT", 7));
    CHECK(sku.value_ref == &sku.value);
    CHECK(sku.fn_vptr == kStringVarFnVtbl && sku.fn_payload == 0 && sku.fn_aux == 0);
    CHECK(r.string_var.ok && r.string_var.views_inert);

    // 2..3. The two int variables: class A (-1) and class B (0), both bounds disengaged.
    const ConCmdRangedVar* const vars[2] = {&o.age_restriction, &o.region};
    const std::uint32_t want_value[2] = {0xffffffffu, 0u};
    for (int i = 0; i < 2; ++i) {
        const ConCmdRangedVar& v = *vars[i];
        const Slot312IntVarClass& row = kSlot312IntVarClasses[i];
        const Slot312IntVarSpec& spec = kSlot312IntVars[i];
        CHECK(v.base.vptr == row.final_vptr && v.base.flags == 1 && v.base.registered == 1);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
        CHECK(v.value_ref == &v.value);                       // the object's own +0xd0, not a stack cell
        CHECK(dword_at(v, 0x68) == want_value[i] && dword_at(v, 0xd0) == want_value[i]);
        CHECK(v.min_engaged == 0 && v.max_engaged == 0);
        // The leaf writes the SINGLE bytes +0x6c / +0x74; +0x6d..+0x6f and +0x75..+0x77 are untouched.
        CHECK(byte_at(v, 0x6c) == 0 && byte_at(v, 0x74) == 0);
        CHECK(v.fn_vptr == row.final_fn_tag && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]));   // q[3] != 0 self pointer
        CHECK(v.args[1] == spec.arg_q1 && v.args[2] == 0 && v.args[3] == spec.arg_q3);
        CHECK(r.int_vars[i].ok && r.int_vars[i].functor_slot_calls == kSlot312VarFunctorDispatches);
        CHECK(r.int_vars[i].description_inert && r.int_vars[i].local_functor_inert);
        // The five regions of the shape no body of this family writes.
        CHECK(region_poison(v, 0x19, 3) && region_poison(v, 0x6d, 3) && region_poison(v, 0x71, 3));
        CHECK(region_poison(v, 0x75, 3) && region_poison(v, 0x79, 7) && region_poison(v, 0xa0, 0x10));
        CHECK(region_poison(v, 0xb8, 8) && region_poison(v, 0xd4, 4));
    }

    // 4. The menu group.
    const DebugMenuItem& g = o.group;
    CHECK(g.kind == kMenuKindGroup && g.flags == 0x30);
    CHECK(same_view(g.label, "Platform Services", 0x11) && same_view(g.text, "TitleInfo", 9));
    CHECK(g.target == nullptr && dword_at(g, 0x98) == 0x3f800000u && dword_at(g, 0x9c) == 0x41200000u);
    CHECK(r.group_registration.ok && r.group_registration.root_initialised);

    // 5..7. The three toggles target the three variables and keep the default range (all three gate
    // answers are {1, n} with n != 3, so the narrowing branch cannot be taken).
    const DebugMenuItem* const items[3] = {&o.toggle_sku, &o.toggle_age, &o.toggle_region};
    const void* const want_target[3] = {&o.title_sku, &o.age_restriction, &o.region};
    const char* const want_text[3] = {kSlot312ToggleSkuText, kSlot312ToggleAgeText, kSlot312ToggleRegionText};
    for (int i = 0; i < 3; ++i) {
        const DebugMenuItem& it = *items[i];
        CHECK(it.kind == kDebugMenuItemKind && it.flags == 0x30);
        CHECK(it.target == want_target[i]);
        CHECK(same_view(it.label, "TitleInfo", 9));
        CHECK(same_view(it.text, want_text[i], std::strlen(want_text[i])));
        CHECK(dword_at(it, 0x98) == 0x3f800000u && dword_at(it, 0x9c) == 0x41200000u);
        CHECK(r.toggle_registrations[i].ok && !r.toggle_registrations[i].root_initialised);
    }

    // 8. The 0x80 B record: raw stores only, field meanings not claimed.
    const Slot312Record& rec = o.record;
    CHECK(same_view(rec.name, "DEFAULT", 7));
    CHECK(rec.word20 == 0 && rec.word34 == 0 && rec.word38 == 0xffffffffu && rec.word3c == 0x72);
    CHECK(rec.block24[0] == 1 && rec.block24[1] == 7 && rec.block24[2] == 0xffffffffu &&
          rec.block24[3] == 0xffffffffu);
    CHECK(rec.empty40.data != nullptr && rec.empty40.size == 0 && rec.empty40.ctl == nullptr &&
          rec.empty40.data[0] == 0);
    CHECK(rec.empty60.data != nullptr && rec.empty60.size == 0 && rec.empty60.ctl == nullptr &&
          rec.empty60.data[0] == 0);

    // Registration side effects: three pending ConCmds, four root-list menu items.
    CHECK(creg.pending.size() == 3 && creg.pending.first() == &sku.base.pending);
    CHECK(menu.root_guard_done && menu.root.count == 4);

    // The eight atexit records, in body order, with the menu root's record between 3 and 4.
    CHECK(creg.atexit_log.size() == 9);
    CHECK(creg.atexit_log.at(3).dtor_rva == kDebugMenuRootDtorRva);
    CHECK(creg.atexit_log.at(3).object == &menu.root);
    StartupAtexitEntry recs[8]{};
    CHECK(slot_records(creg.atexit_log, recs, 8) == 8);
    const void* const want_object[8] = {&sku, &o.age_restriction, &o.region, &o.group,
                                        &o.toggle_sku, &o.toggle_age, &o.toggle_region, &o.record};
    for (std::size_t i = 0; i < 8; ++i) {
        CHECK(recs[i].dtor_rva == kSlot312AtexitRows[i].dtor_rva);
        CHECK(recs[i].object == want_object[i]);
    }
}

void section_no_var_hook() {
    // Without the 0xa45ba0/0xa45de0 contract the fragment stops before the first int variable: the
    // string variable and its atexit record exist, the two int stand-ins were never touched.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot312Objects o;
    poison(o.age_restriction);
    poison(o.region);
    const Slot312Result r = init_slot312_a46130(o, creg, menu);
    CHECK(!r.ok && r.blocked == Slot312Stage::age_restriction_build);
    CHECK(r.atexit_records == 1 && r.vars_built == 1 && r.menu_items == 0);
    CHECK(creg.atexit_log.size() == 1);
    CHECK(region_poison(o.age_restriction, 0x08, 0xd0));
    CHECK(region_poison(o.region, 0x08, 0xd0));
    CHECK(!r.canary_checked && !r.canary_matched);
}

void section_var_hook_fails() {
    // A hook that reports failure stops the slot at that variable's stage; the wrapper's pre-stored
    // final tag is the only byte the slot itself writes.
    g_log = HookLog{};
    g_log.fail_on_call = true;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot312Objects o;
    poison(o.age_restriction);
    Slot312Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    const Slot312Result r = init_slot312_a46130(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot312Stage::age_restriction_build);
    CHECK(r.atexit_records == 1 && r.vars_built == 1 && r.functors_destroyed == 0);
    CHECK(o.age_restriction.base.vptr == kSlot312IntVarClasses[0].final_vptr);
    CHECK(region_poison(o.age_restriction, 0x08, 0xd0));
}

void section_foreign_spec_refused() {
    // A spec whose wrapper is not a pinned class row, or whose leaf/dtor disagree with it, must make
    // the native model refuse and write nothing.
    ConCmdRegistry creg;
    ConCmdRangedVar var;
    poison(var);
    Slot312IntVarSpec broken = kSlot312IntVars[0];
    broken.wrapper_rva = 0xa4f4d0;     // slot 309's wrapper, not this body's class
    CHECK(!init_slot312_int_var(var, broken, creg).ok);
    CHECK(all_poison(var));
    CHECK(creg.pending.size() == 0);

    broken = kSlot312IntVars[0];
    broken.leaf_rva = 0xa45fa0;        // class B's leaf under class A's wrapper
    Slot312IntVarResult rr = init_slot312_int_var(var, broken, creg);
    CHECK(!rr.ok && rr.functor_slot_calls == 0);
    CHECK(all_poison(var));

    // The recording hook's `refuse` mode goes through the slot and must stop at the build stage.
    g_log = HookLog{};
    g_log.refuse = true;
    DebugMenuRegistry menu;
    Slot312Objects o;
    poison(o.age_restriction);
    Slot312Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    const Slot312Result r = init_slot312_a46130(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot312Stage::age_restriction_build && r.atexit_records == 1);
    CHECK(region_poison(o.age_restriction, 0x08, 0xd0));
    CHECK(g_log.calls == 1);
}

void section_menu_registration_blocked() {
    // A non-null parent cell with no insert contract fails the group registration visibly, after the
    // three variables were built.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    Slot312Objects o;
    poison(o.group);
    const Slot312Result r = init_slot312_a46130(o, creg, menu, default_slot312_hooks());
    CHECK(!r.ok && r.blocked == Slot312Stage::group_registration);
    CHECK(r.vars_built == 3 && r.atexit_records == 3 && r.menu_items == 0);
    CHECK(!r.group_registration.ok && r.group_registration.path == DebugMenuRegisterPath::parent);
    CHECK(menu.root.count == 0 && !menu.root_guard_done);
}

void section_parent_contract() {
    // With a menu parent contract every item goes to the parent and the root is never initialised, so
    // the atexit log holds exactly the body's eight records.
    static int inserted = 0;
    struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
    inserted = 0;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    menu.parent.insert = &P::insert;
    Slot312Objects o;
    const Slot312Result r = init_slot312_a46130(o, creg, menu, default_slot312_hooks());
    CHECK(r.ok && inserted == 4 && menu.root.count == 0 && !menu.root_guard_done);
    CHECK(o.group.registered == 1 && o.toggle_sku.registered == 1 && o.toggle_age.registered == 1 &&
          o.toggle_region.registered == 1);
    CHECK(creg.atexit_log.size() == 8 && r.atexit_records == 8);
    for (std::size_t i = 0; i < creg.atexit_log.size(); ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == kSlot312AtexitRows[i].dtor_rva);
    }
}

void section_canary() {
    // The body's only branch is the __stack_chk_guard compare. A guard that changed inside the body
    // takes the original's `jne 0xa4660c` / __stack_chk_fail path, so the model fails at `canary`
    // after every object was built.
    g_log = HookLog{};
    std::uint64_t guard = 0x0123456789abcdefull;
    g_log.guard = &guard;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot312Objects o;
    Slot312Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    hooks.stack_guard = &guard;
    const Slot312Result r = init_slot312_a46130(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot312Stage::canary);
    CHECK(r.canary_checked && !r.canary_matched);
    CHECK(r.atexit_records == 8 && r.menu_items == 4);
    CHECK(creg.atexit_log.size() == 9);
}

}  // namespace

int main() {
    section_constants();
    section_functor_and_target();
    section_native_build();
    section_no_var_hook();
    section_var_hook_fails();
    section_foreign_spec_refused();
    section_menu_registration_blocked();
    section_parent_contract();
    section_canary();
    std::puts("OK test_ctor_slot_312");
    return 0;
}
