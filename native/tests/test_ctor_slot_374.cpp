// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: fixture for startup constructor index 374 (0x701230, the
// `shadow_prayTime` float variable plus its "Save System" / "Shadow Prayer Time (s)" toggle, and the
// LEADING `{name, fnv1a(name)}` record that no other member of the batch has).
//
// Two things are the point of this fixture:
//   * the record is written BEFORE the first variable row (`[cell] = "IsPraying"` then
//     `[cell+8] = 0x52a990("IsPraying") = 0x411fa39f`), so a refused variable row still leaves the
//     record written — the original's stop-at-failure order;
//   * the toggle targets the float class 0x15942b0, whose gate answers {1, 3}, so the item range must be
//     the narrowed {0.1, 0.5} and not the constructor default {1.0, 10.0}.
// Byte-for-byte pinning: tools/lift/check_ctor_slot_374.py.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_374_701230.hpp"
#include "core/fnv_cstr_161fb0.hpp"

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
    CHECK(kSlot374Rva == 0x701230 && kSlot374BodySize == 537 && kSlot374EndRva == 0x701449);
    CHECK(kSlot374InstructionCount == 110 && kSlot374DirectCallSites == 11 && kSlot374DirectCallees == 7);
    CHECK(kSlot374FunctorSites == 1 && kSlot374BranchCount == 1);
    CHECK(kSlot374TableTopRva - 8 * kSlot374TableIndex == kSlot374TableWordRva && kSlot374TableWordRva == 0x2dd6530);
    CHECK(kSlot374BranchRva == 0x701432 && kSlot374BranchTargetRva == 0x701443);

    // The leading record IS the body's first step: address, name, both sites and the 0x52a990 result.
    CHECK(kSlot374HashedNames.size() == 1 && kSlot374Vars.size() == 1 && kSlot374Items.size() == 1);
    const debug_page::HashedNameRow& h = kSlot374HashedNames[0];
    CHECK(h.cell_rva == 0x3244978 && std::strcmp(h.name, "IsPraying") == 0 && h.name_len == 0x09);
    CHECK(h.pointer_site_rva == 0x701268 && h.hash_site_rva == 0x70126b);
    CHECK(debug_page::kNameHashCallRva == 0x52a990);
    CHECK(debug_page::kMaxScriptHashedNames == 1);
    // The pinned hash is what the native 0x52a990 model returns for the pinned name.
    CHECK(fnv_cstr_52a990(kSlot374RecordName) == kSlot374RecordHash && kSlot374RecordHash == 0x411fa39fu);

    const debug_page::FloatVarRow& v = kSlot374Vars[0];
    CHECK(v.object_rva == 0x3244988 && v.object_rva == kSlot374RecordRva + 0x10);
    CHECK(v.object_rva + debug_page::kVarStride == kSlot374ItemRva && kSlot374ItemRva == 0x3244a60);
    CHECK(std::strcmp(v.name, "shadow_prayTime") == 0 && v.name_len == 0x0f);
    CHECK(std::strcmp(v.description, "") == 0 && v.description_len == 0x00);
    CHECK(v.lo_bits == 0x00000000u && v.hi_bits == 0x41200000u && v.value_bits == 0x40000000u);
    CHECK(v.arg_fn_vtbl == 0x15cccf0 && debug_page::is_batch_float_site_tag(v.arg_fn_vtbl));
    CHECK(v.ctor_site_rva == 0x70133e && v.functor_site_rva == 0x70135a && v.register_site_rva == 0x701376 &&
          v.atexit_site_rva == 0x701397);
    const debug_page::MenuRow& m = kSlot374Items[0];
    CHECK(m.kind == debug_page::MenuKind::toggle_26ea0 && m.object_rva == 0x3244a60 && m.target_var == 0);
    CHECK(std::strcmp(m.label, "Save System") == 0 && m.label_len == 0x0b);
    CHECK(std::strcmp(m.text, "Shadow Prayer Time (s)") == 0 && m.text_len == 0x16);
    CHECK(m.range_min == 0.0f && m.range_max == 0.0f);   // unused: the toggle reads the class gate
    CHECK(m.ctor_site_rva == 0x7013f9 && m.atexit_site_rva == 0x70141b);
    CHECK(kSlot374ToggleLoBits == 0x3dcccccd && kSlot374ToggleHiBits == 0x3f000000);

    // Stand-in strides: record 0x10, variable 0xd8, item 0xa8, in that order.
    CHECK(sizeof(NameHashCell) == 0x10 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot374Objects, record) == 0 && offsetof(Slot374Objects, var) == 0x10 &&
          offsetof(Slot374Objects, item) == 0xe8 && sizeof(Slot374Objects) == 0x190);
}

