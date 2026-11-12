// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 501 (0x24db80): 5 bool and 0 bounded float variables and 10 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_501_24db80.hpp"
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

Row rows_of(Slot501Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1, nullptr};
    out[3] = Row{'u', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'b', &o.r04, 3, -1, -1, -1, -1, nullptr};
    out[5] = Row{'z', &o.r05, 0, -1, -1, -1, -1, nullptr};
    out[6] = Row{'n', &o.r06, 0, -1, -1, -1, -1, nullptr};
    out[7] = Row{'b', &o.r07, 4, -1, -1, -1, -1, nullptr};
    out[8] = Row{'g', &o.r08, 32, -1, 0, 1, -1, nullptr};
    out[9] = Row{'J', &o.r09, 4, -1, 1, -1, -1, nullptr};
    out[10] = Row{'e', &o.r10, -1, 7, 1, 2, -1, nullptr};
    out[11] = Row{'e', &o.r11, -1, 1, 1, 3, -1, nullptr};
    out[12] = Row{'e', &o.r12, -1, 3, 1, 4, -1, nullptr};
    out[13] = Row{'e', &o.r13, -1, 4, 1, 5, -1, nullptr};
    out[14] = Row{'e', &o.r14, -1, 2, 1, 6, -1, nullptr};
    out[15] = Row{'e', &o.r15, -1, 0, 1, 7, -1, nullptr};
    out[16] = Row{'e', &o.r16, -1, 5, 1, 8, -1, nullptr};
    out[17] = Row{'e', &o.r17, -1, 6, 1, 9, -1, nullptr};
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
    CHECK(kSlot501Rva == 0x24db80 && kSlot501Size == 3244 && kSlot501RowCount == 18);
    CHECK(is_float_functor_tag(kSlot501UFloatFnTag));
    CHECK(is_int_functor_tag(kSlot501UIntFnTag));
    CHECK(sizeof(ConCmdStringVar) == 0x148);
    for (std::size_t n = 0; n < kSlot501StrVarCount; ++n) CHECK(std::strlen(kSlot501StrNames[n]) == kSlot501StrNameLengths[n]);
    CHECK(offsetof(Slot501Objects, r00) == 0x0);
    CHECK(offsetof(Slot501Objects, r01) == 0xc8);
    CHECK(offsetof(Slot501Objects, r02) == 0x190);
    CHECK(offsetof(Slot501Objects, r03) == 0x258);
    CHECK(offsetof(Slot501Objects, r04) == 0x330);
    CHECK(offsetof(Slot501Objects, r05) == 0x3f8);
    CHECK(offsetof(Slot501Objects, r06) == 0x540);
    CHECK(offsetof(Slot501Objects, r07) == 0x618);
    CHECK(offsetof(Slot501Objects, r08) == 0x6e0);
    CHECK(offsetof(Slot501Objects, r09) == 0x788);
    CHECK(offsetof(Slot501Objects, r10) == 0x830);
    CHECK(offsetof(Slot501Objects, r11) == 0x8d8);
    CHECK(offsetof(Slot501Objects, r12) == 0x980);
    CHECK(offsetof(Slot501Objects, r13) == 0xa28);
    CHECK(offsetof(Slot501Objects, r14) == 0xad0);
    CHECK(offsetof(Slot501Objects, r15) == 0xb78);
    CHECK(offsetof(Slot501Objects, r16) == 0xc20);
    CHECK(offsetof(Slot501Objects, r17) == 0xcc8);
    CHECK(sizeof(Slot501Objects) == 0xd80);

    for (std::size_t n = 0; n < kSlot501BoolCount; ++n) CHECK(std::strlen(kSlot501BoolNames[n]) == kSlot501BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot501UFloatCount; ++n) CHECK(std::strlen(kSlot501UFloatNames[n]) == kSlot501UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot501UIntCount; ++n) CHECK(std::strlen(kSlot501UIntNames[n]) == kSlot501UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot501StringCount; ++n) CHECK(std::strlen(kSlot501MenuStrings[n]) == kSlot501MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot501BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot501BoolFnTag, kSlot501BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot501BoolFnTag, kSlot501BoolValues[row.var], kSlot501MenuStrings[kSlot501BoolDesc[row.var]], kSlot501MenuStringLengths[kSlot501BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot501BoolNames[row.var], kSlot501BoolNameLengths[row.var]));
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot501VarFlags);
            CHECK(v.value == kSlot501UFloatValueBits[row.var] && v.default_value == kSlot501UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot501UFloatNames[row.var], kSlot501UFloatNameLengths[row.var]));
            if (kSlot501UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot501MenuStrings[kSlot501UFloatDesc[row.var]], kSlot501MenuStringLengths[kSlot501UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'n': case 'N': case 'P': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot501VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot501UIntNames[row.var], kSlot501UIntNameLengths[row.var]));
            CHECK(empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot501UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot501UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'z': {
            const auto& v = *static_cast<const ConCmdStringVar*>(row.obj);
            const std::int32_t d = kSlot501StrInit[row.var];
            CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == kStringVarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot501StrNames[row.var], kSlot501StrNameLengths[row.var]) && empty_view(v.base.description));
            if (d < 0) CHECK(v.default_value.data == empty_string_f730().data && v.default_value.size == 0 && empty_view(v.default_value) && empty_view(v.value) && v.value.data == empty_string_f730().data);
            else CHECK(same_view(v.default_value, kSlot501MenuStrings[d], kSlot501MenuStringLengths[d]) && same_view(v.value, kSlot501MenuStrings[d], kSlot501MenuStringLengths[d]));
            CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot501MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot501MenuStrings[row.s0]));
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
            CHECK(it.flags == ((row.kind == 'g' || row.kind == 'J') ? static_cast<std::uint32_t>(row.var) : kSlot501ItemFlags) && it.registered == 0);
            CHECK(same_view(it.label, kSlot501MenuStrings[row.s0], kSlot501MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot501MenuStrings[row.s1], kSlot501MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot501MenuStrings[row.var], kSlot501MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'J') {
                CHECK(it.kind == 0 && it.target == nullptr && it.flags == static_cast<std::uint32_t>(row.var));   // 0x26800 stores kind 3, then overwrites it with 0
                CHECK(it.text.size == 13 && it.text.data != nullptr && std::memcmp(it.text.data, "MenuSeparator", 13) == 0);   // built by 0xf1a0 (heap copy)
                CHECK(same_view(it.label, kSlot501MenuStrings[row.s0], kSlot501MenuStringLengths[row.s0]));
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
                CHECK(same_view(it.text, kSlot501MenuStrings[row.s1], kSlot501MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot501MenuStrings[row.s1], kSlot501MenuStringLengths[row.s1]));
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
    Slot501Objects o;
    poison(o);
    o.separator_alloc = default_family_separator_alloc();
    const Slot501Result r = init_slot501_24db80(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot501RowCount);
    Row rows[kSlot501RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot501RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'P' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G' || kd == 'H') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L' && kd != 'Z' && kd != 'H') ++items;
    }
    CHECK(vars == kSlot501BoolCount + kSlot501IntCount + kSlot501LocalIntCount + kSlot501UIntCount + kSlot501CmdCount + kSlot501StrVarCount + kSlot501FloatCount + kSlot501UFloatCount);
    CHECK(menu.root.count == items && items == 10 && menu.root.link.next == &o.r08.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot501CellCount && creg.atexit_log.size() == kSlot501RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot501RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G' || rows[i].kind == 'H') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : (rows[i].kind == 'N' || rows[i].kind == 'P') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'Z' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? 0 : rows[i].kind == 'L' ? 0 : 0x272d0;

        if (dtor == 0x272d0 && !root_seen) {
            check_atexit(creg, at, 0x27280, &menu.root);
            root_seen = true;
            check_atexit(creg, at + 1, dtor, rows[i].obj);
        } else {
            check_atexit(creg, at, dtor, rows[i].kind == 'Z' ? nullptr : rows[i].obj);
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
        Slot501Objects o;
    o.separator_alloc = default_family_separator_alloc();
        const Slot501Result r = init_slot501_24db80(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot501Objects o;
    o.separator_alloc = default_family_separator_alloc();
        const Slot501Result r = init_slot501_24db80(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_501");
    return 0;
}
