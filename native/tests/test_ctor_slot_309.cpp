// CPU-01-STARTUP-SLOT-309-NATIVE-LIFT-01: fixture for startup constructor index 309 (0xa505e0).
//
// Covers: the pinned constants (table word, object strides, atexit table, functor/vtable identities),
// the lifted `callq *0x18(%rax)` functor destroy and the 0x26ea0 gate answers, the full 12-object
// build with every stored field and the 13-entry atexit order (12 slot records + the debug-menu root),
// and the fail-visible stages: missing var hook, failing var hook, refusing functor destroy, failed
// 0x20d60 registration, the parent-contract path, and a changed __stack_chk_guard.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "core/ctor_slot_309_a505e0.hpp"

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

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 && v.ctl == nullptr &&
           std::memcmp(v.data, text, len) == 0;
}

// The three variable stand-ins are hook-owned; this hook only records what it was asked to build.
struct HookLog {
    std::uint32_t calls = 0;
    std::uint64_t ctors[3]{};
    std::uint64_t vptrs[3]{};
    std::uint64_t functors[3]{};
    std::uint64_t dtors[3]{};
    std::int32_t min[3]{};
    std::int32_t max[3]{};
    std::uint32_t flags[3]{};
    bool fail_on_call = false;
    std::uint64_t* guard = nullptr;   // when set, the hook flips one bit of the guard (corruption)
};
HookLog g_log{};

bool recording_var_ctor(Slot309VarObject& object, const Slot309VarSpec& spec) noexcept {
    const std::uint32_t i = g_log.calls++;
    if (i < 3) {
        g_log.ctors[i] = spec.ctor_rva;
        g_log.vptrs[i] = spec.vptr_tag;
        g_log.functors[i] = spec.functor_tag;
        g_log.dtors[i] = spec.dtor_rva;
        g_log.min[i] = spec.min_value;
        g_log.max[i] = spec.max_value;
        g_log.flags[i] = spec.flags;
        if (!g_log.fail_on_call) object.bytes[8] = 0x5A;   // proof the hook owned the interior
    }
    if (g_log.guard != nullptr) *g_log.guard ^= 1;
    return !g_log.fail_on_call;
}

bool refusing_functor(void*, const Slot309FunctorSite&) noexcept { return false; }

bool destroy_calls = false;
bool counting_functor(void*, const Slot309FunctorSite&) noexcept { destroy_calls = true; return true; }

// The slot's own 12 atexit records, skipping the one `0x268b0` records for the menu root.
std::size_t slot_records(const StartupAtexitLog& log, StartupAtexitEntry* out, std::size_t cap) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < log.size(); ++i) {
        const StartupAtexitEntry& e = log.at(i);
        if (e.dtor_rva == kSlot309MenuRootDtorRva) continue;   // recorded by 0x268b0, not by this body
        if (n < cap) out[n] = e;
        ++n;
    }
    return n;
}

