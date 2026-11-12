// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 503 (0x247f00): 3 bool and 0 bounded float variables and 1 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_503_247f00.hpp"

namespace {
using namespace l4;


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

Row rows_of(Slot503Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'o', &o.r00, -1, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 0, -1, -1, -1, -1, nullptr};
    out[2] = Row{'u', &o.r02, 0, -1, -1, -1, -1, nullptr};
    out[3] = Row{'b', &o.r03, 1, -1, -1, -1, -1, nullptr};
    out[4] = Row{'m', &o.r04, 0, -1, -1, -1, -1, nullptr};
    out[5] = Row{'m', &o.r05, 1, -1, -1, -1, -1, nullptr};
    out[6] = Row{'m', &o.r06, 2, -1, -1, -1, -1, nullptr};
    out[7] = Row{'m', &o.r07, 3, -1, -1, -1, -1, nullptr};
    out[8] = Row{'m', &o.r08, 4, -1, -1, -1, -1, nullptr};
    out[9] = Row{'m', &o.r09, 5, -1, -1, -1, -1, nullptr};
    out[10] = Row{'m', &o.r10, 6, -1, -1, -1, -1, nullptr};
    out[11] = Row{'m', &o.r11, 7, -1, -1, -1, -1, nullptr};
    out[12] = Row{'m', &o.r12, 8, -1, -1, -1, -1, nullptr};
    out[13] = Row{'b', &o.r13, 2, -1, -1, -1, -1, nullptr};
    out[14] = Row{'e', &o.r14, -1, 13, 13, 14, -1, nullptr};
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
    CHECK(kSlot503Rva == 0x247f00 && kSlot503Size == 3185 && kSlot503RowCount == 15);
    CHECK(is_float_functor_tag(kSlot503UFloatFnTag));
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot503Objects, r00) == 0x0);
    CHECK(offsetof(Slot503Objects, r01) == 0x18);
    CHECK(offsetof(Slot503Objects, r02) == 0xe0);
    CHECK(offsetof(Slot503Objects, r03) == 0x1b8);
    CHECK(offsetof(Slot503Objects, r04) == 0x280);
    CHECK(offsetof(Slot503Objects, r05) == 0x320);
    CHECK(offsetof(Slot503Objects, r06) == 0x3c0);
    CHECK(offsetof(Slot503Objects, r07) == 0x460);
    CHECK(offsetof(Slot503Objects, r08) == 0x500);
    CHECK(offsetof(Slot503Objects, r09) == 0x5a0);
    CHECK(offsetof(Slot503Objects, r10) == 0x640);
    CHECK(offsetof(Slot503Objects, r11) == 0x6e0);
    CHECK(offsetof(Slot503Objects, r12) == 0x780);
    CHECK(offsetof(Slot503Objects, r13) == 0x820);
    CHECK(offsetof(Slot503Objects, r14) == 0x8e8);
    CHECK(sizeof(Slot503Objects) == 0x990);

    for (std::size_t n = 0; n < kSlot503BoolCount; ++n) CHECK(std::strlen(kSlot503BoolNames[n]) == kSlot503BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot503UFloatCount; ++n) CHECK(std::strlen(kSlot503UFloatNames[n]) == kSlot503UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot503CmdCount; ++n) CHECK(std::strlen(kSlot503CmdNames[n]) == kSlot503CmdNameLengths[n] && kSlot503CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot503StringCount; ++n) CHECK(std::strlen(kSlot503MenuStrings[n]) == kSlot503MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot503BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot503BoolFnTag, kSlot503BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot503BoolFnTag, kSlot503BoolValues[row.var], kSlot503MenuStrings[kSlot503BoolDesc[row.var]], kSlot503MenuStringLengths[kSlot503BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot503BoolNames[row.var], kSlot503BoolNameLengths[row.var]));
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot503VarFlags);
            CHECK(v.value == kSlot503UFloatValueBits[row.var] && v.default_value == kSlot503UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot503UFloatNames[row.var], kSlot503UFloatNameLengths[row.var]));
            if (kSlot503UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot503MenuStrings[kSlot503UFloatDesc[row.var]], kSlot503MenuStringLengths[kSlot503UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot503CmdNames[row.var], kSlot503MenuStrings[kSlot503CmdDesc[row.var]], kSlot503MenuStringLengths[kSlot503CmdDesc[row.var]], kSlot503CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot503MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot503MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'o': {
            const auto& c = *static_cast<const NullNameCell247f47*>(row.obj);
            CHECK(c.id == 0 && c.name[0] == 'N' && c.name[1] == 'U' && c.name[2] == 'L' && c.name[3] == 'L');
            for (int b = 4; b < 16; ++b) CHECK(c.name[b] == 0);   // strncpy pads with NULs up to 15 bytes; byte 0x13 is stored 0
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
            CHECK(it.flags == kSlot503ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot503MenuStrings[row.s0], kSlot503MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot503MenuStrings[row.s1], kSlot503MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot503MenuStrings[row.var], kSlot503MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot503MenuStrings[row.s1], kSlot503MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot503MenuStrings[row.s1], kSlot503MenuStringLengths[row.s1]));
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
    Slot503Objects o;
    poison(o);
    const Slot503Result r = init_slot503_247f00(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot503RowCount);
    Row rows[kSlot503RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot503RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c' || kd == 'o') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'p' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot503BoolCount + kSlot503IntCount + kSlot503LocalIntCount + kSlot503UIntCount + kSlot503CmdCount + kSlot503StrVarCount + kSlot503FloatCount + kSlot503UFloatCount);
    CHECK(menu.root.count == items && items == 1 && menu.root.link.next == &o.r14.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r01.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot503CellCount && creg.atexit_log.size() == kSlot503RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot503RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : 0x272d0;

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
        Slot503Objects o;
        const Slot503Result r = init_slot503_247f00(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot503Objects o;
        const Slot503Result r = init_slot503_247f00(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 14);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_503");
    return 0;
}
