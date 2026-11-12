// CPU-01-STARTUP-BATCH-10: generated fixture for startup constructor index 349 (0x7bde50): 9 bool and 28 bounded float variables and 5 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_349_7bde50.hpp"

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

Row rows_of(Slot349Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'f', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'f', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'f', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'f', &o.r03, 3, -1, -1, -1, -1};
    out[4] = Row{'f', &o.r04, 4, -1, -1, -1, -1};
    out[5] = Row{'f', &o.r05, 5, -1, -1, -1, -1};
    out[6] = Row{'f', &o.r06, 6, -1, -1, -1, -1};
    out[7] = Row{'b', &o.r07, 0, -1, -1, -1, -1};
    out[8] = Row{'f', &o.r08, 7, -1, -1, -1, -1};
    out[9] = Row{'f', &o.r09, 8, -1, -1, -1, -1};
    out[10] = Row{'f', &o.r10, 9, -1, -1, -1, -1};
    out[11] = Row{'f', &o.r11, 10, -1, -1, -1, -1};
    out[12] = Row{'f', &o.r12, 11, -1, -1, -1, -1};
    out[13] = Row{'f', &o.r13, 12, -1, -1, -1, -1};
    out[14] = Row{'f', &o.r14, 13, -1, -1, -1, -1};
    out[15] = Row{'f', &o.r15, 14, -1, -1, -1, -1};
    out[16] = Row{'f', &o.r16, 15, -1, -1, -1, -1};
    out[17] = Row{'f', &o.r17, 16, -1, -1, -1, -1};
    out[18] = Row{'f', &o.r18, 17, -1, -1, -1, -1};
    out[19] = Row{'f', &o.r19, 18, -1, -1, -1, -1};
    out[20] = Row{'f', &o.r20, 19, -1, -1, -1, -1};
    out[21] = Row{'f', &o.r21, 20, -1, -1, -1, -1};
    out[22] = Row{'f', &o.r22, 21, -1, -1, -1, -1};
    out[23] = Row{'b', &o.r23, 1, -1, -1, -1, -1};
    out[24] = Row{'b', &o.r24, 2, -1, -1, -1, -1};
    out[25] = Row{'b', &o.r25, 3, -1, -1, -1, -1};
    out[26] = Row{'f', &o.r26, 22, -1, -1, -1, -1};
    out[27] = Row{'b', &o.r27, 4, -1, -1, -1, -1};
    out[28] = Row{'b', &o.r28, 5, -1, -1, -1, -1};
    out[29] = Row{'b', &o.r29, 6, -1, -1, -1, -1};
    out[30] = Row{'b', &o.r30, 7, -1, -1, -1, -1};
    out[31] = Row{'f', &o.r31, 23, -1, -1, -1, -1};
    out[32] = Row{'f', &o.r32, 24, -1, -1, -1, -1};
    out[33] = Row{'f', &o.r33, 25, -1, -1, -1, -1};
    out[34] = Row{'f', &o.r34, 26, -1, -1, -1, -1};
    out[35] = Row{'f', &o.r35, 27, -1, -1, -1, -1};
    out[36] = Row{'b', &o.r36, 8, -1, -1, -1, -1};
    out[37] = Row{'s', &o.r37, -1, 0, 0, -1, 0};
    out[38] = Row{'s', &o.r38, -1, 1, 0, -1, 1};
    out[39] = Row{'s', &o.r39, -1, 35, 0, -1, 2};
    out[40] = Row{'t', &o.r40, -1, 24, 1, -1, -1};
    out[41] = Row{'t', &o.r41, -1, 25, 1, -1, -1};
    out[42] = Row{'h', &o.r42, 0, -1, 2, -1, -1};
    out[43] = Row{'h', &o.r43, 1, -1, 3, -1, -1};
    out[44] = Row{'h', &o.r44, 1, -1, 4, -1, -1};
    out[45] = Row{'h', &o.r45, 1, -1, 5, -1, -1};
    out[46] = Row{'h', &o.r46, 1, -1, 6, -1, -1};
    out[47] = Row{'h', &o.r47, 1, -1, 7, -1, -1};
    out[48] = Row{'h', &o.r48, 1, -1, 8, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot349Rva == 0x7bde50 && kSlot349Size == 11443 && kSlot349RowCount == 49);
    CHECK(is_known_float_arg_functor(kSlot349FloatFnTag));
    CHECK(offsetof(Slot349Objects, r00) == 0x0);
    CHECK(offsetof(Slot349Objects, r01) == 0xd8);
    CHECK(offsetof(Slot349Objects, r02) == 0x1b0);
    CHECK(offsetof(Slot349Objects, r03) == 0x288);
    CHECK(offsetof(Slot349Objects, r04) == 0x360);
    CHECK(offsetof(Slot349Objects, r05) == 0x438);
    CHECK(offsetof(Slot349Objects, r06) == 0x510);
    CHECK(offsetof(Slot349Objects, r07) == 0x5e8);
    CHECK(offsetof(Slot349Objects, r08) == 0x6b0);
    CHECK(offsetof(Slot349Objects, r09) == 0x788);
    CHECK(offsetof(Slot349Objects, r10) == 0x860);
    CHECK(offsetof(Slot349Objects, r11) == 0x938);
    CHECK(offsetof(Slot349Objects, r12) == 0xa10);
    CHECK(offsetof(Slot349Objects, r13) == 0xae8);
    CHECK(offsetof(Slot349Objects, r14) == 0xbc0);
    CHECK(offsetof(Slot349Objects, r15) == 0xc98);
    CHECK(offsetof(Slot349Objects, r16) == 0xd70);
    CHECK(offsetof(Slot349Objects, r17) == 0xe48);
    CHECK(offsetof(Slot349Objects, r18) == 0xf20);
    CHECK(offsetof(Slot349Objects, r19) == 0xff8);
    CHECK(offsetof(Slot349Objects, r20) == 0x10d0);
    CHECK(offsetof(Slot349Objects, r21) == 0x11a8);
    CHECK(offsetof(Slot349Objects, r22) == 0x1280);
    CHECK(offsetof(Slot349Objects, r23) == 0x1358);
    CHECK(offsetof(Slot349Objects, r24) == 0x1420);
    CHECK(offsetof(Slot349Objects, r25) == 0x14e8);
    CHECK(offsetof(Slot349Objects, r26) == 0x15b0);
    CHECK(offsetof(Slot349Objects, r27) == 0x1688);
    CHECK(offsetof(Slot349Objects, r28) == 0x1750);
    CHECK(offsetof(Slot349Objects, r29) == 0x1818);
    CHECK(offsetof(Slot349Objects, r30) == 0x18e0);
    CHECK(offsetof(Slot349Objects, r31) == 0x19a8);
    CHECK(offsetof(Slot349Objects, r32) == 0x1a80);
    CHECK(offsetof(Slot349Objects, r33) == 0x1b58);
    CHECK(offsetof(Slot349Objects, r34) == 0x1c30);
    CHECK(offsetof(Slot349Objects, r35) == 0x1d08);
    CHECK(offsetof(Slot349Objects, r36) == 0x1de0);
    CHECK(offsetof(Slot349Objects, r37) == 0x1ea8);
    CHECK(offsetof(Slot349Objects, r38) == 0x1f50);
    CHECK(offsetof(Slot349Objects, r39) == 0x1ff8);
    CHECK(offsetof(Slot349Objects, r40) == 0x20a0);
    CHECK(offsetof(Slot349Objects, r41) == 0x2148);
    CHECK(offsetof(Slot349Objects, r42) == 0x2238);
    CHECK(offsetof(Slot349Objects, r43) == 0x2268);
    CHECK(offsetof(Slot349Objects, r44) == 0x2298);
    CHECK(offsetof(Slot349Objects, r45) == 0x22c8);
    CHECK(offsetof(Slot349Objects, r46) == 0x22f8);
    CHECK(offsetof(Slot349Objects, r47) == 0x2328);
    CHECK(offsetof(Slot349Objects, r48) == 0x2358);
    CHECK(sizeof(Slot349Objects) == 0x2388);
    for (std::size_t n = 0; n < kSlot349BoolCount; ++n) CHECK(std::strlen(kSlot349BoolNames[n]) == kSlot349BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot349FloatCount; ++n) CHECK(std::strlen(kSlot349FloatNames[n]) == kSlot349FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot349StringCount; ++n) CHECK(std::strlen(kSlot349MenuStrings[n]) == kSlot349MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot349BoolFnTag, kSlot349BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot349BoolNames[row.var], kSlot349BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot349VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot349FloatNames[row.var], kSlot349FloatNameLengths[row.var]));
            CHECK(v.value == kSlot349FloatValueBits[row.var] && v.default_value == kSlot349FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot349FloatLoBits[row.var] && hi == kSlot349FloatHiBits[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot349MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot349MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == (row.var == 0 ? kSlot349HolderVptr : kSlot349HolderVptrB) && h.tail == 0 && same_view(h.name, kSlot349MenuStrings[row.s0], kSlot349MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot349ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot349MenuStrings[row.s0], kSlot349MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot349MenuStrings[row.s1], kSlot349MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot349MenuStrings[row.s1], kSlot349MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's') {
                CHECK(it.range_min == bits_to_float(kSlot349SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot349SliderHiBits[row.slider]));
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
    Slot349Objects o;
    poison(o);
    const Slot349Result r = init_slot349_7bde50(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot349RowCount);
    Row rows[kSlot349RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot349RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot349BoolCount + kSlot349FloatCount + kSlot349UFloatCount);
    CHECK(menu.root.count == items && items == 5 && menu.root.link.next == &o.r37.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot349CellCount && creg.atexit_log.size() == kSlot349RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot349RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? (rows[i].var == 0 ? kSlot349HolderDtorRva : kSlot349HolderDtorRvaB) : 0x272d0;
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
        Slot349Objects o;
        const Slot349Result r = init_slot349_7bde50(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot349Objects o;
        const Slot349Result r = init_slot349_7bde50(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 37);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_349");
    return 0;
}
