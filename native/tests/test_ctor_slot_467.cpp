// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: fixture for startup constructor index 467 (0x3ca110, the
// `consoleTextScale` float variable plus its "Debug" / "Console Text Scale" slider).
//
// Same body shape as slot 361 with the value stores in rip-relative form; this fixture pins ITS data rows
// (the object addresses, the strings, the IEEE words and the site RVAs), the complete order of effects,
// and the fail-visible stages. Byte-for-byte pinning: tools/lift/check_ctor_slot_467.py.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_467_3ca110.hpp"

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
    CHECK(kSlot467Rva == 0x3ca110 && kSlot467BodySize == 537 && kSlot467EndRva == 0x3ca329);
    CHECK(kSlot467InstructionCount == 107 && kSlot467DirectCallSites == 10 && kSlot467DirectCallees == 6);
    CHECK(kSlot467FunctorSites == 1 && kSlot467BranchCount == 1);
    CHECK(kSlot467TableTopRva - 8 * kSlot467TableIndex == kSlot467TableWordRva && kSlot467TableWordRva == 0x2dd6248);
    CHECK(kSlot467BranchRva == 0x3ca312 && kSlot467BranchTargetRva == 0x3ca323);

    CHECK(kSlot467Vars.size() == 1 && kSlot467Items.size() == 1);
    const debug_page::FloatVarRow& v = kSlot467Vars[0];
    CHECK(v.object_rva == 0x2ef6e08 && v.object_rva + debug_page::kVarStride == kSlot467ItemRva);
    CHECK(kSlot467ItemRva == 0x2ef6ee0);
    CHECK(std::strcmp(v.name, "consoleTextScale") == 0 && v.name_len == 0x10);
    CHECK(std::strcmp(v.description, "") == 0 && v.description_len == 0x00);
    CHECK(v.lo_bits == 0x3dcccccd && v.hi_bits == 0x41200000 && v.value_bits == 0x3f800000);
    CHECK(v.arg_fn_vtbl == 0x15b0e90 && debug_page::is_batch_float_site_tag(v.arg_fn_vtbl));
    CHECK(v.ctor_site_rva == 0x3ca209 && v.functor_site_rva == 0x3ca225 && v.register_site_rva == 0x3ca246 &&
          v.atexit_site_rva == 0x3ca267);
    const debug_page::MenuRow& m = kSlot467Items[0];
    CHECK(m.kind == debug_page::MenuKind::slider_27050 && m.object_rva == 0x2ef6ee0 && m.target_var == 0);
    CHECK(std::strcmp(m.label, "Debug") == 0 && m.label_len == 5);
    CHECK(std::strcmp(m.text, "Console Text Scale") == 0 && m.text_len == 0x12);
    CHECK(m.range_min == 0.1f && m.range_max == 1.0f);
    CHECK(m.ctor_site_rva == 0x3ca2d9 && m.atexit_site_rva == 0x3ca2fb);
    CHECK(offsetof(Slot467Objects, item) == 0xd8 && sizeof(Slot467Objects) == 0x180);
}

