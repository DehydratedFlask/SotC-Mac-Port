// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 451 (0x49d230): 8 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_451_49d230.hpp"
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

Row rows_of(Slot451Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'n', &o.r02, 0, -1, -1, -1, -1, nullptr};
    out[3] = Row{'n', &o.r03, 1, -1, -1, -1, -1, nullptr};
    out[4] = Row{'n', &o.r04, 2, -1, -1, -1, -1, nullptr};
    out[5] = Row{'n', &o.r05, 3, -1, -1, -1, -1, nullptr};
    out[6] = Row{'u', &o.r06, 0, -1, -1, -1, -1, nullptr};
    out[7] = Row{'u', &o.r07, 1, -1, -1, -1, -1, nullptr};
    out[8] = Row{'n', &o.r08, 4, -1, -1, -1, -1, nullptr};
    out[9] = Row{'d', &o.r09, 0, -1, -1, -1, -1, nullptr};
    out[10] = Row{'u', &o.r10, 2, -1, -1, -1, -1, nullptr};
    out[11] = Row{'b', &o.r11, 2, -1, -1, -1, -1, nullptr};
    out[12] = Row{'b', &o.r12, 3, -1, -1, -1, -1, nullptr};
    out[13] = Row{'d', &o.r13, 1, -1, -1, -1, -1, nullptr};
    out[14] = Row{'b', &o.r14, 4, -1, -1, -1, -1, nullptr};
    out[15] = Row{'b', &o.r15, 5, -1, -1, -1, -1, nullptr};
    out[16] = Row{'b', &o.r16, 6, -1, -1, -1, -1, nullptr};
    out[17] = Row{'b', &o.r17, 7, -1, -1, -1, -1, nullptr};
    out[18] = Row{'u', &o.r18, 3, -1, -1, -1, -1, nullptr};
    out[19] = Row{'C', &o.r19, 0, -1, -1, -1, -1, nullptr};
    out[20] = Row{'C', &o.r20, 1, -1, -1, -1, -1, nullptr};
    out[21] = Row{'p', &o.r21, -1, -1, -1, -1, -1, nullptr};
    out[22] = Row{'m', &o.r22, 0, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot451Rva == 0x49d230 && kSlot451Size == 5126 && kSlot451RowCount == 23);
    CHECK(is_float_functor_tag(kSlot451UFloatFnTag));
    CHECK(is_int_functor_tag(kSlot451UIntFnTag));
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    for (std::size_t n = 0; n < kSlot451DwrCount; ++n) CHECK(std::strlen(kSlot451DwrNames[n]) == kSlot451DwrNameLengths[n] && kSlot451DwrMin[n] <= kSlot451DwrMax[n]);
    CHECK(kSlot451CritStoreFirst[kSlot451CritCount] == kSlot451CritStoreCount);
    CHECK(offsetof(Slot451Objects, r00) == 0x0);
    CHECK(offsetof(Slot451Objects, r01) == 0xc8);
    CHECK(offsetof(Slot451Objects, r02) == 0x190);
    CHECK(offsetof(Slot451Objects, r03) == 0x268);
    CHECK(offsetof(Slot451Objects, r04) == 0x340);
    CHECK(offsetof(Slot451Objects, r05) == 0x418);
    CHECK(offsetof(Slot451Objects, r06) == 0x4f0);
    CHECK(offsetof(Slot451Objects, r07) == 0x5c8);
    CHECK(offsetof(Slot451Objects, r08) == 0x6a0);
    CHECK(offsetof(Slot451Objects, r09) == 0x778);
    CHECK(offsetof(Slot451Objects, r10) == 0x850);
    CHECK(offsetof(Slot451Objects, r11) == 0x928);
    CHECK(offsetof(Slot451Objects, r12) == 0x9f0);
    CHECK(offsetof(Slot451Objects, r13) == 0xab8);
    CHECK(offsetof(Slot451Objects, r14) == 0xb90);
    CHECK(offsetof(Slot451Objects, r15) == 0xc58);
    CHECK(offsetof(Slot451Objects, r16) == 0xd20);
    CHECK(offsetof(Slot451Objects, r17) == 0xde8);
    CHECK(offsetof(Slot451Objects, r18) == 0xeb0);
    CHECK(offsetof(Slot451Objects, r19) == 0xf88);
    CHECK(offsetof(Slot451Objects, r20) == 0x1060);
    CHECK(offsetof(Slot451Objects, r21) == 0x1090);
    CHECK(offsetof(Slot451Objects, r22) == 0x1098);
    CHECK(sizeof(Slot451Objects) == 0x1138);

    for (std::size_t n = 0; n < kSlot451BoolCount; ++n) CHECK(std::strlen(kSlot451BoolNames[n]) == kSlot451BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot451UFloatCount; ++n) CHECK(std::strlen(kSlot451UFloatNames[n]) == kSlot451UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot451UIntCount; ++n) CHECK(std::strlen(kSlot451UIntNames[n]) == kSlot451UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot451CmdCount; ++n) CHECK(std::strlen(kSlot451CmdNames[n]) == kSlot451CmdNameLengths[n] && kSlot451CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot451StringCount; ++n) CHECK(std::strlen(kSlot451MenuStrings[n]) == kSlot451MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot451BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot451BoolFnTag, kSlot451BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot451BoolFnTag, kSlot451BoolValues[row.var], kSlot451MenuStrings[kSlot451BoolDesc[row.var]], kSlot451MenuStringLengths[kSlot451BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot451BoolNames[row.var], kSlot451BoolNameLengths[row.var]));
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot451VarFlags);
            CHECK(v.value == kSlot451UFloatValueBits[row.var] && v.default_value == kSlot451UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot451UFloatNames[row.var], kSlot451UFloatNameLengths[row.var]));
            if (kSlot451UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot451MenuStrings[kSlot451UFloatDesc[row.var]], kSlot451MenuStringLengths[kSlot451UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'n': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot451VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot451UIntNames[row.var], kSlot451UIntNameLengths[row.var]));
            if (kSlot451UIntDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot451MenuStrings[kSlot451UIntDesc[row.var]], kSlot451MenuStringLengths[kSlot451UIntDesc[row.var]]));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot451UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot451UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot451CmdNames[row.var], kSlot451MenuStrings[kSlot451CmdDesc[row.var]], kSlot451MenuStringLengths[kSlot451CmdDesc[row.var]], kSlot451CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot451MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot451MenuStrings[row.s0]));
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
        case 'd': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const RangedDwordVariant variant = kSlot451DwrVariant[row.var] == 0 ? RangedDwordVariant::a_49ca60 : RangedDwordVariant::b_49cc60;
            CHECK(kSlot451DwrFnTag[row.var] == ranged_dword_class(variant) && kSlot451DwrFinalVptr[row.var] != ranged_dword_vptr(variant));   // the site overwrites the constructor's mid vptr with its own literal final vptr
            CHECK(v.base.vptr == kSlot451DwrFinalVptr[row.var] && v.base.flags == kSlot451VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot451DwrNames[row.var], kSlot451DwrNameLengths[row.var]));
            CHECK(same_view(v.base.description, kSlot451MenuStrings[kSlot451DwrDesc[row.var]], kSlot451MenuStringLengths[kSlot451DwrDesc[row.var]]));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot451DwrValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot451DwrDefault[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot451DwrMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot451DwrMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kSlot451DwrFnTag[row.var] && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'C': {
            // Only the handle (+0, filled by 0x3b310) and the listed stores were written; every other byte still carries the poison.
            const auto* bytes = static_cast<const std::uint8_t*>(row.obj);
            const std::size_t size = kSlot451CritSize[row.var];
            std::uint8_t want[0x100];
            CHECK(size <= sizeof want);
            std::memset(want, 0xa5, sizeof want);
            for (std::size_t q = kSlot451CritStoreFirst[row.var]; q < kSlot451CritStoreFirst[row.var + 1]; ++q) {
                const std::uint64_t value = kSlot451CritStoreValue[q];
                if (kSlot451CritStoreWidth[q] <= 8) std::memcpy(want + kSlot451CritStoreOffset[q], &value, kSlot451CritStoreWidth[q]);
                else std::memset(want + kSlot451CritStoreOffset[q], 0, kSlot451CritStoreWidth[q]);
            }
            for (std::size_t b = 8; b < size; ++b) CHECK(bytes[b] == want[b]);
            std::uint64_t handle = 0;
            std::memcpy(&handle, bytes, 8);
            CHECK(handle != 0xa5a5a5a5a5a5a5a5ull && handle != 0);   // 0x3b310 wrote a live token
            CHECK(critsec_destroy_at(const_cast<std::uint8_t*>(bytes)) == 0);   // fixture cleanup (0x3b380 model)
            break;
        }
        case 'p': {
            CHECK(*static_cast<const std::uint64_t*>(row.obj) == 0);   // the one store of the original (poison would read 0xa5..)
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot451ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot451MenuStrings[row.s0], kSlot451MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot451MenuStrings[row.s1], kSlot451MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot451MenuStrings[row.var], kSlot451MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot451MenuStrings[row.s1], kSlot451MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot451MenuStrings[row.s1], kSlot451MenuStringLengths[row.s1]));
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
    Slot451Objects o;
    poison(o);
    const Slot451Result r = init_slot451_49d230(o, creg);
    CHECK(r.ok && r.rows_done == kSlot451RowCount);
    Row rows[kSlot451RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot451RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C') ++items;
    }
    CHECK(vars == kSlot451BoolCount + kSlot451DwrCount + kSlot451IntCount + kSlot451LocalIntCount + kSlot451UIntCount + kSlot451CmdCount + kSlot451StrVarCount + kSlot451FloatCount + kSlot451UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot451CellCount && creg.atexit_log.size() == kSlot451RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot451RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? kSlot451ZeroPtrDtorRva : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? kSlot451DwrDtorRva[rows[i].var] : rows[i].kind == 'C' ? kSlot451CritDtorRva[rows[i].var] : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot451Objects o;
        const Slot451Result r = init_slot451_49d230(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_451");
    return 0;
}
