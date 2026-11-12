// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 382 (0x6c8600): 1 bool and 4 bounded float variables and 4 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_382_6c8600.hpp"

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

Row rows_of(Slot382Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'c', &o.r00, -1, -1, 0, -1, -1};
    out[1] = Row{'f', &o.r01, 0, -1, -1, -1, -1};
    out[2] = Row{'f', &o.r02, 1, -1, -1, -1, -1};
    out[3] = Row{'b', &o.r03, 0, -1, -1, -1, -1};
    out[4] = Row{'f', &o.r04, 2, -1, -1, -1, -1};
    out[5] = Row{'f', &o.r05, 3, -1, -1, -1, -1};
    out[6] = Row{'t', &o.r06, -1, 2, 1, -1, -1};
    out[7] = Row{'t', &o.r07, -1, 3, 2, -1, -1};
    out[8] = Row{'t', &o.r08, -1, 4, 2, -1, -1};
    out[9] = Row{'t', &o.r09, -1, 5, 2, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot382Rva == 0x6c8600 && kSlot382Size == 1870 && kSlot382RowCount == 10);
    CHECK(is_known_float_arg_functor(kSlot382FloatFnTag));
    CHECK(offsetof(Slot382Objects, r00) == 0x0);
    CHECK(offsetof(Slot382Objects, r01) == 0x10);
    CHECK(offsetof(Slot382Objects, r02) == 0xe8);
    CHECK(offsetof(Slot382Objects, r03) == 0x1c0);
    CHECK(offsetof(Slot382Objects, r04) == 0x288);
    CHECK(offsetof(Slot382Objects, r05) == 0x360);
    CHECK(offsetof(Slot382Objects, r06) == 0x438);
    CHECK(offsetof(Slot382Objects, r07) == 0x4e0);
    CHECK(offsetof(Slot382Objects, r08) == 0x588);
    CHECK(offsetof(Slot382Objects, r09) == 0x630);
    CHECK(sizeof(Slot382Objects) == 0x6d8);
    for (std::size_t n = 0; n < kSlot382BoolCount; ++n) CHECK(std::strlen(kSlot382BoolNames[n]) == kSlot382BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot382FloatCount; ++n) CHECK(std::strlen(kSlot382FloatNames[n]) == kSlot382FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot382StringCount; ++n) CHECK(std::strlen(kSlot382MenuStrings[n]) == kSlot382MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot382BoolFnTag, kSlot382BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot382BoolNames[row.var], kSlot382BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot382VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot382FloatNames[row.var], kSlot382FloatNameLengths[row.var]));
            CHECK(v.value == kSlot382FloatValueBits[row.var] && v.default_value == kSlot382FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot382FloatLoBits[row.var] && hi == kSlot382FloatHiBits[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot382MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot382MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot382ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot382MenuStrings[row.s0], kSlot382MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot382MenuStrings[row.s1], kSlot382MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot382MenuStrings[row.s1], kSlot382MenuStringLengths[row.s1]));
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
    Slot382Objects o;
    poison(o);
    const Slot382Result r = init_slot382_6c8600(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot382RowCount);
    Row rows[kSlot382RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot382RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot382BoolCount + kSlot382FloatCount + kSlot382UFloatCount);
    CHECK(menu.root.count == items && items == 4 && menu.root.link.next == &o.r06.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r01.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot382CellCount && creg.atexit_log.size() == kSlot382RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot382RowCount; ++i) {
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
        Slot382Objects o;
        const Slot382Result r = init_slot382_6c8600(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot382Objects o;
        const Slot382Result r = init_slot382_6c8600(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 6);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_382");
    return 0;
}
