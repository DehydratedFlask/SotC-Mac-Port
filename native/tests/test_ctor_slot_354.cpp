// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 354 (0x79e180): 3 bool and 1 bounded float variables and 1 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_354_79e180.hpp"

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

Row rows_of(Slot354Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'f', &o.r02, 0, -1, -1, -1, -1};
    out[3] = Row{'b', &o.r03, 2, -1, -1, -1, -1};
    out[4] = Row{'e', &o.r04, -1, 3, 0, 1, -1};
    out[5] = Row{'u', &o.r05, 0, -1, -1, -1, -1};
    out[6] = Row{'u', &o.r06, 1, -1, -1, -1, -1};
    out[7] = Row{'u', &o.r07, 2, -1, -1, -1, -1};
    out[8] = Row{'u', &o.r08, 3, -1, -1, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot354Rva == 0x79e180 && kSlot354Size == 2173 && kSlot354RowCount == 9);
    CHECK(is_known_float_arg_functor(kSlot354FloatFnTag));
    CHECK(is_float_functor_tag(kSlot354UFloatFnTag));
    CHECK(offsetof(Slot354Objects, r00) == 0x0);
    CHECK(offsetof(Slot354Objects, r01) == 0xc8);
    CHECK(offsetof(Slot354Objects, r02) == 0x190);
    CHECK(offsetof(Slot354Objects, r03) == 0x268);
    CHECK(offsetof(Slot354Objects, r04) == 0x330);
    CHECK(offsetof(Slot354Objects, r05) == 0x3d8);
    CHECK(offsetof(Slot354Objects, r06) == 0x4b0);
    CHECK(offsetof(Slot354Objects, r07) == 0x588);
    CHECK(offsetof(Slot354Objects, r08) == 0x660);
    CHECK(sizeof(Slot354Objects) == 0x738);
    for (std::size_t n = 0; n < kSlot354BoolCount; ++n) CHECK(std::strlen(kSlot354BoolNames[n]) == kSlot354BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot354FloatCount; ++n) CHECK(std::strlen(kSlot354FloatNames[n]) == kSlot354FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot354UFloatCount; ++n) CHECK(std::strlen(kSlot354UFloatNames[n]) == kSlot354UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot354StringCount; ++n) CHECK(std::strlen(kSlot354MenuStrings[n]) == kSlot354MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot354BoolFnTag, kSlot354BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot354BoolNames[row.var], kSlot354BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot354VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot354FloatNames[row.var], kSlot354FloatNameLengths[row.var]));
            CHECK(v.value == kSlot354FloatValueBits[row.var] && v.default_value == kSlot354FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot354FloatLoBits[row.var] && hi == kSlot354FloatHiBits[row.var]);
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot354VarFlags);
            CHECK(v.value == kSlot354UFloatValueBits[row.var] && v.default_value == kSlot354UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot354UFloatNames[row.var], kSlot354UFloatNameLengths[row.var]));
            if (kSlot354UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot354MenuStrings[kSlot354UFloatDesc[row.var]], kSlot354MenuStringLengths[kSlot354UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot354MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot354MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot354ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot354MenuStrings[row.s0], kSlot354MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot354MenuStrings[row.s1], kSlot354MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot354MenuStrings[row.s1], kSlot354MenuStringLengths[row.s1]));
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
    Slot354Objects o;
    poison(o);
    const Slot354Result r = init_slot354_79e180(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot354RowCount);
    Row rows[kSlot354RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot354RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot354BoolCount + kSlot354FloatCount + kSlot354UFloatCount);
    CHECK(menu.root.count == items && items == 1 && menu.root.link.next == &o.r04.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot354CellCount && creg.atexit_log.size() == kSlot354RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot354RowCount; ++i) {
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
        Slot354Objects o;
        const Slot354Result r = init_slot354_79e180(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot354Objects o;
        const Slot354Result r = init_slot354_79e180(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 4);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_354");
    return 0;
}
