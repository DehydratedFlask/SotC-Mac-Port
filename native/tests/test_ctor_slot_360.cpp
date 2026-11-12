// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 360 (0x7806f0): 0 bool and 4 bounded float variables and 4 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_360_7806f0.hpp"

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

Row rows_of(Slot360Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'f', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'f', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'f', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'f', &o.r03, 3, -1, -1, -1, -1};
    out[4] = Row{'t', &o.r04, -1, 0, 0, -1, -1};
    out[5] = Row{'t', &o.r05, -1, 1, 0, -1, -1};
    out[6] = Row{'t', &o.r06, -1, 2, 0, -1, -1};
    out[7] = Row{'t', &o.r07, -1, 3, 0, -1, -1};
    out[8] = Row{'h', &o.r08, -1, -1, 1, -1, -1};
    out[9] = Row{'c', &o.r09, -1, -1, 2, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot360Rva == 0x7806f0 && kSlot360Size == 1815 && kSlot360RowCount == 10);
    CHECK(is_known_float_arg_functor(kSlot360FloatFnTag));
    CHECK(offsetof(Slot360Objects, r00) == 0x0);
    CHECK(offsetof(Slot360Objects, r01) == 0xd8);
    CHECK(offsetof(Slot360Objects, r02) == 0x1b0);
    CHECK(offsetof(Slot360Objects, r03) == 0x288);
    CHECK(offsetof(Slot360Objects, r04) == 0x360);
    CHECK(offsetof(Slot360Objects, r05) == 0x408);
    CHECK(offsetof(Slot360Objects, r06) == 0x4b0);
    CHECK(offsetof(Slot360Objects, r07) == 0x558);
    CHECK(offsetof(Slot360Objects, r08) == 0x600);
    CHECK(offsetof(Slot360Objects, r09) == 0x630);
    CHECK(sizeof(Slot360Objects) == 0x640);
    for (std::size_t n = 0; n < kSlot360FloatCount; ++n) CHECK(std::strlen(kSlot360FloatNames[n]) == kSlot360FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot360StringCount; ++n) CHECK(std::strlen(kSlot360MenuStrings[n]) == kSlot360MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot360VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot360FloatNames[row.var], kSlot360FloatNameLengths[row.var]));
            CHECK(v.value == kSlot360FloatValueBits[row.var] && v.default_value == kSlot360FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot360FloatLoBits[row.var] && hi == kSlot360FloatHiBits[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot360MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot360MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == kSlot360HolderVptr && h.tail == 0 && same_view(h.name, kSlot360MenuStrings[row.s0], kSlot360MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot360ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot360MenuStrings[row.s0], kSlot360MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot360MenuStrings[row.s1], kSlot360MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot360MenuStrings[row.s1], kSlot360MenuStringLengths[row.s1]));
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
    Slot360Objects o;
    poison(o);
    const Slot360Result r = init_slot360_7806f0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot360RowCount);
    Row rows[kSlot360RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot360RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot360BoolCount + kSlot360FloatCount + kSlot360UFloatCount);
    CHECK(menu.root.count == items && items == 4 && menu.root.link.next == &o.r04.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot360CellCount && creg.atexit_log.size() == kSlot360RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot360RowCount; ++i) {
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
        Slot360Objects o;
        const Slot360Result r = init_slot360_7806f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot360Objects o;
        const Slot360Result r = init_slot360_7806f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 4);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_360");
    return 0;
}
