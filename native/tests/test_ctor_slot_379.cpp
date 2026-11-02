// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: fixture for startup constructor index 379 (0x6d8d20, the
// `shadow_buddhaBossPlayerOutsideAttackRadius` float variable plus its "Shadow AI" / "Buddha Boss Player
// Outside Attack Radius" TOGGLE).
//
// The toggle is the point of this fixture: 0x26ea0 reads two virtual slots of its target, and the class
// the row targets (the float class 0x15942b0) answers {1, 3}, so the item range must be the narrowed
// {0.1, 0.5} and NOT the constructor default {1.0, 10.0}. The row's own range words are zero and unused.
// Fail-visible: an unpinned caller tag, a shape mismatch, a refused 0x20d60 / 0x268b0 and a changed guard
// — plus a target class that is not the float class, which must block the item.
// Byte-for-byte pinning: tools/lift/check_ctor_slot_379.py.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_379_6d8d20.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

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
    (void)item;
    return true;
}

bool parent_insert_flips_guard(void* parent, DebugMenuItem& item) noexcept {
    (void)parent;
    (void)item;
    if (guard_cell != nullptr) *guard_cell ^= 1u;
    return true;
}

void section_constants() {
    CHECK(kSlot379Rva == 0x6d8d20 && kSlot379BodySize == 512 && kSlot379EndRva == 0x6d8f20);
    CHECK(kSlot379InstructionCount == 105 && kSlot379DirectCallSites == 10 && kSlot379DirectCallees == 6);
    CHECK(kSlot379FunctorSites == 1 && kSlot379BranchCount == 1);
    CHECK(kSlot379TableTopRva - 8 * kSlot379TableIndex == kSlot379TableWordRva && kSlot379TableWordRva == 0x2dd6508);
    CHECK(kSlot379BranchRva == 0x6d8f09 && kSlot379BranchTargetRva == 0x6d8f1a);

    CHECK(kSlot379Vars.size() == 1 && kSlot379Items.size() == 1);
    const debug_page::FloatVarRow& v = kSlot379Vars[0];
    CHECK(v.object_rva == 0x323be40 && v.object_rva + debug_page::kVarStride == kSlot379ItemRva);
    CHECK(kSlot379ItemRva == 0x323bf18);
    CHECK(std::strcmp(v.name, "shadow_buddhaBossPlayerOutsideAttackRadius") == 0 && v.name_len == 0x2a);
    CHECK(std::strcmp(v.description, "") == 0 && v.description_len == 0x00);
    CHECK(v.lo_bits == 0x00000000u && v.hi_bits == 0x42c80000u && v.value_bits == 0x41700000u);
    CHECK(v.arg_fn_vtbl == 0x15cc898 && debug_page::is_batch_float_site_tag(v.arg_fn_vtbl));
    CHECK(v.ctor_site_rva == 0x6d8e15 && v.functor_site_rva == 0x6d8e31 && v.register_site_rva == 0x6d8e4d &&
          v.atexit_site_rva == 0x6d8e6e);
    const debug_page::MenuRow& m = kSlot379Items[0];
    CHECK(m.kind == debug_page::MenuKind::toggle_26ea0 && m.object_rva == 0x323bf18 && m.target_var == 0);
    CHECK(std::strcmp(m.label, "Shadow AI") == 0 && m.label_len == 9);
    CHECK(std::strcmp(m.text, "Buddha Boss Player Outside Attack Radius") == 0 && m.text_len == 0x28);
    CHECK(m.range_min == 0.0f && m.range_max == 0.0f);   // unused: the toggle reads the class gate
    CHECK(m.ctor_site_rva == 0x6d8ed0 && m.atexit_site_rva == 0x6d8ef2);
    // The gate answer the model uses.
    CHECK(debug_page::kFloatClassVptr == 0x15942b0 && debug_page::kFloatClassSlot3 == 1 &&
          debug_page::kFloatClassSlot6 == 3);
    CHECK(debug_page::kFloatClassSlot3Rva == 0xbba0 && debug_page::kFloatClassSlot6Rva == 0xbbf0);
    CHECK(kSlot379ToggleLoBits == 0x3dcccccd && kSlot379ToggleHiBits == 0x3f000000);
}

