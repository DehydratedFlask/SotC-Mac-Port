// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: fixture for startup constructor index 361 (0x777390, the
// `shadow_snakeBossDiveLoopLength` float variable plus its "Shadow AI" / "Snake Boss Dive Loop Audio
// Length" slider).
//
// Covers: the pinned data rows (object addresses, strides, strings + lengths, IEEE words, site RVAs), the
// complete original order of effects, the two __cxa_atexit records of the body, and every fail-visible
// stage this body can reach: an unpinned caller argument-functor tag, an item row that names no variable,
// a refused 0x20d60 registration, a refused 0x268b0 registration and a changed __stack_chk_guard.
// The body is ALSO pinned byte-for-byte by tools/lift/check_ctor_slot_361.py.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_361_777390.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

// The guard cell the canary section corrupts from inside the menu parent contract.
std::uint64_t* guard_cell = nullptr;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
bool region_poison(const T& obj, std::size_t off, std::size_t len) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < len; ++i) if (b[off + i] != kPoison) return false;
    return true;
}

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 && v.ctl == nullptr &&
           std::memcmp(v.data, text, len) == 0;
}

// The body's own atexit records, skipping the one 0x268b0 records for the menu root.
std::size_t body_records(const StartupAtexitLog& log, StartupAtexitEntry* out, std::size_t cap) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < log.size(); ++i) {
        const StartupAtexitEntry& e = log.at(i);
        if (e.dtor_rva == kDebugMenuRootDtorRva) continue;
        if (n < cap) out[n] = e;
        ++n;
    }
    return n;
}

bool parent_insert_ok(void* parent, DebugMenuItem& item) noexcept {
    (void)parent;
    (void)item;   // the model marks the item after the contract returns
    return true;
}

bool parent_insert_flips_guard(void* parent, DebugMenuItem& item) noexcept {
    (void)parent;
    (void)item;
    if (guard_cell != nullptr) *guard_cell ^= 1u;
    return true;
}

void section_constants() {
    CHECK(kSlot361Rva == 0x777390 && kSlot361BodySize == 528 && kSlot361EndRva == 0x7775a0);
    CHECK(kSlot361InstructionCount == 107 && kSlot361DirectCallSites == 10 && kSlot361DirectCallees == 6);
    CHECK(kSlot361FunctorSites == 1 && kSlot361BranchCount == 1);
    // 0x2dd70e0 - 361*8 == 0x2dd6598, the one word that references the body.
    CHECK(kSlot361TableTopRva - 8 * kSlot361TableIndex == kSlot361TableWordRva && kSlot361TableWordRva == 0x2dd6598);
    CHECK(kSlot361CanaryCellRva == 0x15fe518 && kSlot361CanaryFrameOffset == 0xc0);
    CHECK(kSlot361BranchRva == 0x777589 && kSlot361BranchTargetRva == 0x77759a);

    // The two data rows ARE the body: object addresses, strides, strings, IEEE words and site RVAs.
    CHECK(kSlot361Vars.size() == 1 && kSlot361Items.size() == 1);
    const debug_page::FloatVarRow& v = kSlot361Vars[0];
    CHECK(v.object_rva == 0x3254fb0 && v.object_rva + debug_page::kVarStride == kSlot361ItemRva);
    CHECK(kSlot361ItemRva == 0x3255088);
    CHECK(std::strcmp(v.name, "shadow_snakeBossDiveLoopLength") == 0 && v.name_len == 0x1e);
    CHECK(std::strcmp(v.description, "") == 0 && v.description_len == 0x00);
    CHECK(v.lo_bits == 0x00000000u && v.hi_bits == 0x41f00000u && v.value_bits == 0x41600000u);
    CHECK(v.arg_fn_vtbl == 0x15cda18 && debug_page::is_batch_float_site_tag(v.arg_fn_vtbl));
    CHECK(v.ctor_site_rva == 0x777485 && v.functor_site_rva == 0x7774a1 && v.register_site_rva == 0x7774bd &&
          v.atexit_site_rva == 0x7774de);
    const debug_page::MenuRow& m = kSlot361Items[0];
    CHECK(m.kind == debug_page::MenuKind::slider_27050 && m.object_rva == 0x3255088 && m.target_var == 0);
    CHECK(std::strcmp(m.label, "Shadow AI") == 0 && m.label_len == 9);
    CHECK(std::strcmp(m.text, "Snake Boss Dive Loop Audio Length") == 0 && m.text_len == 0x21);
    CHECK(m.range_min == 0.1f && m.range_max == 1.0f);
    CHECK(m.ctor_site_rva == 0x777550 && m.atexit_site_rva == 0x777572);
    CHECK(debug_page::kFloatClassVptr == 0x15942b0 && debug_page::kFloatVarDtorRva == 0x20f0);
    // Strides of the stand-ins.
    CHECK(sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot361Objects, item) == 0xd8 && sizeof(Slot361Objects) == 0x180);
}