void section_constants() {
    CHECK(kSlot309Rva == 0xa505e0 && kSlot309BodySize == 1892 && kSlot309EndRva == 0xa50d44);
    CHECK(kSlot309InstructionCount == 354 && kSlot309DirectCallSites == 40 && kSlot309FunctorSiteCount == 3);
    // 0x2dd70e0 - 309*8 == 0x2dd6738, the one RELATIVE row that references the body.
    CHECK(kSlot309TableTopRva - 8 * kSlot309TableIndex == kSlot309TableWordRva);
    CHECK(kSlot309CanaryCellRva == 0x15fe518 && kSlot309StackChkFailRva == 0xf8f3a0);
    CHECK(kSlot309BranchTargetRva == kSlot309StackChkFailCallRva && kSlot309BranchRva == 0xa50d2d);
    CHECK(kSlot309FrameBytes == 0xe0 && kSlot309CanaryFrameOffset == 0xc0);
    // Frame: push rbp + 5 pushes then `and rsp,-0x20` + `sub rsp,0xe0`; the epilogue restores -0x28(%rbp).
    CHECK(kSlot309AtexitRva == 0xf8f390 && kSlot309DsoRva == 0x1604218 && kSlot309ReleaseRva == 0xf590);
    // The 12 original objects, in atexit order, and their exact strides.
    const std::uint64_t want_rva[12] = {0x32f79b8, 0x32f79c8, 0x32f79e8, 0x32f7ac0, 0x32f7af0, 0x32f7bc0,
                                        0x32f7be0, 0x32f7c88, 0x32f7d30, 0x32f7e08, 0x32f7eb0, 0x32f7f88};
    const std::uint64_t want_dtor[12] = {0xa46620, 0xa47cb0, 0xa47d20, 0x2c84f0, 0x20b0, 0xf7b0,
                                         0x272d0,  0x272d0,  0xa48240, 0x272d0,  0xa4ab50, 0x272d0};
    const std::uint64_t want_site[12] = {0xa5062d, 0xa5065d, 0xa50754, 0xa507d6, 0xa508e2, 0xa50903,
                                         0xa50998, 0xa50a15, 0xa50b13, 0xa50b97, 0xa50c92, 0xa50d16};
    CHECK(kSlot309AtexitRows.size() == 12);
    for (std::size_t i = 0; i < kSlot309AtexitRows.size(); ++i) {
        CHECK(kSlot309AtexitRows[i].step == i + 1);
        CHECK(kSlot309AtexitRows[i].object_rva == want_rva[i]);
        CHECK(kSlot309AtexitRows[i].dtor_rva == want_dtor[i]);
        CHECK(kSlot309AtexitRows[i].site_rva == want_site[i]);
    }
    // Three variable classes: ctor, vptr, functor tag, dtor, and the two 0x26ea0 gate slots.
    CHECK(kSlot309Vars[0].ctor_rva == 0xa4f4d0 && kSlot309Vars[1].ctor_rva == 0xa4fec0 &&
          kSlot309Vars[2].ctor_rva == 0xa50280);
    CHECK(kSlot309Vars[0].vptr_tag == 0x15d1ba8 && kSlot309Vars[1].vptr_tag == 0x15d1c60 &&
          kSlot309Vars[2].vptr_tag == 0x15d1cf8);
    CHECK(kSlot309Vars[0].functor_tag == 0x15d1b78 && kSlot309Vars[1].functor_tag == 0x15d1c30 &&
          kSlot309Vars[2].functor_tag == 0x15d1cc8);
    CHECK(kSlot309Vars[0].max_value == 5 && kSlot309Vars[1].max_value == 0x12 && kSlot309Vars[2].max_value == 0x14);
    CHECK(kSlot309Vars[0].min_value == 0 && kSlot309Vars[1].min_value == 0 && kSlot309Vars[2].min_value == 0);
    CHECK(kSlot309Vars[0].initial_value == 0 && kSlot309Vars[1].initial_value == 0 &&
          kSlot309Vars[2].initial_value == 0);
    for (const auto& v : kSlot309Vars) CHECK(v.flags == 1);
    CHECK(kSlot309Vars[0].name_len == 0x1a && kSlot309Vars[0].description_len == 0x19);
    CHECK(kSlot309Vars[1].name_len == 0x12 && kSlot309Vars[1].description_len == 0x34);
    CHECK(kSlot309Vars[2].name_len == 0x13 && kSlot309Vars[2].description_len == 0x35);
    // functor slot 3 is a bare ret at all three sites, and the three tags are distinct.
    for (const auto& site : kSlot309FunctorSites) CHECK(site.slot3_is_bare_ret);
    CHECK(kSlot309FunctorSites[0].site_rva == 0xa5073c && kSlot309FunctorSites[1].site_rva == 0xa50af4 &&
          kSlot309FunctorSites[2].site_rva == 0xa50c73);
    CHECK(kSlot309Var1FunctorSlot3Rva == 0xa4f4c0 && kSlot309Var2FunctorSlot3Rva == 0xa4feb0 &&
          kSlot309Var3FunctorSlot3Rva == 0xa50270);
    // objects + strides
    CHECK(offsetof(Slot309Objects, var1) == 0x030 && offsetof(Slot309Objects, name_object) == 0x108);
    CHECK(offsetof(Slot309Objects, bool_var) == 0x138 && offsetof(Slot309Objects, empty_view) == 0x208);
    CHECK(offsetof(Slot309Objects, toggle4) == 0x5d0 && sizeof(Slot309Objects) == 0x678);
    CHECK(kSlot309BoolVarStride == 0xd0 && kSlot309MenuItemStride == 0xa8);
    CHECK(std::strcmp(slot309_stage_name(Slot309Stage::none), "none") == 0);
    CHECK(std::strcmp(slot309_stage_name(Slot309Stage::var2_functor), "var2_functor") == 0);
    CHECK(std::strcmp(slot309_stage_name(Slot309Stage::canary), "canary") == 0);
}

