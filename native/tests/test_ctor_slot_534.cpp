// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 534 (0x1418d0): 6 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_534_1418d0.hpp"
#include "core/helper_f8f630_strncpy.hpp"

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

Row rows_of(Slot534Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'N', &o.r02, 0, -1, -1, -1, -1, nullptr};
    out[3] = Row{'N', &o.r03, 1, -1, -1, -1, -1, nullptr};
    out[4] = Row{'n', &o.r04, 2, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 2, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 3, -1, -1, -1, -1, nullptr};
    out[7] = Row{'u', &o.r07, 0, -1, -1, -1, -1, nullptr};
    out[8] = Row{'u', &o.r08, 1, -1, -1, -1, -1, nullptr};
    out[9] = Row{'n', &o.r09, 3, -1, -1, -1, -1, nullptr};
    out[10] = Row{'N', &o.r10, 4, -1, -1, -1, -1, nullptr};
    out[11] = Row{'b', &o.r11, 4, -1, -1, -1, -1, nullptr};
    out[12] = Row{'b', &o.r12, 5, -1, -1, -1, -1, nullptr};
    out[13] = Row{'i', &o.r13, 0, -1, -1, -1, -1, nullptr};
    out[14] = Row{'n', &o.r14, 5, -1, -1, -1, -1, nullptr};
    out[15] = Row{'n', &o.r15, 6, -1, -1, -1, -1, nullptr};
    out[16] = Row{'n', &o.r16, 7, -1, -1, -1, -1, nullptr};
    out[17] = Row{'n', &o.r17, 8, -1, -1, -1, -1, nullptr};
    out[18] = Row{'n', &o.r18, 9, -1, -1, -1, -1, nullptr};
    out[19] = Row{'n', &o.r19, 10, -1, -1, -1, -1, nullptr};
    out[20] = Row{'j', &o.r20, 0, -1, -1, -1, -1, nullptr};
    out[21] = Row{'L', &o.r21, -1, -1, -1, -1, -1, nullptr};
    out[22] = Row{'m', &o.r22, 0, -1, -1, -1, -1, nullptr};
    out[23] = Row{'m', &o.r23, 1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot534Rva == 0x1418d0 && kSlot534Size == 6207 && kSlot534RowCount == 24);
    CHECK(is_int_functor_tag(kSlot534IntFnTag));
    CHECK(is_float_functor_tag(kSlot534UFloatFnTag));
    CHECK(is_int_functor_tag(kSlot534UIntFnTag));
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot534Objects, r00) == 0x0);
    CHECK(offsetof(Slot534Objects, r01) == 0xc8);
    CHECK(offsetof(Slot534Objects, r02) == 0x190);
    CHECK(offsetof(Slot534Objects, r03) == 0x268);
    CHECK(offsetof(Slot534Objects, r04) == 0x340);
    CHECK(offsetof(Slot534Objects, r05) == 0x418);
    CHECK(offsetof(Slot534Objects, r06) == 0x4e0);
    CHECK(offsetof(Slot534Objects, r07) == 0x5a8);
    CHECK(offsetof(Slot534Objects, r08) == 0x680);
    CHECK(offsetof(Slot534Objects, r09) == 0x758);
    CHECK(offsetof(Slot534Objects, r10) == 0x830);
    CHECK(offsetof(Slot534Objects, r11) == 0x908);
    CHECK(offsetof(Slot534Objects, r12) == 0x9d0);
    CHECK(offsetof(Slot534Objects, r13) == 0xa98);
    CHECK(offsetof(Slot534Objects, r14) == 0xb70);
    CHECK(offsetof(Slot534Objects, r15) == 0xc48);
    CHECK(offsetof(Slot534Objects, r16) == 0xd20);
    CHECK(offsetof(Slot534Objects, r17) == 0xdf8);
    CHECK(offsetof(Slot534Objects, r18) == 0xed0);
    CHECK(offsetof(Slot534Objects, r19) == 0xfa8);
    CHECK(offsetof(Slot534Objects, r20) == 0x1080);
    CHECK(offsetof(Slot534Objects, r21) == 0x1158);
    CHECK(offsetof(Slot534Objects, r22) == 0x1178);
    CHECK(offsetof(Slot534Objects, r23) == 0x1218);
    CHECK(sizeof(Slot534Objects) == 0x12b8);

    for (std::size_t n = 0; n < kSlot534BoolCount; ++n) CHECK(std::strlen(kSlot534BoolNames[n]) == kSlot534BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot534UFloatCount; ++n) CHECK(std::strlen(kSlot534UFloatNames[n]) == kSlot534UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot534IntCount; ++n) CHECK(std::strlen(kSlot534IntNames[n]) == kSlot534IntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot534UIntCount; ++n) CHECK(std::strlen(kSlot534UIntNames[n]) == kSlot534UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot534CmdCount; ++n) CHECK(std::strlen(kSlot534CmdNames[n]) == kSlot534CmdNameLengths[n] && kSlot534CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot534StringCount; ++n) CHECK(std::strlen(kSlot534MenuStrings[n]) == kSlot534MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot534BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot534BoolFnTag, kSlot534BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot534BoolFnTag, kSlot534BoolValues[row.var], kSlot534MenuStrings[kSlot534BoolDesc[row.var]], kSlot534MenuStringLengths[kSlot534BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot534BoolNames[row.var], kSlot534BoolNameLengths[row.var]));
            break;
        }
        case 'i': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == kSlot534VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot534IntNames[row.var], kSlot534IntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot534IntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot534IntValue[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot534IntMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot534IntMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot534VarFlags);
            CHECK(v.value == kSlot534UFloatValueBits[row.var] && v.default_value == kSlot534UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot534UFloatNames[row.var], kSlot534UFloatNameLengths[row.var]));
            if (kSlot534UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot534MenuStrings[kSlot534UFloatDesc[row.var]], kSlot534MenuStringLengths[kSlot534UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'n': case 'N': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == (row.kind == 'N' ? kSlot534UIntTwinFinalVptr : kUnboundedIntFinalVptr) && v.base.flags == kSlot534VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot534UIntNames[row.var], kSlot534UIntNameLengths[row.var]));
            CHECK(empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot534UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot534UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == (row.kind == 'N' ? kUnboundedIntTwinASeedClass : kIntFunctorClassTag) && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot534CmdNames[row.var], kSlot534MenuStrings[kSlot534CmdDesc[row.var]], kSlot534MenuStringLengths[kSlot534CmdDesc[row.var]], kSlot534CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot534MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot534MenuStrings[row.s0]));
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
        case 'L': {
            const auto& z = *static_cast<const Slot534ZeroCell*>(row.obj);
            CHECK(z.q00 == 0);   // every qword zeroed
            break;
        }
        case 'j': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kSlot534EnumFinalVptr && v.base.flags == kSlot534VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot534EnumNames[row.var], kSlot534EnumNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot534EnumValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot534EnumValue[row.var]));
            if (kSlot534EnumBounded) {
                CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot534EnumMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot534EnumMax[row.var]) && v.max_engaged == 1);
            } else {
                CHECK(v.min_value[0] == 0 && v.min_engaged == 0 && v.max_value[0] == 0 && v.max_engaged == 0);   // one byte stored per bound, engaged bytes cleared
                for (std::size_t q = 1; q < 4; ++q) CHECK(v.min_value[q] == 0xa5 && v.max_value[q] == 0xa5);   // never written
            }
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kSlot534EnumFnTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot534ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot534MenuStrings[row.s0], kSlot534MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot534MenuStrings[row.s1], kSlot534MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot534MenuStrings[row.var], kSlot534MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'K') {
                CHECK(it.kind == kMenuKindSeparator && it.target == nullptr);
                CHECK(it.text.size == 13 && it.text.data != nullptr && std::memcmp(it.text.data, "MenuSeparator", 13) == 0);   // built by 0xf1a0 (heap copy)
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot534MenuStrings[row.s1], kSlot534MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot534MenuStrings[row.s1], kSlot534MenuStringLengths[row.s1]));
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
    Slot534Objects o;
    poison(o);
    const Slot534Result r = init_slot534_1418d0(o, creg);
    CHECK(r.ok && r.rows_done == kSlot534RowCount);
    Row rows[kSlot534RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot534RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L') ++items;
    }
    CHECK(vars == kSlot534BoolCount + kSlot534IntCount + kSlot534LocalIntCount + kSlot534UIntCount + kSlot534CmdCount + kSlot534StrVarCount + kSlot534FloatCount + kSlot534UFloatCount + kSlot534EnumCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot534CellCount && creg.atexit_log.size() == kSlot534RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot534RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'N' ? kSlot534UIntTwinDtorRva : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? kSlot534EnumDtorRva : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? 0 : rows[i].kind == 'L' ? kSlot534ZeroCellDtorRva : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot534Objects o;
        const Slot534Result r = init_slot534_1418d0(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_534");
    return 0;
}