void section_native_build() {
    Slot361Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const debug_page::Result r = init_slot361_777390(objects, concmd, menu);

    CHECK(r.ok && r.blocked == debug_page::Stage::none);
    CHECK(r.vars_built == 1 && r.items_built == 1 && r.atexit_records == 2 && r.functors_destroyed == 1);
    CHECK(r.vars[0].ok && r.vars[0].ctor_ok && r.vars[0].functor_destroy_ok && r.vars[0].views_inert &&
          r.vars[0].atexit_recorded);
    CHECK(r.items[0].ok && r.items[0].ctor_ok && r.items[0].views_inert && r.items[0].atexit_recorded);
    CHECK(!r.canary_checked);   // no host guard supplied => the comparison is not claimed

    // The variable: class tags, both bounds engaged, value and default, registration.
    const ConCmdRangedVar& var = objects.var;
    CHECK(var.base.vptr == 0x15942b0 && var.base.registered == 1 && var.base.flags == 1);
    CHECK(var.value_ref == &var.value);
    CHECK(var.min_engaged == 1 && var.max_engaged == 1);
    std::uint32_t lo = 0, hi = 0;
    std::memcpy(&lo, var.min_value, 4);
    std::memcpy(&hi, var.max_value, 4);
    CHECK(lo == 0x00000000u && hi == 0x41f00000u);
    CHECK(var.value == 0x41600000u && var.default_value == 0x41600000u);
    CHECK(var.fn_vptr == kRangedFloatFnVtbl && var.fn_payload == 0 && var.fn_aux == 0);
    for (std::uint64_t q : var.args) CHECK(q == 0);   // q[3] == 0 => no self-pointer fix-up
    CHECK(same_view(var.base.name, "shadow_snakeBossDiveLoopLength", 0x1e));
    CHECK(same_view(var.base.description, "", 0));

    // The slider: label, text, target, explicit range, flags, registration.
    const DebugMenuItem& item = objects.item;
    CHECK(item.target == &var);
    CHECK(same_view(item.label, "Shadow AI", 9));
    CHECK(same_view(item.text, "Snake Boss Dive Loop Audio Length", 0x21));
    CHECK(item.kind == 2 && item.flags == 0x30);
    // 0x268b0's root-list path links the item but does not set +0x20 (only the parent path does).
    CHECK(item.registered == 0);
    CHECK(item.range_min == 0.1f && item.range_max == 1.0f);
    CHECK(item.link.self == &item.link && item.link.owner == &menu.root.link);
    CHECK(menu.root.count == 1);
    // The item's +0x70 empty view went through the native 0xf180 model.
    CHECK(item.empty.ctl == nullptr && item.empty.size == 0);

    // The body's two __cxa_atexit records, in body order.
    StartupAtexitEntry records[8];
    const std::size_t n = body_records(concmd.atexit_log, records, 8);
    CHECK(n == 2);
    CHECK(records[0].dtor_rva == 0x20f0 && records[0].object == &objects.var);
    CHECK(records[1].dtor_rva == 0x272d0 && records[1].object == &objects.item);
    CHECK(concmd.pending.size() == 1);
    CHECK(concmd.pending.first() == &var.base.pending);
}