void section_native_build() {
    Slot467Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const debug_page::Result r = init_slot467_3ca110(objects, concmd, menu);

    CHECK(r.ok && r.blocked == debug_page::Stage::none);
    CHECK(r.vars_built == 1 && r.items_built == 1 && r.atexit_records == 2 && r.functors_destroyed == 1);
    CHECK(r.vars[0].ok && r.items[0].ok && !r.canary_checked);

    const ConCmdRangedVar& var = objects.var;
    CHECK(var.base.vptr == 0x15942b0 && var.base.registered == 1 && var.base.flags == 1);
    CHECK(var.value_ref == &var.value && var.min_engaged == 1 && var.max_engaged == 1);
    std::uint32_t lo = 0, hi = 0;
    std::memcpy(&lo, var.min_value, 4);
    std::memcpy(&hi, var.max_value, 4);
    CHECK(lo == 0x3dcccccd && hi == 0x41200000);
    CHECK(var.value == 0x3f800000u && var.default_value == 0x3f800000u);
    CHECK(var.fn_vptr == kRangedFloatFnVtbl && var.fn_payload == 0 && var.fn_aux == 0);
    for (std::uint64_t q : var.args) CHECK(q == 0);
    CHECK(same_view(var.base.name, "consoleTextScale", 0x10));
    CHECK(same_view(var.base.description, "", 0));

    const DebugMenuItem& item = objects.item;
    CHECK(item.target == &var);
    CHECK(same_view(item.label, "Debug", 5));
    CHECK(same_view(item.text, "Console Text Scale", 0x12));
    CHECK(item.kind == 2 && item.flags == 0x30);
    // 0x268b0's root-list path links the item but does not set +0x20 (only the parent path does).
    CHECK(item.registered == 0);
    CHECK(item.range_min == 0.1f && item.range_max == 1.0f);
    CHECK(item.link.owner == &menu.root.link && menu.root.count == 1);

    StartupAtexitEntry records[8];
    const std::size_t n = body_records(concmd.atexit_log, records, 8);
    CHECK(n == 2);
    CHECK(records[0].dtor_rva == 0x20f0 && records[0].object == &objects.var);
    CHECK(records[1].dtor_rva == 0x272d0 && records[1].object == &objects.item);
    CHECK(concmd.pending.size() == 1);
}

void section_unpinned_tag() {
    Slot467Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::FloatVarRow bad = kSlot467Vars[0];
    // A tag nobody proved: the model must refuse it (a proved-equivalent tag would be accepted, which is
    // why the ROW itself is pinned by the checker as well).
    bad.arg_fn_vtbl = 0x15dead0;
    const debug_page::FloatVarRow rows[1] = {bad};
    debug_page::Script script = kSlot467Script;
    script.vars = std::span<const debug_page::FloatVarRow>(rows, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_ctor && r.vars_built == 0);
    CHECK(r.atexit_records == 0 && concmd.atexit_log.size() == 0);
    CHECK(region_poison(objects, 0, sizeof objects));
}

void section_shape_refused() {
    Slot467Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::MenuRow bad = kSlot467Items[0];
    bad.target_var = 3;
    const debug_page::MenuRow items[1] = {bad};
    debug_page::Script script = kSlot467Script;
    script.items = std::span<const debug_page::MenuRow>(items, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::shape && r.atexit_records == 0);
    CHECK(region_poison(objects, 0, sizeof objects));
}

void section_register_refused() {
    Slot467Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int manager_token = 0;
    concmd.manager.manager = &manager_token;

    const debug_page::Result r = init_slot467_3ca110(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_register && r.vars_built == 0);
    CHECK(r.vars[0].ctor_ok && !r.vars[0].registration.ok);
    CHECK(r.atexit_records == 0 && concmd.pending.size() == 0);
}

void section_item_register_refused() {
    Slot467Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int parent_token = 0;
    menu.parent.parent = &parent_token;

    const debug_page::Result r = init_slot467_3ca110(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::item_register && r.items_built == 0);
    CHECK(r.vars_built == 1 && r.atexit_records == 1);
    CHECK(r.items[0].registration.path == DebugMenuRegisterPath::parent);

    Slot467Objects objects2;
    ConCmdRegistry concmd2;
    DebugMenuRegistry menu2;
    menu2.parent.parent = &parent_token;
    menu2.parent.insert = &parent_insert_ok;
    const debug_page::Result r2 = init_slot467_3ca110(objects2, concmd2, menu2);
    CHECK(r2.ok && objects2.item.registered == 1 && menu2.root.count == 0);
}

void section_canary() {
    std::uint64_t guard = 0xfedcba9876543210ull;
    guard_cell = &guard;
    {
        Slot467Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        const debug_page::Result r = init_slot467_3ca110(objects, concmd, menu, {&guard});
        CHECK(r.ok && r.canary_checked && r.canary_matched);
    }
    {
        Slot467Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        int parent_token = 0;
        menu.parent.parent = &parent_token;
        menu.parent.insert = &parent_insert_flips_guard;
        const debug_page::Result r = init_slot467_3ca110(objects, concmd, menu, {&guard});
        CHECK(!r.ok && r.blocked == debug_page::Stage::canary && !r.canary_matched);
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
    std::printf("OK test_ctor_slot_467\n");
    return 0;
}
