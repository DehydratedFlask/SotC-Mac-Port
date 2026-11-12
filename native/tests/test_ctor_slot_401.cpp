// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 401 (0x621530): 10 bool and 0 bounded float variables and 11 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_401_621530.hpp"

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

Row rows_of(Slot401Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'l', &o.r03, 0, -1, -1, -1, -1};
    out[4] = Row{'l', &o.r04, 1, -1, -1, -1, -1};
    out[5] = Row{'b', &o.r05, 3, -1, -1, -1, -1};
    out[6] = Row{'b', &o.r06, 4, -1, -1, -1, -1};
    out[7] = Row{'t', &o.r07, -1, 2, 0, -1, -1};
    out[8] = Row{'t', &o.r08, -1, 3, 0, -1, -1};
    out[9] = Row{'t', &o.r09, -1, 4, 0, -1, -1};
    out[10] = Row{'t', &o.r10, -1, 0, 0, -1, -1};
    out[11] = Row{'t', &o.r11, -1, 1, 0, -1, -1};
    out[12] = Row{'t', &o.r12, -1, 6, 1, -1, -1};
    out[13] = Row{'b', &o.r13, 5, -1, -1, -1, -1};
    out[14] = Row{'t', &o.r14, -1, 13, 0, -1, -1};
    out[15] = Row{'b', &o.r15, 6, -1, -1, -1, -1};
    out[16] = Row{'t', &o.r16, -1, 15, 0, -1, -1};
    out[17] = Row{'b', &o.r17, 7, -1, -1, -1, -1};
    out[18] = Row{'t', &o.r18, -1, 17, 0, -1, -1};
    out[19] = Row{'b', &o.r19, 8, -1, -1, -1, -1};
    out[20] = Row{'t', &o.r20, -1, 19, 0, -1, -1};
    out[21] = Row{'b', &o.r21, 9, -1, -1, -1, -1};
    out[22] = Row{'t', &o.r22, -1, 21, 0, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot401Rva == 0x621530 && kSlot401Size == 3799 && kSlot401RowCount == 23);
    for (const LocalIntSiteSpec& site : kSlot401LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
    CHECK(offsetof(Slot401Objects, r00) == 0x0);
    CHECK(offsetof(Slot401Objects, r01) == 0xc8);
    CHECK(offsetof(Slot401Objects, r02) == 0x190);
    CHECK(offsetof(Slot401Objects, r03) == 0x258);
    CHECK(offsetof(Slot401Objects, r04) == 0x330);
    CHECK(offsetof(Slot401Objects, r05) == 0x408);
    CHECK(offsetof(Slot401Objects, r06) == 0x4d0);
    CHECK(offsetof(Slot401Objects, r07) == 0x598);
    CHECK(offsetof(Slot401Objects, r08) == 0x640);
    CHECK(offsetof(Slot401Objects, r09) == 0x6e8);
    CHECK(offsetof(Slot401Objects, r10) == 0x790);
    CHECK(offsetof(Slot401Objects, r11) == 0x838);
    CHECK(offsetof(Slot401Objects, r12) == 0x8e0);
    CHECK(offsetof(Slot401Objects, r13) == 0x9a0);
    CHECK(offsetof(Slot401Objects, r14) == 0xa68);
    CHECK(offsetof(Slot401Objects, r15) == 0xb10);
    CHECK(offsetof(Slot401Objects, r16) == 0xbd8);
    CHECK(offsetof(Slot401Objects, r17) == 0xc80);
    CHECK(offsetof(Slot401Objects, r18) == 0xd48);
    CHECK(offsetof(Slot401Objects, r19) == 0xdf0);
    CHECK(offsetof(Slot401Objects, r20) == 0xeb8);
    CHECK(offsetof(Slot401Objects, r21) == 0xf60);
    CHECK(offsetof(Slot401Objects, r22) == 0x1028);
    CHECK(sizeof(Slot401Objects) == 0x10d0);
    for (std::size_t n = 0; n < kSlot401BoolCount; ++n) CHECK(std::strlen(kSlot401BoolNames[n]) == kSlot401BoolNameLengths[n]);
    for (const LocalIntSiteSpec& site : kSlot401LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot401StringCount; ++n) CHECK(std::strlen(kSlot401MenuStrings[n]) == kSlot401MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot401BoolFnTag, kSlot401BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot401BoolNames[row.var], kSlot401BoolNameLengths[row.var]));
            break;
        }
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot401LocalIntSites[row.var];
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
            CHECK(c.name == kSlot401MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot401MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot401ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot401MenuStrings[row.s0], kSlot401MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot401MenuStrings[row.s1], kSlot401MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot401MenuStrings[row.s1], kSlot401MenuStringLengths[row.s1]));
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
    Slot401Objects o;
    poison(o);
    const Slot401Result r = init_slot401_621530(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot401RowCount);
    Row rows[kSlot401RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot401RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot401BoolCount + kSlot401IntCount + kSlot401LocalIntCount + kSlot401FloatCount + kSlot401UFloatCount);
    CHECK(menu.root.count == items && items == 11 && menu.root.link.next == &o.r07.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot401CellCount && creg.atexit_log.size() == kSlot401RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot401RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : rows[i].kind == 'i' ? 0x18a60 : rows[i].kind == 'l' ? find_local_int_var_class(kSlot401LocalIntSites[rows[i].var].wrapper_rva)->dtor_rva : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
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
        Slot401Objects o;
        const Slot401Result r = init_slot401_621530(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot401Objects o;
        const Slot401Result r = init_slot401_621530(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 7);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_401");
    return 0;
}