void section_functor_and_target() {
    // The lifted `callq *0x18(%rax)`: inert only when the pinned body is a bare ret.
    std::uint8_t functor[0x20]{};
    for (const auto& site : kSlot309FunctorSites) CHECK(slot309_inert_functor_destroy(functor, site));
    Slot309FunctorSite unlifted = kSlot309FunctorSites[0];
    unlifted.slot3_is_bare_ret = false;
    CHECK(!slot309_inert_functor_destroy(functor, unlifted));

    // The 0x26ea0 gate: {1, 1} for the three variable classes, unknown for anything else, {1, 0} bool.
    Slot309VarObject var;
    for (std::uint64_t tag : {kSlot309Var1Vptr, kSlot309Var2Vptr, kSlot309Var3Vptr}) {
        std::memcpy(var.bytes.data(), &tag, sizeof tag);
        const DebugMenuTarget t = slot309_var_menu_target(var);
        CHECK(t.known && t.object == &var && t.slot3 == 1 && t.slot6 == 1);
    }
    const std::uint64_t foreign = 0x1594bf8;
    std::memcpy(var.bytes.data(), &foreign, sizeof foreign);
    CHECK(!slot309_var_menu_target(var).known);

    ConCmdBoolVar b{};
    b.base.vptr = kBoolVarFinalVptr;
    const DebugMenuTarget bt = debug_menu_target_for(b);
    CHECK(bt.known && bt.slot3 == 1 && bt.slot6 == 0);
}

