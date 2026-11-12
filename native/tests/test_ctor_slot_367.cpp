// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 367 (0x740980): 6 bool and 1 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_367_740980.hpp"

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

Row rows_of(Slot367Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'b', &o.r03, 3, -1, -1, -1, -1};
    out[4] = Row{'u', &o.r04, 0, -1, -1, -1, -1};
    out[5] = Row{'u', &o.r05, 1, -1, -1, -1, -1};
    out[6] = Row{'u', &o.r06, 2, -1, -1, -1, -1};
    out[7] = Row{'u', &o.r07, 3, -1, -1, -1, -1};
    out[8] = Row{'u', &o.r08, 4, -1, -1, -1, -1};
    out[9] = Row{'u', &o.r09, 5, -1, -1, -1, -1};
    out[10] = Row{'u', &o.r10, 6, -1, -1, -1, -1};
    out[11] = Row{'u', &o.r11, 7, -1, -1, -1, -1};
    out[12] = Row{'u', &o.r12, 8, -1, -1, -1, -1};
    out[13] = Row{'u', &o.r13, 9, -1, -1, -1, -1};
    out[14] = Row{'b', &o.r14, 4, -1, -1, -1, -1};
    out[15] = Row{'u', &o.r15, 10, -1, -1, -1, -1};
    out[16] = Row{'u', &o.r16, 11, -1, -1, -1, -1};
    out[17] = Row{'b', &o.r17, 5, -1, -1, -1, -1};
    out[18] = Row{'f', &o.r18, 0, -1, -1, -1, -1};
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
    CHECK(kSlot367Rva == 0x740980 && kSlot367Size == 4701 && kSlot367RowCount == 19);
    CHECK(is_known_float_arg_functor(kSlot367FloatFnTag));
    CHECK(is_float_functor_tag(kSlot367UFloatFnTag));
    CHECK(offsetof(Slot367Objects, r00) == 0x0);
    CHECK(offsetof(Slot367Objects, r01) == 0xc8);
    CHECK(offsetof(Slot367Objects, r02) == 0x190);
    CHECK(offsetof(Slot367Objects, r03) == 0x258);
    CHECK(offsetof(Slot367Objects, r04) == 0x320);
    CHECK(offsetof(Slot367Objects, r05) == 0x3f8);
    CHECK(offsetof(Slot367Objects, r06) == 0x4d0);
    CHECK(offsetof(Slot367Objects, r07) == 0x5a8);
    CHECK(offsetof(Slot367Objects, r08) == 0x680);
    CHECK(offsetof(Slot367Objects, r09) == 0x758);
    CHECK(offsetof(Slot367Objects, r10) == 0x830);
    CHECK(offsetof(Slot367Objects, r11) == 0x908);
    CHECK(offsetof(Slot367Objects, r12) == 0x9e0);
    CHECK(offsetof(Slot367Objects, r13) == 0xab8);
    CHECK(offsetof(Slot367Objects, r14) == 0xb90);
    CHECK(offsetof(Slot367Objects, r15) == 0xc58);
    CHECK(offsetof(Slot367Objects, r16) == 0xd30);
    CHECK(offsetof(Slot367Objects, r17) == 0xe08);
    CHECK(offsetof(Slot367Objects, r18) == 0xed0);
    CHECK(sizeof(Slot367Objects) == 0xfa8);
    for (std::size_t n = 0; n < kSlot367BoolCount; ++n) CHECK(std::strlen(kSlot367BoolNames[n]) == kSlot367BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot367FloatCount; ++n) CHECK(std::strlen(kSlot367FloatNames[n]) == kSlot367FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot367UFloatCount; ++n) CHECK(std::strlen(kSlot367UFloatNames[n]) == kSlot367UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot367StringCount; ++n) CHECK(std::strlen(kSlot367MenuStrings[n]) == kSlot367MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot367BoolFnTag, kSlot367BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot367BoolNames[row.var], kSlot367BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot367VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot367FloatNames[row.var], kSlot367FloatNameLengths[row.var]));
            CHECK(v.value == kSlot367FloatValueBits[row.var] && v.default_value == kSlot367FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot367FloatLoBits[row.var] && hi == kSlot367FloatHiBits[row.var]);
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot367VarFlags);
            CHECK(v.value == kSlot367UFloatValueBits[row.var] && v.default_value == kSlot367UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot367UFloatNames[row.var], kSlot367UFloatNameLengths[row.var]));
            if (kSlot367UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot367MenuStrings[kSlot367UFloatDesc[row.var]], kSlot367MenuStringLengths[kSlot367UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot367MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot367MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot367ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot367MenuStrings[row.s0], kSlot367MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot367MenuStrings[row.s1], kSlot367MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot367MenuStrings[row.s1], kSlot367MenuStringLengths[row.s1]));
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
    Slot367Objects o;
    poison(o);
    const Slot367Result r = init_slot367_740980(o, creg);
    CHECK(r.ok && r.rows_done == kSlot367RowCount);
    Row rows[kSlot367RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot367RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot367BoolCount + kSlot367IntCount + kSlot367LocalIntCount + kSlot367FloatCount + kSlot367UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot367CellCount && creg.atexit_log.size() == kSlot367RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot367RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : rows[i].kind == 'i' ? 0x18a60 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot367Objects o;
        const Slot367Result r = init_slot367_740980(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_367");
    return 0;
}
