// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 398 (0x63cf60): 2 bool and 0 bounded float variables and 4 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_398_63cf60.hpp"

namespace {
using namespace l4;

struct Row {
    char kind;            // b bool, f float, t 0x26db0 toggle, e 0x26ea0 toggle, s 0x27120 slider, g group
    const void* obj;
    int var;              // index into the bool / float tables (variables)
    int target;           // row index of the target variable (items)
    int s0;               // menu string: parent / label
    int s1;               // menu string: explicit text / caption
    int slider;           // slider table index
};

Row rows_of(Slot398Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'t', &o.r01, -1, 0, 0, -1, -1};
    out[2] = Row{'b', &o.r02, 1, -1, -1, -1, -1};
    out[3] = Row{'t', &o.r03, -1, 2, 0, -1, -1};
    out[4] = Row{'l', &o.r04, 0, -1, -1, -1, -1};
    out[5] = Row{'l', &o.r05, 1, -1, -1, -1, -1};
    out[6] = Row{'e', &o.r06, -1, 4, 1, 2, -1};
    out[7] = Row{'e', &o.r07, -1, 5, 1, 3, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot398Rva == 0x63cf60 && kSlot398Size == 1408 && kSlot398RowCount == 8);
    for (const LocalIntSiteSpec& site : kSlot398LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
    CHECK(offsetof(Slot398Objects, r00) == 0x0);
    CHECK(offsetof(Slot398Objects, r01) == 0xc8);
    CHECK(offsetof(Slot398Objects, r02) == 0x170);
    CHECK(offsetof(Slot398Objects, r03) == 0x238);
    CHECK(offsetof(Slot398Objects, r04) == 0x2e0);
    CHECK(offsetof(Slot398Objects, r05) == 0x3b8);
    CHECK(offsetof(Slot398Objects, r06) == 0x490);
    CHECK(offsetof(Slot398Objects, r07) == 0x538);
    CHECK(sizeof(Slot398Objects) == 0x5e0);
    for (std::size_t n = 0; n < kSlot398BoolCount; ++n) CHECK(std::strlen(kSlot398BoolNames[n]) == kSlot398BoolNameLengths[n]);
    for (const LocalIntSiteSpec& site : kSlot398LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot398StringCount; ++n) CHECK(std::strlen(kSlot398MenuStrings[n]) == kSlot398MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot398BoolFnTag, kSlot398BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot398BoolNames[row.var], kSlot398BoolNameLengths[row.var]));
            break;
        }
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot398LocalIntSites[row.var];
            const LocalIntVarClass* cls = find_local_int_var_class(site.wrapper_rva);
            CHECK(cls != nullptr && (site.arg_fn_tag == cls->fn_vtbl || is_local_int_caller_twin_tag(site.arg_fn_tag, cls->fn_vtbl)));
            CHECK(v.base.vptr == cls->final_vptr && v.base.flags == site.flags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, site.name, site.name_length) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(site.value) && dword(v, 0x68) == static_cast<std::uint32_t>(site.value));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(site.min_value) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(site.max_value) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == cls->fn_vtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot398MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot398MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot398ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot398MenuStrings[row.s0], kSlot398MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot398MenuStrings[row.s1], kSlot398MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot398MenuStrings[row.s1], kSlot398MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's') {
                CHECK(false);   // no slider rows in this slot
            } else if (is_float) {
                CHECK(it.range_min == 0.1f && it.range_max == 0.5f);   // float class slot3 = 1, slot6 = 3: the one narrowing case
            } else {
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
            }
            break;
        }
    }
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot398Objects o;
    poison(o);
    const Slot398Result r = init_slot398_63cf60(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot398RowCount);
    Row rows[kSlot398RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot398RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot398BoolCount + kSlot398IntCount + kSlot398LocalIntCount + kSlot398FloatCount + kSlot398UFloatCount);
    CHECK(menu.root.count == items && items == 4 && menu.root.link.next == &o.r01.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot398CellCount && creg.atexit_log.size() == kSlot398RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot398RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : rows[i].kind == 'i' ? 0x18a60 : rows[i].kind == 'l' ? find_local_int_var_class(kSlot398LocalIntSites[rows[i].var].wrapper_rva)->dtor_rva : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
        if (dtor == 0x272d0 && !root_seen) {
            check_atexit(creg, at, 0x27280, &menu.root);
            root_seen = true;
            check_atexit(creg, at + 1, dtor, rows[i].obj);
        } else {
            check_atexit(creg, at, dtor, rows[i].obj);
        }
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot398Objects o;
        const Slot398Result r = init_slot398_63cf60(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot398Objects o;
        const Slot398Result r = init_slot398_63cf60(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_398");
    return 0;
}
