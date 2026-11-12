// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 381 (0x6cd5b0): 0 bool and 5 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_381_6cd5b0.hpp"

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
};

Row rows_of(Slot381Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'f', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'f', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'f', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'f', &o.r03, 3, -1, -1, -1, -1};
    out[4] = Row{'f', &o.r04, 4, -1, -1, -1, -1};
    out[5] = Row{'u', &o.r05, 0, -1, -1, -1, -1};
    out[6] = Row{'u', &o.r06, 1, -1, -1, -1, -1};
    out[7] = Row{'u', &o.r07, 2, -1, -1, -1, -1};
    out[8] = Row{'m', &o.r08, 0, -1, -1, -1, -1};
    out[9] = Row{'m', &o.r09, 1, -1, -1, -1, -1};
    out[10] = Row{'m', &o.r10, 2, -1, -1, -1, -1};
    out[11] = Row{'c', &o.r11, -1, -1, 4, -1, -1};
    out[12] = Row{'c', &o.r12, -1, -1, 5, -1, -1};
    out[13] = Row{'c', &o.r13, -1, -1, 6, -1, -1};
    out[14] = Row{'c', &o.r14, -1, -1, 7, -1, -1};
    out[15] = Row{'c', &o.r15, -1, -1, 8, -1, -1};
    out[16] = Row{'c', &o.r16, -1, -1, 9, -1, -1};
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
    CHECK(kSlot381Rva == 0x6cd5b0 && kSlot381Size == 3059 && kSlot381RowCount == 17);
    CHECK(is_known_float_arg_functor(kSlot381FloatFnTag));
    CHECK(is_float_functor_tag(kSlot381UFloatFnTag));
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot381Objects, r00) == 0x0);
    CHECK(offsetof(Slot381Objects, r01) == 0xd8);
    CHECK(offsetof(Slot381Objects, r02) == 0x1b0);
    CHECK(offsetof(Slot381Objects, r03) == 0x288);
    CHECK(offsetof(Slot381Objects, r04) == 0x360);
    CHECK(offsetof(Slot381Objects, r05) == 0x438);
    CHECK(offsetof(Slot381Objects, r06) == 0x510);
    CHECK(offsetof(Slot381Objects, r07) == 0x5e8);
    CHECK(offsetof(Slot381Objects, r08) == 0x6c0);
    CHECK(offsetof(Slot381Objects, r09) == 0x760);
    CHECK(offsetof(Slot381Objects, r10) == 0x800);
    CHECK(offsetof(Slot381Objects, r11) == 0x8a0);
    CHECK(offsetof(Slot381Objects, r12) == 0x8b0);
    CHECK(offsetof(Slot381Objects, r13) == 0x8c0);
    CHECK(offsetof(Slot381Objects, r14) == 0x8d0);
    CHECK(offsetof(Slot381Objects, r15) == 0x8e0);
    CHECK(offsetof(Slot381Objects, r16) == 0x8f0);
    CHECK(sizeof(Slot381Objects) == 0x900);
    for (std::size_t n = 0; n < kSlot381FloatCount; ++n) CHECK(std::strlen(kSlot381FloatNames[n]) == kSlot381FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot381UFloatCount; ++n) CHECK(std::strlen(kSlot381UFloatNames[n]) == kSlot381UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot381CmdCount; ++n) CHECK(std::strlen(kSlot381CmdNames[n]) == kSlot381CmdNameLengths[n] && kSlot381CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot381StringCount; ++n) CHECK(std::strlen(kSlot381MenuStrings[n]) == kSlot381MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot381VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot381FloatNames[row.var], kSlot381FloatNameLengths[row.var]));
            CHECK(v.value == kSlot381FloatValueBits[row.var] && v.default_value == kSlot381FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot381FloatLoBits[row.var] && hi == kSlot381FloatHiBits[row.var]);
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot381VarFlags);
            CHECK(v.value == kSlot381UFloatValueBits[row.var] && v.default_value == kSlot381UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot381UFloatNames[row.var], kSlot381UFloatNameLengths[row.var]));
            if (kSlot381UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot381MenuStrings[kSlot381UFloatDesc[row.var]], kSlot381MenuStringLengths[kSlot381UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot381CmdNames[row.var], kSlot381MenuStrings[kSlot381CmdDesc[row.var]], kSlot381MenuStringLengths[kSlot381CmdDesc[row.var]], kSlot381CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot381MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot381MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot381ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot381MenuStrings[row.s0], kSlot381MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot381MenuStrings[row.s1], kSlot381MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot381MenuStrings[row.s1], kSlot381MenuStringLengths[row.s1]));
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
    Slot381Objects o;
    poison(o);
    const Slot381Result r = init_slot381_6cd5b0(o, creg);
    CHECK(r.ok && r.rows_done == kSlot381RowCount);
    Row rows[kSlot381RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot381RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot381BoolCount + kSlot381IntCount + kSlot381LocalIntCount + kSlot381UIntCount + kSlot381CmdCount + kSlot381FloatCount + kSlot381UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot381CellCount && creg.atexit_log.size() == kSlot381RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot381RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot381Objects o;
        const Slot381Result r = init_slot381_6cd5b0(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_381");
    return 0;
}