void section_native_build() {
    Slot374Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const debug_page::Result r = init_slot374_701230(objects, concmd, menu);

    CHECK(r.ok && r.blocked == debug_page::Stage::none);
    CHECK(r.hashed_names_built == 1 && r.vars_built == 1 && r.items_built == 1);
    CHECK(r.atexit_records == 2 && r.functors_destroyed == 1);
    CHECK(r.vars[0].ok && r.items[0].ok && !r.canary_checked);

    // The record: pointer stored, hash computed by the native 0x52a990 model, +0x0c never written.
    CHECK(objects.record.name == kSlot374RecordName);
    CHECK(objects.record.hash == kSlot374RecordHash);
    CHECK(region_poison(objects.record, 0x0c, 4));

    const ConCmdRangedVar& var = objects.var;
    CHECK(var.base.vptr == 0x15942b0 && var.base.registered == 1 && var.base.flags == 1);
    CHECK(var.value_ref == &var.value && var.min_engaged == 1 && var.max_engaged == 1);
    std::uint32_t lo = 0, hi = 0;
    std::memcpy(&lo, var.min_value, 4);
    std::memcpy(&hi, var.max_value, 4);
    CHECK(lo == 0x00000000u && hi == 0x41200000u);
    CHECK(var.value == 0x40000000u && var.default_value == 0x40000000u);
    CHECK(var.fn_vptr == kRangedFloatFnVtbl);
    for (std::uint64_t q : var.args) CHECK(q == 0);
    CHECK(same_view(var.base.name, "shadow_prayTime", 0x0f));
    CHECK(same_view(var.base.description, "", 0));

    const DebugMenuItem& item = objects.item;
    CHECK(item.target == &var);
    CHECK(same_view(item.label, "Save System", 0x0b));
    CHECK(same_view(item.text, "Shadow Prayer Time (s)", 0x16));
    CHECK(item.kind == 2 && item.flags == 0x30);
    CHECK(item.registered == 0);   // 0x268b0's root-list path does not set +0x20
    std::uint32_t lo_bits = 0, hi_bits = 0;
    std::memcpy(&lo_bits, &item.range_min, 4);
    std::memcpy(&hi_bits, &item.range_max, 4);
    CHECK(lo_bits == kSlot374ToggleLoBits && hi_bits == kSlot374ToggleHiBits);
    CHECK(item.link.owner == &menu.root.link && menu.root.count == 1);

    StartupAtexitEntry records[8];
    const std::size_t n = body_records(concmd.atexit_log, records, 8);
    CHECK(n == 2);
    CHECK(records[0].dtor_rva == 0x20f0 && records[0].object == &objects.var);
    CHECK(records[1].dtor_rva == 0x272d0 && records[1].object == &objects.item);
    CHECK(concmd.pending.size() == 1);
}

void section_record_precedes_the_variable() {
    // An unpinned caller tag refuses the variable row — but the record was already written, exactly as
    // the original's straight-line order does. Nothing of the variable is touched.
    Slot374Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::FloatVarRow bad = kSlot374Vars[0];
    bad.arg_fn_vtbl = 0x15dead0;
    const debug_page::FloatVarRow rows[1] = {bad};
    const debug_page::HashedNameRow hashes[1] = {kSlot374HashedNames[0]};
    debug_page::Script script = kSlot374Script;
    script.vars = std::span<const debug_page::FloatVarRow>(rows, 1);
    script.hashed_names = std::span<const debug_page::HashedNameRow>(hashes, 1);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1),
                                   std::span<NameHashCell>(&objects.record, 1)};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_ctor);
    CHECK(r.hashed_names_built == 1 && r.vars_built == 0 && r.atexit_records == 0);
    CHECK(objects.record.name == kSlot374RecordName && objects.record.hash == kSlot374RecordHash);
    CHECK(region_poison(objects.var, 0, sizeof objects.var));
    CHECK(region_poison(objects.item, 0, sizeof objects.item));
}