void section_unpinned_tag() {
    // A caller argument-functor tag that no checker proved is refused BEFORE anything is written.
    Slot361Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::FloatVarRow bad = kSlot361Vars[0];
    bad.arg_fn_vtbl = 0x15dead0;   // not this site's pinned tag and not a proved-equivalent one
    const debug_page::FloatVarRow rows[1] = {bad};
    debug_page::Script script = kSlot361Script;
    script.vars = std::span<const debug_page::FloatVarRow>(rows, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_ctor && r.vars_built == 0 && r.items_built == 0);
    CHECK(r.atexit_records == 0 && concmd.atexit_log.size() == 0);
    CHECK(region_poison(objects, 0, sizeof objects));

    // A second unpinned tag, to prove the refusal is not a single-value special case.
    debug_page::FloatVarRow unknown = kSlot361Vars[0];
    unknown.arg_fn_vtbl = 0x15d0f00;
    const debug_page::FloatVarRow rows2[1] = {unknown};
    script.vars = std::span<const debug_page::FloatVarRow>(rows2, 1);
    const debug_page::Result r2 = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r2.ok && r2.blocked == debug_page::Stage::var_ctor);
    CHECK(region_poison(objects, 0, sizeof objects));
}

void section_shape_refused() {
    Slot361Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    // An item row that names a variable this script does not have.
    debug_page::MenuRow bad = kSlot361Items[0];
    bad.target_var = 1;
    const debug_page::MenuRow items[1] = {bad};
    debug_page::Script script = kSlot361Script;
    script.items = std::span<const debug_page::MenuRow>(items, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::shape && r.atexit_records == 0);
    CHECK(region_poison(objects, 0, sizeof objects));

    // A script with more rows than the host's stand-ins.
    const debug_page::Objects empty{std::span<ConCmdRangedVar>(), std::span<DebugMenuItem>()};
    const debug_page::Result r2 = debug_page::init_debug_page(kSlot361Script, empty, concmd, menu, {});
    CHECK(!r2.ok && r2.blocked == debug_page::Stage::shape);
}

void section_register_refused() {
    // A non-null manager without the insert contract is a visible failure, never a silent success.
    Slot361Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int manager_token = 0;
    concmd.manager.manager = &manager_token;   // insert stays null

    const debug_page::Result r = init_slot361_777390(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_register && r.vars_built == 0);
    CHECK(r.vars[0].ctor_ok && r.vars[0].functor_destroy_ok && !r.vars[0].registration.ok);
    CHECK(r.atexit_records == 0 && concmd.atexit_log.size() == 0);
    CHECK(concmd.pending.size() == 0);   // the manager path never queues
}

void section_item_register_refused() {
    // A non-null parent without the insert contract blocks the item after the variable is complete.
    Slot361Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int parent_token = 0;
    menu.parent.parent = &parent_token;

    const debug_page::Result r = init_slot361_777390(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::item_register);
    CHECK(r.vars_built == 1 && r.items_built == 0);
    CHECK(r.items[0].ctor_ok && !r.items[0].registration.ok);
    CHECK(r.items[0].registration.path == DebugMenuRegisterPath::parent);
    CHECK(r.atexit_records == 1);   // the variable's own record was already made

    // With the contract installed the same parent path succeeds and the item registers there.
    Slot361Objects objects2;
    poison(objects2);
    ConCmdRegistry concmd2;
    DebugMenuRegistry menu2;
    menu2.parent.parent = &parent_token;
    menu2.parent.insert = &parent_insert_ok;
    const debug_page::Result r2 = init_slot361_777390(objects2, concmd2, menu2);
    CHECK(r2.ok && r2.items_built == 1 && objects2.item.registered == 1);   // parent path sets +0x20
    CHECK(objects2.item.link.owner == nullptr);   // the parent path does not use the root list
    CHECK(menu2.root.count == 0);
}

void section_canary() {
    // A host-supplied guard that does not change: the comparison is performed and matches.
    std::uint64_t guard = 0x0123456789abcdefull;
    guard_cell = &guard;
    {
        Slot361Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        const debug_page::Result r = init_slot361_777390(objects, concmd, menu, {&guard});
        CHECK(r.ok && r.canary_checked && r.canary_matched);
    }
    {
        // The parent contract flips the guard mid-body; the epilogue compare must fail at `canary`.
        Slot361Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        int parent_token = 0;
        menu.parent.parent = &parent_token;
        menu.parent.insert = &parent_insert_flips_guard;
        const debug_page::Result r = init_slot361_777390(objects, concmd, menu, {&guard});
        CHECK(!r.ok && r.blocked == debug_page::Stage::canary && r.canary_checked && !r.canary_matched);
    }
    guard_cell = nullptr;
}

}  // namespace

int main() {
    section_constants();
    section_native_build();
    section_unpinned_tag();
    section_shape_refused();
    section_register_refused();
    section_item_register_refused();
    section_canary();
    std::printf("OK test_ctor_slot_361\n");
    return 0;
}