void section_native_build() {
    Slot379Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const debug_page::Result r = init_slot379_6d8d20(objects, concmd, menu);

    CHECK(r.ok && r.blocked == debug_page::Stage::none);
    CHECK(r.vars_built == 1 && r.items_built == 1 && r.atexit_records == 2 && r.functors_destroyed == 1);
    CHECK(r.vars[0].ok && r.items[0].ok && !r.canary_checked);

    const ConCmdRangedVar& var = objects.var;
    CHECK(var.base.vptr == 0x15942b0 && var.base.registered == 1 && var.base.flags == 1);
    CHECK(var.value_ref == &var.value && var.min_engaged == 1 && var.max_engaged == 1);
    std::uint32_t lo = 0, hi = 0;
    std::memcpy(&lo, var.min_value, 4);
    std::memcpy(&hi, var.max_value, 4);
    CHECK(lo == 0x00000000u && hi == 0x42c80000u);
    CHECK(var.value == 0x41700000u && var.default_value == 0x41700000u);
    CHECK(same_view(var.base.name, "shadow_buddhaBossPlayerOutsideAttackRadius", 0x2a));

    const DebugMenuItem& item = objects.item;
    CHECK(item.target == &var);
    CHECK(same_view(item.label, "Shadow AI", 9));
    CHECK(same_view(item.text, "Buddha Boss Player Outside Attack Radius", 0x28));
    CHECK(item.kind == 2 && item.flags == 0x30);
    // 0x268b0's root-list path links the item but does not set +0x20 (only the parent path does).
    CHECK(item.registered == 0);
    // THE toggle gate: the float class answers {1, 3}, so the range narrowed to {0.1, 0.5}.
    std::uint32_t lo_bits = 0, hi_bits = 0;
    std::memcpy(&lo_bits, &item.range_min, 4);
    std::memcpy(&hi_bits, &item.range_max, 4);
    CHECK(lo_bits == 0x3dcccccd && hi_bits == 0x3f000000);
    CHECK(item.link.owner == &menu.root.link && menu.root.count == 1);

    StartupAtexitEntry records[8];
    const std::size_t n = body_records(concmd.atexit_log, records, 8);
    CHECK(n == 2);
    CHECK(records[0].dtor_rva == 0x20f0 && records[0].object == &objects.var);
    CHECK(records[1].dtor_rva == 0x272d0 && records[1].object == &objects.item);
    CHECK(concmd.pending.size() == 1);
}

void section_unpinned_tag() {
    Slot379Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::FloatVarRow bad = kSlot379Vars[0];
    bad.arg_fn_vtbl = 0x15dead0;
    const debug_page::FloatVarRow rows[1] = {bad};
    debug_page::Script script = kSlot379Script;
    script.vars = std::span<const debug_page::FloatVarRow>(rows, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_ctor && r.atexit_records == 0);
    CHECK(region_poison(objects, 0, sizeof objects));
}

void section_kind_dispatch() {
    // The row's `kind` selects the constructor: the same script with a slider row must use the row's
    // explicit range instead of the class gate. (A toggle can only ever follow its own variable row, so
    // the "target class not the float class" refusal is defensive and not reachable from pinned data.)
    Slot379Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::MenuRow row = kSlot379Items[0];
    row.kind = debug_page::MenuKind::slider_27050;
    row.range_min = 2.5f;
    row.range_max = 7.5f;
    const debug_page::MenuRow rows[1] = {row};
    debug_page::Script script = kSlot379Script;
    script.items = std::span<const debug_page::MenuRow>(rows, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(r.ok && r.items_built == 1);
    std::uint32_t lo_bits = 0, hi_bits = 0;
    std::memcpy(&lo_bits, &objects.item.range_min, 4);
    std::memcpy(&hi_bits, &objects.item.range_max, 4);
    CHECK(lo_bits == 0x40200000 && hi_bits == 0x40f00000);   // 2.5f / 7.5f from the row
}

void section_shape_refused() {
    Slot379Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::MenuRow bad = kSlot379Items[0];
    bad.target_var = 2;
    const debug_page::MenuRow items[1] = {bad};
    debug_page::Script script = kSlot379Script;
    script.items = std::span<const debug_page::MenuRow>(items, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::shape);
    CHECK(region_poison(objects, 0, sizeof objects));
}

void section_register_refused() {
    Slot379Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int manager_token = 0;
    concmd.manager.manager = &manager_token;

    const debug_page::Result r = init_slot379_6d8d20(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_register && r.vars_built == 0);
    CHECK(r.atexit_records == 0);
}

void section_item_register_refused() {
    Slot379Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int parent_token = 0;
    menu.parent.parent = &parent_token;

    const debug_page::Result r = init_slot379_6d8d20(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::item_register && r.items_built == 0);
    CHECK(r.vars_built == 1 && r.atexit_records == 1);

    Slot379Objects objects2;
    ConCmdRegistry concmd2;
    DebugMenuRegistry menu2;
    menu2.parent.parent = &parent_token;
    menu2.parent.insert = &parent_insert_ok;
    const debug_page::Result r2 = init_slot379_6d8d20(objects2, concmd2, menu2);
    CHECK(r2.ok && objects2.item.registered == 1);   // the parent path sets +0x20
}

void section_canary() {
    std::uint64_t guard = 0x0f1e2d3c4b5a6978ull;
    guard_cell = &guard;
    {
        Slot379Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        const debug_page::Result r = init_slot379_6d8d20(objects, concmd, menu, {&guard});
        CHECK(r.ok && r.canary_checked && r.canary_matched);
    }
    {
        Slot379Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        int parent_token = 0;
        menu.parent.parent = &parent_token;
        menu.parent.insert = &parent_insert_flips_guard;
        const debug_page::Result r = init_slot379_6d8d20(objects, concmd, menu, {&guard});
        CHECK(!r.ok && r.blocked == debug_page::Stage::canary && !r.canary_matched);
    }
    guard_cell = nullptr;
}

}  // namespace

int main() {
    section_constants();
    section_native_build();
    section_unpinned_tag();
    section_kind_dispatch();
    section_shape_refused();
    section_register_refused();
    section_item_register_refused();
    section_canary();
    std::printf("OK test_ctor_slot_379\n");
    return 0;
}
