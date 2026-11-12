// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 571 (0xc780): 26 bool and 13 bounded float variables and 1 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_571_c780.hpp"

namespace {
using namespace l4;

int g_external_0 = 0;   // stands for the object at 0x2dfcbd0, which another slot builds

struct Row {
    char kind;            // b bool, i/l/n ranged / local / unbounded int, f/u bounded / unbounded float, m console command, t 0x26db0 toggle, e 0x26ea0 toggle, s 0x27120 slider, S 0x27050 slider, g group
    const void* obj;
    int var;              // index into the bool / float tables (variables)
    int target;           // row index of the target variable (items)
    int s0;               // menu string: parent / label
    int s1;               // menu string: explicit text / caption
    int slider;           // slider table index
    const void* ext;      // identity pointer of an external slider target (target == -1), otherwise nullptr
};

Row rows_of(Slot571Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1, nullptr};
    out[3] = Row{'b', &o.r03, 3, -1, -1, -1, -1, nullptr};
    out[4] = Row{'b', &o.r04, 4, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 5, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 6, -1, -1, -1, -1, nullptr};
    out[7] = Row{'b', &o.r07, 7, -1, -1, -1, -1, nullptr};
    out[8] = Row{'b', &o.r08, 8, -1, -1, -1, -1, nullptr};
    out[9] = Row{'b', &o.r09, 9, -1, -1, -1, -1, nullptr};
    out[10] = Row{'b', &o.r10, 10, -1, -1, -1, -1, nullptr};
    out[11] = Row{'b', &o.r11, 11, -1, -1, -1, -1, nullptr};
    out[12] = Row{'b', &o.r12, 12, -1, -1, -1, -1, nullptr};
    out[13] = Row{'b', &o.r13, 13, -1, -1, -1, -1, nullptr};
    out[14] = Row{'b', &o.r14, 14, -1, -1, -1, -1, nullptr};
    out[15] = Row{'b', &o.r15, 15, -1, -1, -1, -1, nullptr};
    out[16] = Row{'b', &o.r16, 16, -1, -1, -1, -1, nullptr};
    out[17] = Row{'b', &o.r17, 17, -1, -1, -1, -1, nullptr};
    out[18] = Row{'b', &o.r18, 18, -1, -1, -1, -1, nullptr};
    out[19] = Row{'b', &o.r19, 19, -1, -1, -1, -1, nullptr};
    out[20] = Row{'f', &o.r20, 0, -1, -1, -1, -1, nullptr};
    out[21] = Row{'f', &o.r21, 1, -1, -1, -1, -1, nullptr};
    out[22] = Row{'f', &o.r22, 2, -1, -1, -1, -1, nullptr};
    out[23] = Row{'f', &o.r23, 3, -1, -1, -1, -1, nullptr};
    out[24] = Row{'f', &o.r24, 4, -1, -1, -1, -1, nullptr};
    out[25] = Row{'f', &o.r25, 5, -1, -1, -1, -1, nullptr};
    out[26] = Row{'f', &o.r26, 6, -1, -1, -1, -1, nullptr};
    out[27] = Row{'f', &o.r27, 7, -1, -1, -1, -1, nullptr};
    out[28] = Row{'f', &o.r28, 8, -1, -1, -1, -1, nullptr};
    out[29] = Row{'f', &o.r29, 9, -1, -1, -1, -1, nullptr};
    out[30] = Row{'f', &o.r30, 10, -1, -1, -1, -1, nullptr};
    out[31] = Row{'b', &o.r31, 20, -1, -1, -1, -1, nullptr};
    out[32] = Row{'b', &o.r32, 21, -1, -1, -1, -1, nullptr};
    out[33] = Row{'f', &o.r33, 11, -1, -1, -1, -1, nullptr};
    out[34] = Row{'f', &o.r34, 12, -1, -1, -1, -1, nullptr};
    out[35] = Row{'b', &o.r35, 22, -1, -1, -1, -1, nullptr};
    out[36] = Row{'b', &o.r36, 23, -1, -1, -1, -1, nullptr};
    out[37] = Row{'b', &o.r37, 24, -1, -1, -1, -1, nullptr};
    out[38] = Row{'b', &o.r38, 25, -1, -1, -1, -1, nullptr};
    out[39] = Row{'m', &o.r39, 0, -1, -1, -1, -1, nullptr};
    out[40] = Row{'m', &o.r40, 1, -1, -1, -1, -1, nullptr};
    out[41] = Row{'m', &o.r41, 2, -1, -1, -1, -1, nullptr};
    out[42] = Row{'m', &o.r42, 3, -1, -1, -1, -1, nullptr};
    out[43] = Row{'x', &o.r43, 6, -1, 4, 5, -1, o.external_target_0};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    return dtor;
}