void section_build() {
    g_log = HookLog{};
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot309Objects o;
    poison(o.cell);
    poison(o.cleared);
    poison(o.var1);
    poison(o.var2);
    poison(o.var3);
    poison(o.name_object);
    poison(o.bool_var.gap_c8);
    o.name_object.tail28 = 0xfeedface;

    std::uint64_t guard = 0x1122334455667788ull;
    Slot309Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    hooks.stack_guard = &guard;
    const Slot309Result r = init_slot309_a505e0(o, creg, menu, hooks);

    CHECK(r.ok && r.blocked == Slot309Stage::none);
    CHECK(r.atexit_records == 12 && r.functors_destroyed == 3 && r.vars_built == 3 && r.menu_items == 4);
    CHECK(r.name_object_released && r.views_inert);
    CHECK(r.canary_checked && r.canary_matched);
    CHECK(g_log.calls == 3);
    CHECK(g_log.ctors[0] == 0xa4f4d0 && g_log.ctors[1] == 0xa4fec0 && g_log.ctors[2] == 0xa50280);
    CHECK(g_log.vptrs[0] == 0x15d1ba8 && g_log.vptrs[1] == 0x15d1c60 && g_log.vptrs[2] == 0x15d1cf8);
    CHECK(g_log.functors[0] == 0x15d1b78 && g_log.functors[1] == 0x15d1c30 && g_log.functors[2] == 0x15d1cc8);
    CHECK(g_log.dtors[0] == 0xa47d20 && g_log.dtors[1] == 0xa48240 && g_log.dtors[2] == 0xa4ab50);
    CHECK(g_log.min[0] == 0 && g_log.min[1] == 0 && g_log.min[2] == 0);
    CHECK(g_log.max[0] == 5 && g_log.max[1] == 0x12 && g_log.max[2] == 0x14);
    CHECK(g_log.flags[0] == 1 && g_log.flags[1] == 1 && g_log.flags[2] == 1);

    // 1. cell: 8 bytes written, +0x08 untouched.
    CHECK(o.cell.value == 0);
    CHECK(o.cell.gap08 == 0xa5a5a5a5a5a5a5a5ull);
    // 2. cleared: +0x00..+0x17 zero, +0x18 untouched.
    for (std::uint8_t byte : o.cleared.zero) CHECK(byte == 0);
    CHECK(o.cleared.gap18 == 0xa5a5a5a5a5a5a5a5ull);
    // 3..5. the variable stand-ins keep the class tag and the hook's marker; the rest stays poison.
    const std::uint8_t* v1 = o.var1.bytes.data();
    CHECK(dword_at(o.var1, 0) == 0x15d1ba8u && v1[8] == 0x5A);
    for (std::size_t i = 9; i < kSlot309VarBytes; ++i) CHECK(v1[i] == kPoison);
    // 4. name object: vptr, borrowed view, three zero words.
    CHECK(o.name_object.vptr == kSlot309NameObjectVptr);
    CHECK(same_view(o.name_object.name, "sSaveManagerShowInfoLevel", 0x19));
    CHECK(o.name_object.tail28 == 0);
    // 5. bool variable through the shared native shape.
    const ConCmdBoolVar& bv = o.bool_var.var;
    CHECK(bv.base.vptr == kBoolVarFinalVptr && bv.base.flags == 1 && bv.base.registered == 1);
    CHECK(same_view(bv.base.name, "saveSmallMemory", 0xf));
    CHECK(bv.base.description.size == 0 && bv.base.description.data[0] == 0 && bv.base.description.ctl == nullptr);
    CHECK(bv.value == 0 && bv.default_value == 0 && bv.value_ref == &bv.value);
    CHECK(bv.fn_vptr == kSlot309BoolVarFnTag && bv.fn_payload == 0 && bv.fn_aux == 0);
    CHECK(o.bool_var.gap_c8 == 0xa5a5a5a5a5a5a5a5ull);
    // 6. empty SharedStr through the native 0xf180 model.
    CHECK(o.empty_view.data != nullptr && o.empty_view.size == 0 && o.empty_view.aux == 0 &&
          o.empty_view.ctl == nullptr && o.empty_view.data[0] == 0);
    // 7..12. the four toggles: kind 2, flags 0x30, default range {1.0f, 10.0f}, no narrowing.
    const void* want_target[4] = {&o.var1, &o.bool_var.var, &o.var2, &o.var3};
    const char* want_text[4] = {kSlot309Toggle1Text, kSlot309Toggle2Text, kSlot309Toggle3Text,
                                kSlot309Toggle4Text};
    const DebugMenuItem* items[4] = {&o.toggle1, &o.toggle2, &o.toggle3, &o.toggle4};
    for (int i = 0; i < 4; ++i) {
        const DebugMenuItem& it = *items[i];
        // The root-list path of 0x268b0 links the item but leaves +0x20 alone (only the parent
        // path stores 1 at 0x26a92); the menu root list below is the proof of the link.
        CHECK(it.kind == 2 && it.flags == 0x30 && it.registered == 0);
        CHECK(it.target == want_target[i]);
        CHECK(same_view(it.label, "Save System", 0xb));
        CHECK(same_view(it.text, want_text[i], std::strlen(want_text[i])));
        CHECK(dword_at(it, 0x98) == 0x3f800000u && dword_at(it, 0x9c) == 0x41200000u);
        CHECK(r.toggle_registration[i].ok);
    }
    CHECK(r.toggle_registration[0].root_initialised && r.toggle_registration[0].atexit_recorded);
    CHECK(!r.toggle_registration[1].root_initialised && !r.toggle_registration[2].root_initialised &&
          !r.toggle_registration[3].root_initialised);

    // Registration side effects: one pending ConCmd (the bool variable), four root-list menu items.
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &bv.base.pending);
    CHECK(menu.root_guard_done && menu.root.count == 4);

    // Atexit order: 12 slot records with the debug-menu root's record between 6 and 7.
    CHECK(creg.atexit_log.size() == 13);
    CHECK(creg.atexit_log.at(6).dtor_rva == kSlot309MenuRootDtorRva);
    CHECK(creg.atexit_log.at(6).object == &menu.root);
    StartupAtexitEntry recs[12]{};
    CHECK(slot_records(creg.atexit_log, recs, 12) == 12);
    const void* want_object[12] = {&o.cell,  &o.cleared, &o.var1,  &o.name_object, &bv,       &o.empty_view,
                                   &o.toggle1, &o.toggle2, &o.var2, &o.toggle3,     &o.var3, &o.toggle4};
    for (std::size_t i = 0; i < 12; ++i) {
        CHECK(recs[i].dtor_rva == kSlot309AtexitRows[i].dtor_rva);
        CHECK(recs[i].object == want_object[i]);
    }
}

void section_no_var_hook() {
    // Without the 0xa4f4d0 contract the fragment stops before the first variable: two atexit records,
    // and the variable stand-ins were never touched.
    g_log = HookLog{};
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot309Objects o;
    poison(o.var1);
    poison(o.var2);
    poison(o.var3);
    const Slot309Result r = init_slot309_a505e0(o, creg, menu);
    CHECK(!r.ok && r.blocked == Slot309Stage::var1_ctor);
    CHECK(r.atexit_records == 2 && r.vars_built == 0 && r.menu_items == 0);
    CHECK(creg.atexit_log.size() == 2);
    CHECK(all_poison(o.var1) && all_poison(o.var2) && all_poison(o.var3));
    CHECK(!r.canary_checked && !r.canary_matched);
}

