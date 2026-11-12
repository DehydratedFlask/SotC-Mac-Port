// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 508 (0x2241e0): 2 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_508_2241e0.hpp"

namespace {
using namespace l4;

std::uint32_t g_source_bits[kSlot508UFloatSourceCount] = {};   // the inputs section_alone supplied, read back by the row check

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

Row rows_of(Slot508Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'i', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'i', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'n', &o.r02, 0, -1, -1, -1, -1, nullptr};
    out[3] = Row{'n', &o.r03, 1, -1, -1, -1, -1, nullptr};
    out[4] = Row{'i', &o.r04, 2, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 0, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 1, -1, -1, -1, -1, nullptr};
    out[7] = Row{'u', &o.r07, 0, -1, -1, -1, -1, nullptr};
    out[8] = Row{'u', &o.r08, 1, -1, -1, -1, -1, nullptr};
    out[9] = Row{'u', &o.r09, 2, -1, -1, -1, -1, nullptr};
    out[10] = Row{'u', &o.r10, 3, -1, -1, -1, -1, nullptr};
    out[11] = Row{'u', &o.r11, 4, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot508Rva == 0x2241e0 && kSlot508Size == 3252 && kSlot508RowCount == 12);
    CHECK(is_int_functor_tag(kSlot508IntFnTag));
    CHECK(is_float_functor_tag(kSlot508UFloatFnTag));
    CHECK(is_int_functor_tag(kSlot508UIntFnTag));
    CHECK(offsetof(Slot508Objects, r00) == 0x0);
    CHECK(offsetof(Slot508Objects, r01) == 0xd8);
    CHECK(offsetof(Slot508Objects, r02) == 0x1b0);
    CHECK(offsetof(Slot508Objects, r03) == 0x288);
    CHECK(offsetof(Slot508Objects, r04) == 0x360);
    CHECK(offsetof(Slot508Objects, r05) == 0x438);
    CHECK(offsetof(Slot508Objects, r06) == 0x500);
    CHECK(offsetof(Slot508Objects, r07) == 0x5c8);
    CHECK(offsetof(Slot508Objects, r08) == 0x6a0);
    CHECK(offsetof(Slot508Objects, r09) == 0x778);
    CHECK(offsetof(Slot508Objects, r10) == 0x850);
    CHECK(offsetof(Slot508Objects, r11) == 0x928);
    CHECK(sizeof(Slot508Objects) == 0xa18);
    {
        // A default-constructed Objects carries the ELF initial image of every run-time loaded global (zero for .bss), not a value this model invented.
        Slot508Objects fresh;
        for (std::size_t k = 0; k < kSlot508UFloatSourceCount; ++k) CHECK(fresh.source_bits[k] == kSlot508SourceImageBits[k] && kSlot508SourceAddr[k] != 0);
    }

    for (std::size_t n = 0; n < kSlot508BoolCount; ++n) CHECK(std::strlen(kSlot508BoolNames[n]) == kSlot508BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot508UFloatCount; ++n) CHECK(std::strlen(kSlot508UFloatNames[n]) == kSlot508UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot508IntCount; ++n) CHECK(std::strlen(kSlot508IntNames[n]) == kSlot508IntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot508UIntCount; ++n) CHECK(std::strlen(kSlot508UIntNames[n]) == kSlot508UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot508StringCount; ++n) CHECK(std::strlen(kSlot508MenuStrings[n]) == kSlot508MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot508BoolFnTag, kSlot508BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot508BoolNames[row.var], kSlot508BoolNameLengths[row.var]));
            break;
        }
        case 'i': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == kSlot508VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot508IntNames[row.var], kSlot508IntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot508IntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot508IntValue[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot508IntMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot508IntMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot508VarFlags);
            CHECK(v.value == (kSlot508UFloatSource[row.var] < 0 ? kSlot508UFloatValueBits[row.var] : g_source_bits[kSlot508UFloatSource[row.var]]) && v.default_value == (kSlot508UFloatSource[row.var] < 0 ? kSlot508UFloatValueBits[row.var] : g_source_bits[kSlot508UFloatSource[row.var]]) && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot508UFloatNames[row.var], kSlot508UFloatNameLengths[row.var]));
            if (kSlot508UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot508MenuStrings[kSlot508UFloatDesc[row.var]], kSlot508MenuStringLengths[kSlot508UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'n': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot508VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot508UIntNames[row.var], kSlot508UIntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot508UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot508UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot508MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot508MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot508ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot508MenuStrings[row.s0], kSlot508MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot508MenuStrings[row.s1], kSlot508MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot508MenuStrings[row.s1], kSlot508MenuStringLengths[row.s1]));
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
    Slot508Objects o;
    poison(o);
    for (std::size_t k = 0; k < kSlot508UFloatSourceCount; ++k) {
        g_source_bits[k] = 0x3f800000u + static_cast<std::uint32_t>(k);   // distinct marker INPUTS (1.0f, ...), deliberately not the ELF image
        o.source_bits[k] = g_source_bits[k];
    }
    const Slot508Result r = init_slot508_2241e0(o, creg);
    CHECK(r.ok && r.rows_done == kSlot508RowCount);
    Row rows[kSlot508RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot508RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w') ++items;
    }
    CHECK(vars == kSlot508BoolCount + kSlot508IntCount + kSlot508LocalIntCount + kSlot508UIntCount + kSlot508CmdCount + kSlot508StrVarCount + kSlot508FloatCount + kSlot508UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot508CellCount && creg.atexit_log.size() == kSlot508RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot508RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot508Objects o;
        const Slot508Result r = init_slot508_2241e0(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_508");
    return 0;
}
