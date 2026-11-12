// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 373 (0x7081e0): 3 bool and 0 bounded float variables and 5 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_373_7081e0.hpp"

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

Row rows_of(Slot373Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'u', &o.r03, 0, -1, -1, -1, -1};
    out[4] = Row{'u', &o.r04, 1, -1, -1, -1, -1};
    out[5] = Row{'u', &o.r05, 2, -1, -1, -1, -1};
    out[6] = Row{'u', &o.r06, 3, -1, -1, -1, -1};
    out[7] = Row{'t', &o.r07, -1, 2, 0, -1, -1};
    out[8] = Row{'t', &o.r08, -1, 3, 0, -1, -1};
    out[9] = Row{'t', &o.r09, -1, 4, 0, -1, -1};
    out[10] = Row{'t', &o.r10, -1, 5, 0, -1, -1};
    out[11] = Row{'t', &o.r11, -1, 6, 0, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot373Rva == 0x7081e0 && kSlot373Size == 2165 && kSlot373RowCount == 12);
    CHECK(is_float_functor_tag(kSlot373UFloatFnTag));
    CHECK(offsetof(Slot373Objects, r00) == 0x0);
    CHECK(offsetof(Slot373Objects, r01) == 0xc8);
    CHECK(offsetof(Slot373Objects, r02) == 0x190);
    CHECK(offsetof(Slot373Objects, r03) == 0x258);
    CHECK(offsetof(Slot373Objects, r04) == 0x330);
    CHECK(offsetof(Slot373Objects, r05) == 0x408);
    CHECK(offsetof(Slot373Objects, r06) == 0x4e0);
    CHECK(offsetof(Slot373Objects, r07) == 0x5b8);
    CHECK(offsetof(Slot373Objects, r08) == 0x660);
    CHECK(offsetof(Slot373Objects, r09) == 0x708);
    CHECK(offsetof(Slot373Objects, r10) == 0x7b0);
    CHECK(offsetof(Slot373Objects, r11) == 0x858);
    CHECK(sizeof(Slot373Objects) == 0x900);
    for (std::size_t n = 0; n < kSlot373BoolCount; ++n) CHECK(std::strlen(kSlot373BoolNames[n]) == kSlot373BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot373UFloatCount; ++n) CHECK(std::strlen(kSlot373UFloatNames[n]) == kSlot373UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot373StringCount; ++n) CHECK(std::strlen(kSlot373MenuStrings[n]) == kSlot373MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot373BoolFnTag, kSlot373BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot373BoolNames[row.var], kSlot373BoolNameLengths[row.var]));
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot373VarFlags);
            CHECK(v.value == kSlot373UFloatValueBits[row.var] && v.default_value == kSlot373UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot373UFloatNames[row.var], kSlot373UFloatNameLengths[row.var]));
            if (kSlot373UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot373MenuStrings[kSlot373UFloatDesc[row.var]], kSlot373MenuStringLengths[kSlot373UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot373MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot373MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot373ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot373MenuStrings[row.s0], kSlot373MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot373MenuStrings[row.s1], kSlot373MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot373MenuStrings[row.s1], kSlot373MenuStringLengths[row.s1]));
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
    Slot373Objects o;
    poison(o);
    const Slot373Result r = init_slot373_7081e0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot373RowCount);
    Row rows[kSlot373RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot373RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot373BoolCount + kSlot373FloatCount + kSlot373UFloatCount);
    CHECK(menu.root.count == items && items == 5 && menu.root.link.next == &o.r07.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot373CellCount && creg.atexit_log.size() == kSlot373RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot373RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0x8ef00 : 0x272d0;
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
        Slot373Objects o;
        const Slot373Result r = init_slot373_7081e0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot373Objects o;
        const Slot373Result r = init_slot373_7081e0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 7);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_373");
    return 0;
}