void section_var_hook_fails() {
    g_log = HookLog{};
    g_log.fail_on_call = true;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot309Objects o;
    poison(o.var1);
    Slot309Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    const Slot309Result r = init_slot309_a505e0(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot309Stage::var1_ctor);
    CHECK(r.atexit_records == 2 && r.vars_built == 0);
    // The class tag was stored before the hook (as the original ctor does); a failing hook that writes
    // nothing leaves the rest of the stand-in untouched.
    CHECK(dword_at(o.var1, 0) == 0x15d1ba8u);
    CHECK(o.var1.bytes[8] == kPoison);
}

void section_functor_refused() {
    // A functor whose slot 3 is not the pinned bare ret must not be reported as an inert destroy.
    g_log = HookLog{};
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot309Objects o;
    Slot309Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    hooks.destroy_functor = &refusing_functor;
    const Slot309Result r = init_slot309_a505e0(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot309Stage::var1_functor);
    CHECK(r.vars_built == 1 && r.functors_destroyed == 0 && r.atexit_records == 2);

    // A host hook that accepts the site is used instead of the inert one.
    g_log = HookLog{};
    destroy_calls = false;
    ConCmdRegistry creg2;
    DebugMenuRegistry menu2;
    Slot309Objects o2;
    hooks.destroy_functor = &counting_functor;
    const Slot309Result r2 = init_slot309_a505e0(o2, creg2, menu2, hooks);
    CHECK(r2.ok && destroy_calls && r2.functors_destroyed == 3);
}

void section_concmd_registration_fails() {
    // A non-null manager cell with no insert contract fails the bool variable (0x20d60) visibly.
    g_log = HookLog{};
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    DebugMenuRegistry menu;
    Slot309Objects o;
    poison(o.toggle1);
    Slot309Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    const Slot309Result r = init_slot309_a505e0(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot309Stage::bool_var && !r.bool_var.ok);
    CHECK(r.atexit_records == 4 && r.menu_items == 0);
    CHECK(all_poison(o.toggle1) && menu.root.count == 0);
}

void section_parent_contract() {
    // With a menu parent contract the four toggles go to the parent and the root is never initialised,
    // so the atexit log holds exactly the body's 12 records.
    static int inserted = 0;
    struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
    g_log = HookLog{};
    inserted = 0;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    menu.parent.insert = &P::insert;
    Slot309Objects o;
    Slot309Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    const Slot309Result r = init_slot309_a505e0(o, creg, menu, hooks);
    CHECK(r.ok && inserted == 4 && menu.root.count == 0 && !menu.root_guard_done);
    CHECK(o.toggle1.registered == 1 && o.toggle2.registered == 1 && o.toggle3.registered == 1 &&
          o.toggle4.registered == 1);
    CHECK(creg.atexit_log.size() == 12 && r.atexit_records == 12);
    for (std::size_t i = 0; i < creg.atexit_log.size(); ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == kSlot309AtexitRows[i].dtor_rva);
    }
}

void section_canary() {
    // The body's only branch is the __stack_chk_guard compare. A guard that changed inside the body
    // takes the original's `jne 0xa50d3e` / __stack_chk_fail path, so the model fails at `canary`
    // after every object was built.
    g_log = HookLog{};
    std::uint64_t guard = 0x0123456789abcdefull;
    g_log.guard = &guard;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot309Objects o;
    Slot309Hooks hooks{};
    hooks.var_ctor = &recording_var_ctor;
    hooks.stack_guard = &guard;
    const Slot309Result r = init_slot309_a505e0(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot309Stage::canary);
    CHECK(r.canary_checked && !r.canary_matched);
    CHECK(r.atexit_records == 12 && r.menu_items == 4);
    CHECK(creg.atexit_log.size() == 13);
}

}  // namespace

int main() {
    section_constants();
    section_functor_and_target();
    section_build();
    section_no_var_hook();
    section_var_hook_fails();
    section_functor_refused();
    section_concmd_registration_fails();
    section_parent_contract();
    section_canary();
    std::puts("OK test_ctor_slot_309");
    return 0;
}