void section_shape_refused() {
    // A script with a record but a host that provides no record stand-in: refused before ANY write.
    Slot374Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    {
        const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                       std::span<DebugMenuItem>(&objects.item, 1)};   // no hashed_names span
        const debug_page::Result r = debug_page::init_debug_page(kSlot374Script, view, concmd, menu, {});
        CHECK(!r.ok && r.blocked == debug_page::Stage::shape && r.atexit_records == 0);
        CHECK(region_poison(objects, 0, sizeof objects));
    }
    {
        // A second record exceeds the batch's declared maximum even when the span is large enough.
        NameHashCell cells[2];
        const debug_page::HashedNameRow two[2] = {kSlot374HashedNames[0], kSlot374HashedNames[0]};
        debug_page::Script script = kSlot374Script;
        script.hashed_names = std::span<const debug_page::HashedNameRow>(two, 2);
        const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                       std::span<DebugMenuItem>(&objects.item, 1),
                                       std::span<NameHashCell>(cells, 2)};
        const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
        CHECK(!r.ok && r.blocked == debug_page::Stage::shape);
        CHECK(region_poison(objects, 0, sizeof objects));
    }
    {
        // An item row that names a variable this script does not have.
        debug_page::MenuRow bad = kSlot374Items[0];
        bad.target_var = 1;
        const debug_page::MenuRow items[1] = {bad};
        debug_page::Script script = kSlot374Script;
        script.items = std::span<const debug_page::MenuRow>(items, 1);
        const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                       std::span<DebugMenuItem>(&objects.item, 1),
                                       std::span<NameHashCell>(&objects.record, 1)};
        const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
        CHECK(!r.ok && r.blocked == debug_page::Stage::shape);
    }
}

void section_register_refused() {
    Slot374Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int manager_token = 0;
    concmd.manager.manager = &manager_token;

    const debug_page::Result r = init_slot374_701230(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_register && r.vars_built == 0);
    CHECK(r.hashed_names_built == 1 && r.atexit_records == 0);
    CHECK(objects.record.hash == kSlot374RecordHash);
}

void section_item_register_refused() {
    Slot374Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int parent_token = 0;
    menu.parent.parent = &parent_token;

    const debug_page::Result r = init_slot374_701230(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::item_register && r.items_built == 0);
    CHECK(r.vars_built == 1 && r.atexit_records == 1);

    Slot374Objects objects2;
    ConCmdRegistry concmd2;
    DebugMenuRegistry menu2;
    menu2.parent.parent = &parent_token;
    menu2.parent.insert = &parent_insert_ok;
    const debug_page::Result r2 = init_slot374_701230(objects2, concmd2, menu2);
    CHECK(r2.ok && objects2.item.registered == 1);   // the parent path sets +0x20
}

void section_canary() {
    std::uint64_t guard = 0x13579bdf2468ace0ull;
    guard_cell = &guard;
    {
        Slot374Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        const debug_page::Result r = init_slot374_701230(objects, concmd, menu, {&guard});
        CHECK(r.ok && r.canary_checked && r.canary_matched);
    }
    {
        Slot374Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        int parent_token = 0;
        menu.parent.parent = &parent_token;
        menu.parent.insert = &parent_insert_flips_guard;
        const debug_page::Result r = init_slot374_701230(objects, concmd, menu, {&guard});
        CHECK(!r.ok && r.blocked == debug_page::Stage::canary && !r.canary_matched);
    }
    guard_cell = nullptr;
}

}  // namespace

int main() {
    section_constants();
    section_native_build();
    section_record_precedes_the_variable();
    section_shape_refused();
    section_register_refused();
    section_item_register_refused();
    section_canary();
    std::printf("OK test_ctor_slot_374\n");
    return 0;
}
