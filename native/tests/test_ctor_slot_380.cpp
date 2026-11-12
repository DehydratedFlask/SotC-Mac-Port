// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 380 (0x6d71f0): 11 bool and 3 bounded float variables and 13 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_380_6d71f0.hpp"

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

Row rows_of(Slot380Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'t', &o.r02, -1, 1, 0, -1, -1};
    out[3] = Row{'b', &o.r03, 2, -1, -1, -1, -1};
    out[4] = Row{'t', &o.r04, -1, 3, 0, -1, -1};
    out[5] = Row{'b', &o.r05, 3, -1, -1, -1, -1};
    out[6] = Row{'t', &o.r06, -1, 5, 0, -1, -1};
    out[7] = Row{'b', &o.r07, 4, -1, -1, -1, -1};
    out[8] = Row{'t', &o.r08, -1, 7, 0, -1, -1};
    out[9] = Row{'b', &o.r09, 5, -1, -1, -1, -1};
    out[10] = Row{'t', &o.r10, -1, 9, 1, -1, -1};
    out[11] = Row{'t', &o.r11, -1, 0, 1, -1, -1};
    out[12] = Row{'f', &o.r12, 0, -1, -1, -1, -1};
    out[13] = Row{'s', &o.r13, -1, 12, 1, -1, 0};
    out[14] = Row{'b', &o.r14, 6, -1, -1, -1, -1};
    out[15] = Row{'t', &o.r15, -1, 14, 1, -1, -1};
    out[16] = Row{'f', &o.r16, 1, -1, -1, -1, -1};
    out[17] = Row{'t', &o.r17, -1, 16, 1, -1, -1};
    out[18] = Row{'b', &o.r18, 7, -1, -1, -1, -1};
    out[19] = Row{'t', &o.r19, -1, 18, 1, -1, -1};
    out[20] = Row{'f', &o.r20, 2, -1, -1, -1, -1};
    out[21] = Row{'b', &o.r21, 8, -1, -1, -1, -1};
    out[22] = Row{'t', &o.r22, -1, 21, 1, -1, -1};
    out[23] = Row{'b', &o.r23, 9, -1, -1, -1, -1};
    out[24] = Row{'t', &o.r24, -1, 23, 1, -1, -1};
    out[25] = Row{'b', &o.r25, 10, -1, -1, -1, -1};
    out[26] = Row{'t', &o.r26, -1, 25, 1, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot380Rva == 0x6d71f0 && kSlot380Size == 4668 && kSlot380RowCount == 27);
    CHECK(is_known_float_arg_functor(kSlot380FloatFnTag));
    CHECK(offsetof(Slot380Objects, r00) == 0x0);
    CHECK(offsetof(Slot380Objects, r01) == 0xc8);
    CHECK(offsetof(Slot380Objects, r02) == 0x190);
    CHECK(offsetof(Slot380Objects, r03) == 0x238);
    CHECK(offsetof(Slot380Objects, r04) == 0x300);
    CHECK(offsetof(Slot380Objects, r05) == 0x3a8);
    CHECK(offsetof(Slot380Objects, r06) == 0x470);
    CHECK(offsetof(Slot380Objects, r07) == 0x518);
    CHECK(offsetof(Slot380Objects, r08) == 0x5e0);
    CHECK(offsetof(Slot380Objects, r09) == 0x688);
    CHECK(offsetof(Slot380Objects, r10) == 0x750);
    CHECK(offsetof(Slot380Objects, r11) == 0x7f8);
    CHECK(offsetof(Slot380Objects, r12) == 0x8a0);
    CHECK(offsetof(Slot380Objects, r13) == 0x978);
    CHECK(offsetof(Slot380Objects, r14) == 0xa20);
    CHECK(offsetof(Slot380Objects, r15) == 0xae8);
    CHECK(offsetof(Slot380Objects, r16) == 0xb90);
    CHECK(offsetof(Slot380Objects, r17) == 0xc68);
    CHECK(offsetof(Slot380Objects, r18) == 0xd10);
    CHECK(offsetof(Slot380Objects, r19) == 0xdd8);
    CHECK(offsetof(Slot380Objects, r20) == 0xe80);
    CHECK(offsetof(Slot380Objects, r21) == 0xf58);
    CHECK(offsetof(Slot380Objects, r22) == 0x1020);
    CHECK(offsetof(Slot380Objects, r23) == 0x10c8);
    CHECK(offsetof(Slot380Objects, r24) == 0x1190);
    CHECK(offsetof(Slot380Objects, r25) == 0x1238);
    CHECK(offsetof(Slot380Objects, r26) == 0x1300);
    CHECK(sizeof(Slot380Objects) == 0x13a8);
    for (std::size_t n = 0; n < kSlot380BoolCount; ++n) CHECK(std::strlen(kSlot380BoolNames[n]) == kSlot380BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot380FloatCount; ++n) CHECK(std::strlen(kSlot380FloatNames[n]) == kSlot380FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot380StringCount; ++n) CHECK(std::strlen(kSlot380MenuStrings[n]) == kSlot380MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot380BoolFnTag, kSlot380BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot380BoolNames[row.var], kSlot380BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot380VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot380FloatNames[row.var], kSlot380FloatNameLengths[row.var]));
            CHECK(v.value == kSlot380FloatValueBits[row.var] && v.default_value == kSlot380FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot380FloatLoBits[row.var] && hi == kSlot380FloatHiBits[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot380MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot380MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot380ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot380MenuStrings[row.s0], kSlot380MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot380MenuStrings[row.s1], kSlot380MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot380MenuStrings[row.s1], kSlot380MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's') {
                CHECK(it.range_min == bits_to_float(kSlot380SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot380SliderHiBits[row.slider]));
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
    Slot380Objects o;
    poison(o);
    const Slot380Result r = init_slot380_6d71f0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot380RowCount);
    Row rows[kSlot380RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot380RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot380BoolCount + kSlot380FloatCount + kSlot380UFloatCount);
    CHECK(menu.root.count == items && items == 13 && menu.root.link.next == &o.r02.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot380CellCount && creg.atexit_log.size() == kSlot380RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot380RowCount; ++i) {
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
        Slot380Objects o;
        const Slot380Result r = init_slot380_6d71f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot380Objects o;
        const Slot380Result r = init_slot380_6d71f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 2);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_380");
    return 0;
}
