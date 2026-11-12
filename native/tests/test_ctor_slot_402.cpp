// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 402 (0x610430): 3 bool and 3 bounded float variables and 5 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_402_610430.hpp"

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

Row rows_of(Slot402Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'f', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'f', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'j', &o.r02, 0, -1, -1, -1, -1, nullptr};
    out[3] = Row{'b', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'b', &o.r04, 1, -1, -1, -1, -1, nullptr};
    out[5] = Row{'z', &o.r05, 0, -1, 0, -1, -1, nullptr};
    out[6] = Row{'f', &o.r06, 2, -1, -1, -1, -1, nullptr};
    out[7] = Row{'b', &o.r07, 2, -1, -1, -1, -1, nullptr};
    out[8] = Row{'t', &o.r08, -1, 2, 1, -1, -1, nullptr};
    out[9] = Row{'t', &o.r09, -1, 4, 1, -1, -1, nullptr};
    out[10] = Row{'s', &o.r10, -1, 6, 1, -1, 0, nullptr};
    out[11] = Row{'t', &o.r11, -1, 3, 1, -1, -1, nullptr};
    out[12] = Row{'t', &o.r12, -1, 7, 1, -1, -1, nullptr};
    out[13] = Row{'h', &o.r13, 0, -1, 2, -1, -1, nullptr};
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
    CHECK(kSlot402Rva == 0x610430 && kSlot402Size == 2663 && kSlot402RowCount == 14);
    CHECK(is_known_float_arg_functor(kSlot402FloatFnTag));
    CHECK(sizeof(ConCmdStringVar) == 0x148);
    for (std::size_t n = 0; n < kSlot402StrVarCount; ++n) CHECK(std::strlen(kSlot402StrNames[n]) == kSlot402StrNameLengths[n]);
    CHECK(offsetof(Slot402Objects, r00) == 0x0);
    CHECK(offsetof(Slot402Objects, r01) == 0xd8);
    CHECK(offsetof(Slot402Objects, r02) == 0x1b0);
    CHECK(offsetof(Slot402Objects, r03) == 0x288);
    CHECK(offsetof(Slot402Objects, r04) == 0x350);
    CHECK(offsetof(Slot402Objects, r05) == 0x418);
    CHECK(offsetof(Slot402Objects, r06) == 0x560);
    CHECK(offsetof(Slot402Objects, r07) == 0x638);
    CHECK(offsetof(Slot402Objects, r08) == 0x700);
    CHECK(offsetof(Slot402Objects, r09) == 0x7a8);
    CHECK(offsetof(Slot402Objects, r10) == 0x850);
    CHECK(offsetof(Slot402Objects, r11) == 0x8f8);
    CHECK(offsetof(Slot402Objects, r12) == 0x9a0);
    CHECK(offsetof(Slot402Objects, r13) == 0xa48);
    CHECK(sizeof(Slot402Objects) == 0xa78);

    for (std::size_t n = 0; n < kSlot402BoolCount; ++n) CHECK(std::strlen(kSlot402BoolNames[n]) == kSlot402BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot402FloatCount; ++n) CHECK(std::strlen(kSlot402FloatNames[n]) == kSlot402FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot402StringCount; ++n) CHECK(std::strlen(kSlot402MenuStrings[n]) == kSlot402MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot402BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot402BoolFnTag, kSlot402BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot402BoolFnTag, kSlot402BoolValues[row.var], kSlot402MenuStrings[kSlot402BoolDesc[row.var]], kSlot402MenuStringLengths[kSlot402BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot402BoolNames[row.var], kSlot402BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot402VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot402FloatNames[row.var], kSlot402FloatNameLengths[row.var]));
            CHECK(v.value == kSlot402FloatValueBits[row.var] && v.default_value == kSlot402FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot402FloatLoBits[row.var] && hi == kSlot402FloatHiBits[row.var]);
            break;
        }
        case 'z': {
            const auto& v = *static_cast<const ConCmdStringVar*>(row.obj);
            const std::int32_t d = kSlot402StrInit[row.var];
            CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == kStringVarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot402StrNames[row.var], kSlot402StrNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(same_view(v.default_value, kSlot402MenuStrings[d], kSlot402MenuStringLengths[d]) && same_view(v.value, kSlot402MenuStrings[d], kSlot402MenuStringLengths[d]));
            CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot402MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot402MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
            break;
        }
        case 'j': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kSlot402EnumFinalVptr && v.base.flags == kSlot402VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot402EnumNames[row.var], kSlot402EnumNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot402EnumValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot402EnumValue[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot402EnumMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot402EnumMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kSlot402EnumFnTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == kSlot402HolderVptr && h.tail == 0 && same_view(h.name, kSlot402MenuStrings[row.s0], kSlot402MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot402ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot402MenuStrings[row.s0], kSlot402MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot402MenuStrings[row.s1], kSlot402MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot402MenuStrings[row.s1], kSlot402MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's' || row.kind == 'S') {
                CHECK(it.range_min == bits_to_float(kSlot402SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot402SliderHiBits[row.slider]));
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
    Slot402Objects o;
    poison(o);
    const Slot402Result r = init_slot402_610430(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot402RowCount);
    Row rows[kSlot402RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot402RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot402BoolCount + kSlot402IntCount + kSlot402LocalIntCount + kSlot402UIntCount + kSlot402CmdCount + kSlot402StrVarCount + kSlot402FloatCount + kSlot402UFloatCount + kSlot402EnumCount);
    CHECK(menu.root.count == items && items == 5 && menu.root.link.next == &o.r08.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot402CellCount && creg.atexit_log.size() == kSlot402RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot402RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? kSlot402HolderDtorRva : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'j' ? kSlot402EnumDtorRva : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : 0x272d0;

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
        Slot402Objects o;
        const Slot402Result r = init_slot402_610430(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot402Objects o;
        const Slot402Result r = init_slot402_610430(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_402");
    return 0;
}
