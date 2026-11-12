// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 352 (0x7a2390): 0 bool and 6 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_352_7a2390.hpp"

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

Row rows_of(Slot352Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'f', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'f', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'f', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'i', &o.r03, 0, -1, -1, -1, -1};
    out[4] = Row{'f', &o.r04, 3, -1, -1, -1, -1};
    out[5] = Row{'f', &o.r05, 4, -1, -1, -1, -1};
    out[6] = Row{'f', &o.r06, 5, -1, -1, -1, -1};
    out[7] = Row{'u', &o.r07, 0, -1, -1, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot352Rva == 0x7a2390 && kSlot352Size == 2290 && kSlot352RowCount == 8);
    CHECK(is_known_float_arg_functor(kSlot352FloatFnTag));
    CHECK(is_int_functor_tag(kSlot352IntFnTag));
    CHECK(is_float_functor_tag(kSlot352UFloatFnTag));
    CHECK(offsetof(Slot352Objects, r00) == 0x0);
    CHECK(offsetof(Slot352Objects, r01) == 0xd8);
    CHECK(offsetof(Slot352Objects, r02) == 0x1b0);
    CHECK(offsetof(Slot352Objects, r03) == 0x288);
    CHECK(offsetof(Slot352Objects, r04) == 0x360);
    CHECK(offsetof(Slot352Objects, r05) == 0x438);
    CHECK(offsetof(Slot352Objects, r06) == 0x510);
    CHECK(offsetof(Slot352Objects, r07) == 0x5e8);
    CHECK(sizeof(Slot352Objects) == 0x6c0);
    for (std::size_t n = 0; n < kSlot352FloatCount; ++n) CHECK(std::strlen(kSlot352FloatNames[n]) == kSlot352FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot352UFloatCount; ++n) CHECK(std::strlen(kSlot352UFloatNames[n]) == kSlot352UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot352IntCount; ++n) CHECK(std::strlen(kSlot352IntNames[n]) == kSlot352IntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot352StringCount; ++n) CHECK(std::strlen(kSlot352MenuStrings[n]) == kSlot352MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'i': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == kSlot352VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot352IntNames[row.var], kSlot352IntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot352IntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot352IntValue[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot352IntMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot352IntMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot352VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot352FloatNames[row.var], kSlot352FloatNameLengths[row.var]));
            CHECK(v.value == kSlot352FloatValueBits[row.var] && v.default_value == kSlot352FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot352FloatLoBits[row.var] && hi == kSlot352FloatHiBits[row.var]);
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot352VarFlags);
            CHECK(v.value == kSlot352UFloatValueBits[row.var] && v.default_value == kSlot352UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot352UFloatNames[row.var], kSlot352UFloatNameLengths[row.var]));
            if (kSlot352UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot352MenuStrings[kSlot352UFloatDesc[row.var]], kSlot352MenuStringLengths[kSlot352UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot352MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot352MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot352ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot352MenuStrings[row.s0], kSlot352MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot352MenuStrings[row.s1], kSlot352MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot352MenuStrings[row.s1], kSlot352MenuStringLengths[row.s1]));
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
    Slot352Objects o;
    poison(o);
    const Slot352Result r = init_slot352_7a2390(o, creg);
    CHECK(r.ok && r.rows_done == kSlot352RowCount);
    Row rows[kSlot352RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot352RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot352BoolCount + kSlot352IntCount + kSlot352FloatCount + kSlot352UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot352CellCount && creg.atexit_log.size() == kSlot352RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot352RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : rows[i].kind == 'i' ? 0x18a60 : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot352Objects o;
        const Slot352Result r = init_slot352_7a2390(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_352");
    return 0;
}