void section_constants() {
    CHECK(kSlot571Rva == 0xc780 && kSlot571Size == 10644 && kSlot571RowCount == 44);
    CHECK(is_known_float_arg_functor(kSlot571FloatFnTag));
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot571Objects, r00) == 0x0);
    CHECK(offsetof(Slot571Objects, r01) == 0xc8);
    CHECK(offsetof(Slot571Objects, r02) == 0x198);
    CHECK(offsetof(Slot571Objects, r03) == 0x260);
    CHECK(offsetof(Slot571Objects, r04) == 0x328);
    CHECK(offsetof(Slot571Objects, r05) == 0x3f0);
    CHECK(offsetof(Slot571Objects, r06) == 0x4b8);
    CHECK(offsetof(Slot571Objects, r07) == 0x580);
    CHECK(offsetof(Slot571Objects, r08) == 0x648);
    CHECK(offsetof(Slot571Objects, r09) == 0x710);
    CHECK(offsetof(Slot571Objects, r10) == 0x7d8);
    CHECK(offsetof(Slot571Objects, r11) == 0x8a0);
    CHECK(offsetof(Slot571Objects, r12) == 0x968);
    CHECK(offsetof(Slot571Objects, r13) == 0xa30);
    CHECK(offsetof(Slot571Objects, r14) == 0xaf8);
    CHECK(offsetof(Slot571Objects, r15) == 0xbc0);
    CHECK(offsetof(Slot571Objects, r16) == 0xc88);
    CHECK(offsetof(Slot571Objects, r17) == 0xd50);
    CHECK(offsetof(Slot571Objects, r18) == 0xe18);
    CHECK(offsetof(Slot571Objects, r19) == 0xee0);
    CHECK(offsetof(Slot571Objects, r20) == 0xfa8);
    CHECK(offsetof(Slot571Objects, r21) == 0x1080);
    CHECK(offsetof(Slot571Objects, r22) == 0x1158);
    CHECK(offsetof(Slot571Objects, r23) == 0x1230);
    CHECK(offsetof(Slot571Objects, r24) == 0x1308);
    CHECK(offsetof(Slot571Objects, r25) == 0x13e0);
    CHECK(offsetof(Slot571Objects, r26) == 0x14b8);
    CHECK(offsetof(Slot571Objects, r27) == 0x1590);
    CHECK(offsetof(Slot571Objects, r28) == 0x1668);
    CHECK(offsetof(Slot571Objects, r29) == 0x1740);
    CHECK(offsetof(Slot571Objects, r30) == 0x1818);
    CHECK(offsetof(Slot571Objects, r31) == 0x18f0);
    CHECK(offsetof(Slot571Objects, r32) == 0x19b8);
    CHECK(offsetof(Slot571Objects, r33) == 0x1a80);
    CHECK(offsetof(Slot571Objects, r34) == 0x1b58);
    CHECK(offsetof(Slot571Objects, r35) == 0x1c30);
    CHECK(offsetof(Slot571Objects, r36) == 0x1cf8);
    CHECK(offsetof(Slot571Objects, r37) == 0x1dc0);
    CHECK(offsetof(Slot571Objects, r38) == 0x1e88);
    CHECK(offsetof(Slot571Objects, r39) == 0x2600);
    CHECK(offsetof(Slot571Objects, r40) == 0x26a0);
    CHECK(offsetof(Slot571Objects, r41) == 0x2740);
    CHECK(offsetof(Slot571Objects, r42) == 0x27e0);
    CHECK(offsetof(Slot571Objects, r43) == 0x2880);
    CHECK(sizeof(Slot571Objects) == 0x2930);

    for (std::size_t n = 0; n < kSlot571BoolCount; ++n) CHECK(std::strlen(kSlot571BoolNames[n]) == kSlot571BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot571FloatCount; ++n) CHECK(std::strlen(kSlot571FloatNames[n]) == kSlot571FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot571CmdCount; ++n) CHECK(std::strlen(kSlot571CmdNames[n]) == kSlot571CmdNameLengths[n] && kSlot571CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot571StringCount; ++n) CHECK(std::strlen(kSlot571MenuStrings[n]) == kSlot571MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot571BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot571BoolFnTag, kSlot571BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot571BoolFnTag, kSlot571BoolValues[row.var], kSlot571MenuStrings[kSlot571BoolDesc[row.var]], kSlot571MenuStringLengths[kSlot571BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot571BoolNames[row.var], kSlot571BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot571VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot571FloatNames[row.var], kSlot571FloatNameLengths[row.var]));
            CHECK(v.value == kSlot571FloatValueBits[row.var] && v.default_value == kSlot571FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot571FloatLoBits[row.var] && hi == kSlot571FloatHiBits[row.var]);
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot571CmdNames[row.var], kSlot571MenuStrings[kSlot571CmdDesc[row.var]], kSlot571MenuStringLengths[kSlot571CmdDesc[row.var]], kSlot571CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot571MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot571MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot571ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot571MenuStrings[row.s0], kSlot571MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot571MenuStrings[row.s1], kSlot571MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot571MenuStrings[row.var], kSlot571MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot571MenuStrings[row.s1], kSlot571MenuStringLengths[row.s1]));
                break;
            }
            if (row.target < 0) {
                CHECK(row.kind == 'S' && row.ext != nullptr && it.kind == 2 && it.target == row.ext);   // external target: identity pointer only
                CHECK(same_view(it.text, kSlot571MenuStrings[row.s1], kSlot571MenuStringLengths[row.s1]));
               CHECK(false);   // no slider rows in this slot
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot571MenuStrings[row.s1], kSlot571MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's' || row.kind == 'S') {
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
    Slot571Objects o;
    poison(o);
    o.external_target_0 = &g_external_0;
    const Slot571Result r = init_slot571_c780(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot571RowCount);
    Row rows[kSlot571RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot571RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot571BoolCount + kSlot571IntCount + kSlot571LocalIntCount + kSlot571UIntCount + kSlot571CmdCount + kSlot571StrVarCount + kSlot571FloatCount + kSlot571UFloatCount);
    CHECK(menu.root.count == items && items == 1 && menu.root.link.next == &o.r43.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot571CellCount && creg.atexit_log.size() == kSlot571RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot571RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : 0x272d0;

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
        Slot571Objects o;
        const Slot571Result r = init_slot571_c780(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot571Objects o;
        const Slot571Result r = init_slot571_c780(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 43);
    }
    {
        // An external slider target the host did not supply: that row is refused visibly (rows before it were built).
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot571Objects o;
        poison(o);
        o.external_target_0 = nullptr;
        const Slot571Result r = init_slot571_c780(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 43);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_571");
    return 0;
}
