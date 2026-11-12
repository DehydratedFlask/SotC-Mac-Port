// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 375 (0x6f1c20): 11 bool and 23 bounded float variables and 27 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_375_6f1c20.hpp"

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

Row rows_of(Slot375Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'f', &o.r02, 0, -1, -1, -1, -1};
    out[3] = Row{'f', &o.r03, 1, -1, -1, -1, -1};
    out[4] = Row{'f', &o.r04, 2, -1, -1, -1, -1};
    out[5] = Row{'f', &o.r05, 3, -1, -1, -1, -1};
    out[6] = Row{'f', &o.r06, 4, -1, -1, -1, -1};
    out[7] = Row{'f', &o.r07, 5, -1, -1, -1, -1};
    out[8] = Row{'f', &o.r08, 6, -1, -1, -1, -1};
    out[9] = Row{'b', &o.r09, 2, -1, -1, -1, -1};
    out[10] = Row{'b', &o.r10, 3, -1, -1, -1, -1};
    out[11] = Row{'b', &o.r11, 4, -1, -1, -1, -1};
    out[12] = Row{'b', &o.r12, 5, -1, -1, -1, -1};
    out[13] = Row{'b', &o.r13, 6, -1, -1, -1, -1};
    out[14] = Row{'b', &o.r14, 7, -1, -1, -1, -1};
    out[15] = Row{'b', &o.r15, 8, -1, -1, -1, -1};
    out[16] = Row{'f', &o.r16, 7, -1, -1, -1, -1};
    out[17] = Row{'f', &o.r17, 8, -1, -1, -1, -1};
    out[18] = Row{'f', &o.r18, 9, -1, -1, -1, -1};
    out[19] = Row{'f', &o.r19, 10, -1, -1, -1, -1};
    out[20] = Row{'b', &o.r20, 9, -1, -1, -1, -1};
    out[21] = Row{'f', &o.r21, 11, -1, -1, -1, -1};
    out[22] = Row{'f', &o.r22, 12, -1, -1, -1, -1};
    out[23] = Row{'f', &o.r23, 13, -1, -1, -1, -1};
    out[24] = Row{'f', &o.r24, 14, -1, -1, -1, -1};
    out[25] = Row{'f', &o.r25, 15, -1, -1, -1, -1};
    out[26] = Row{'b', &o.r26, 10, -1, -1, -1, -1};
    out[27] = Row{'f', &o.r27, 16, -1, -1, -1, -1};
    out[28] = Row{'f', &o.r28, 17, -1, -1, -1, -1};
    out[29] = Row{'f', &o.r29, 18, -1, -1, -1, -1};
    out[30] = Row{'f', &o.r30, 19, -1, -1, -1, -1};
    out[31] = Row{'f', &o.r31, 20, -1, -1, -1, -1};
    out[32] = Row{'f', &o.r32, 21, -1, -1, -1, -1};
    out[33] = Row{'f', &o.r33, 22, -1, -1, -1, -1};
    out[34] = Row{'s', &o.r34, -1, 31, 0, -1, 0};
    out[35] = Row{'s', &o.r35, -1, 32, 0, -1, 1};
    out[36] = Row{'s', &o.r36, -1, 33, 0, -1, 2};
    out[37] = Row{'t', &o.r37, -1, 9, 0, -1, -1};
    out[38] = Row{'t', &o.r38, -1, 10, 0, -1, -1};
    out[39] = Row{'g', &o.r39, -1, -1, 0, 1, -1};
    out[40] = Row{'g', &o.r40, -1, -1, 1, 2, -1};
    out[41] = Row{'e', &o.r41, -1, 14, 2, 3, -1};
    out[42] = Row{'e', &o.r42, -1, 15, 2, 4, -1};
    out[43] = Row{'e', &o.r43, -1, 16, 2, 5, -1};
    out[44] = Row{'e', &o.r44, -1, 17, 2, 6, -1};
    out[45] = Row{'e', &o.r45, -1, 18, 2, 7, -1};
    out[46] = Row{'e', &o.r46, -1, 19, 2, 8, -1};
    out[47] = Row{'e', &o.r47, -1, 20, 2, 9, -1};
    out[48] = Row{'e', &o.r48, -1, 21, 2, 10, -1};
    out[49] = Row{'e', &o.r49, -1, 22, 2, 11, -1};
    out[50] = Row{'e', &o.r50, -1, 23, 2, 12, -1};
    out[51] = Row{'e', &o.r51, -1, 24, 2, 13, -1};
    out[52] = Row{'g', &o.r52, -1, -1, 1, 14, -1};
    out[53] = Row{'e', &o.r53, -1, 26, 14, 15, -1};
    out[54] = Row{'e', &o.r54, -1, 27, 14, 16, -1};
    out[55] = Row{'e', &o.r55, -1, 28, 14, 17, -1};
    out[56] = Row{'e', &o.r56, -1, 29, 14, 18, -1};
    out[57] = Row{'e', &o.r57, -1, 30, 14, 19, -1};
    out[58] = Row{'t', &o.r58, -1, 11, 0, -1, -1};
    out[59] = Row{'t', &o.r59, -1, 12, 0, -1, -1};
    out[60] = Row{'t', &o.r60, -1, 13, 0, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot375Rva == 0x6f1c20 && kSlot375Size == 12802 && kSlot375RowCount == 61);
    CHECK(is_known_float_arg_functor(kSlot375FloatFnTag));
    CHECK(offsetof(Slot375Objects, r00) == 0x0);
    CHECK(offsetof(Slot375Objects, r01) == 0xc8);
    CHECK(offsetof(Slot375Objects, r02) == 0x190);
    CHECK(offsetof(Slot375Objects, r03) == 0x268);
    CHECK(offsetof(Slot375Objects, r04) == 0x340);
    CHECK(offsetof(Slot375Objects, r05) == 0x418);
    CHECK(offsetof(Slot375Objects, r06) == 0x4f0);
    CHECK(offsetof(Slot375Objects, r07) == 0x5c8);
    CHECK(offsetof(Slot375Objects, r08) == 0x6a0);
    CHECK(offsetof(Slot375Objects, r09) == 0x778);
    CHECK(offsetof(Slot375Objects, r10) == 0x840);
    CHECK(offsetof(Slot375Objects, r11) == 0x908);
    CHECK(offsetof(Slot375Objects, r12) == 0x9d0);
    CHECK(offsetof(Slot375Objects, r13) == 0xa98);
    CHECK(offsetof(Slot375Objects, r14) == 0xb60);
    CHECK(offsetof(Slot375Objects, r15) == 0xc28);
    CHECK(offsetof(Slot375Objects, r16) == 0xcf0);
    CHECK(offsetof(Slot375Objects, r17) == 0xdc8);
    CHECK(offsetof(Slot375Objects, r18) == 0xea0);
    CHECK(offsetof(Slot375Objects, r19) == 0xf78);
    CHECK(offsetof(Slot375Objects, r20) == 0x1050);
    CHECK(offsetof(Slot375Objects, r21) == 0x1118);
    CHECK(offsetof(Slot375Objects, r22) == 0x11f0);
    CHECK(offsetof(Slot375Objects, r23) == 0x12c8);
    CHECK(offsetof(Slot375Objects, r24) == 0x13a0);
    CHECK(offsetof(Slot375Objects, r25) == 0x1478);
    CHECK(offsetof(Slot375Objects, r26) == 0x1550);
    CHECK(offsetof(Slot375Objects, r27) == 0x1618);
    CHECK(offsetof(Slot375Objects, r28) == 0x16f0);
    CHECK(offsetof(Slot375Objects, r29) == 0x17c8);
    CHECK(offsetof(Slot375Objects, r30) == 0x18a0);
    CHECK(offsetof(Slot375Objects, r31) == 0x1978);
    CHECK(offsetof(Slot375Objects, r32) == 0x1a50);
    CHECK(offsetof(Slot375Objects, r33) == 0x1b28);
    CHECK(offsetof(Slot375Objects, r34) == 0x1c00);
    CHECK(offsetof(Slot375Objects, r35) == 0x1ca8);
    CHECK(offsetof(Slot375Objects, r36) == 0x1d50);
    CHECK(offsetof(Slot375Objects, r37) == 0x1df8);
    CHECK(offsetof(Slot375Objects, r38) == 0x1ea0);
    CHECK(offsetof(Slot375Objects, r39) == 0x1f48);
    CHECK(offsetof(Slot375Objects, r40) == 0x1ff0);
    CHECK(offsetof(Slot375Objects, r41) == 0x2098);
    CHECK(offsetof(Slot375Objects, r42) == 0x2140);
    CHECK(offsetof(Slot375Objects, r43) == 0x21e8);
    CHECK(offsetof(Slot375Objects, r44) == 0x2290);
    CHECK(offsetof(Slot375Objects, r45) == 0x2338);
    CHECK(offsetof(Slot375Objects, r46) == 0x23e0);
    CHECK(offsetof(Slot375Objects, r47) == 0x2488);
    CHECK(offsetof(Slot375Objects, r48) == 0x2530);
    CHECK(offsetof(Slot375Objects, r49) == 0x25d8);
    CHECK(offsetof(Slot375Objects, r50) == 0x2680);
    CHECK(offsetof(Slot375Objects, r51) == 0x2728);
    CHECK(offsetof(Slot375Objects, r52) == 0x27d0);
    CHECK(offsetof(Slot375Objects, r53) == 0x2878);
    CHECK(offsetof(Slot375Objects, r54) == 0x2920);
    CHECK(offsetof(Slot375Objects, r55) == 0x29c8);
    CHECK(offsetof(Slot375Objects, r56) == 0x2a70);
    CHECK(offsetof(Slot375Objects, r57) == 0x2b18);
    CHECK(offsetof(Slot375Objects, r58) == 0x2bc0);
    CHECK(offsetof(Slot375Objects, r59) == 0x2c68);
    CHECK(offsetof(Slot375Objects, r60) == 0x2d10);
    CHECK(sizeof(Slot375Objects) == 0x2db8);
    for (std::size_t n = 0; n < kSlot375BoolCount; ++n) CHECK(std::strlen(kSlot375BoolNames[n]) == kSlot375BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot375FloatCount; ++n) CHECK(std::strlen(kSlot375FloatNames[n]) == kSlot375FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot375StringCount; ++n) CHECK(std::strlen(kSlot375MenuStrings[n]) == kSlot375MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot375BoolFnTag, kSlot375BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot375BoolNames[row.var], kSlot375BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot375VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot375FloatNames[row.var], kSlot375FloatNameLengths[row.var]));
            CHECK(v.value == kSlot375FloatValueBits[row.var] && v.default_value == kSlot375FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot375FloatLoBits[row.var] && hi == kSlot375FloatHiBits[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot375MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot375MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot375ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot375MenuStrings[row.s0], kSlot375MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot375MenuStrings[row.s1], kSlot375MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot375MenuStrings[row.s1], kSlot375MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's') {
                CHECK(it.range_min == bits_to_float(kSlot375SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot375SliderHiBits[row.slider]));
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
    Slot375Objects o;
    poison(o);
    const Slot375Result r = init_slot375_6f1c20(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot375RowCount);
    Row rows[kSlot375RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot375RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot375BoolCount + kSlot375FloatCount + kSlot375UFloatCount);
    CHECK(menu.root.count == items && items == 27 && menu.root.link.next == &o.r34.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot375CellCount && creg.atexit_log.size() == kSlot375RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot375RowCount; ++i) {
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
        Slot375Objects o;
        const Slot375Result r = init_slot375_6f1c20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot375Objects o;
        const Slot375Result r = init_slot375_6f1c20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 34);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_375");
    return 0;
}
