// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 498 (0x252ae0): 2 bool and 0 bounded float variables and 5 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_498_252ae0.hpp"
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

Row rows_of(Slot498Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'n', &o.r02, 0, -1, -1, -1, -1, nullptr};
    out[3] = Row{'i', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'t', &o.r04, -1, 0, 0, -1, -1, nullptr};
    out[5] = Row{'t', &o.r05, -1, 1, 0, -1, -1, nullptr};
    out[6] = Row{'t', &o.r06, -1, 2, 0, -1, -1, nullptr};
    out[7] = Row{'t', &o.r07, -1, 3, 0, -1, -1, nullptr};
    out[8] = Row{'K', &o.r08, -1, -1, 0, -1, -1, nullptr};
    out[9] = Row{'G', &o.r09, -1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot498Rva == 0x252ae0 && kSlot498Size == 2379 && kSlot498RowCount == 10);
    CHECK(is_int_functor_tag(kSlot498IntFnTag));
    CHECK(is_int_functor_tag(kSlot498UIntFnTag));
    CHECK(offsetof(Slot498Objects, r00) == 0x0);
    CHECK(offsetof(Slot498Objects, r01) == 0xc8);
    CHECK(offsetof(Slot498Objects, r02) == 0x190);
    CHECK(offsetof(Slot498Objects, r03) == 0x268);
    CHECK(offsetof(Slot498Objects, r04) == 0x340);
    CHECK(offsetof(Slot498Objects, r05) == 0x3e8);
    CHECK(offsetof(Slot498Objects, r06) == 0x490);
    CHECK(offsetof(Slot498Objects, r07) == 0x538);
    CHECK(offsetof(Slot498Objects, r08) == 0x5e0);
    CHECK(offsetof(Slot498Objects, r09) == 0x690);
    CHECK(sizeof(Slot498Objects) == 0x7c0);

    for (std::size_t n = 0; n < kSlot498BoolCount; ++n) CHECK(std::strlen(kSlot498BoolNames[n]) == kSlot498BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot498IntCount; ++n) CHECK(std::strlen(kSlot498IntNames[n]) == kSlot498IntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot498UIntCount; ++n) CHECK(std::strlen(kSlot498UIntNames[n]) == kSlot498UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot498StringCount; ++n) CHECK(std::strlen(kSlot498MenuStrings[n]) == kSlot498MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot498BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot498BoolFnTag, kSlot498BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot498BoolFnTag, kSlot498BoolValues[row.var], kSlot498MenuStrings[kSlot498BoolDesc[row.var]], kSlot498MenuStringLengths[kSlot498BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot498BoolNames[row.var], kSlot498BoolNameLengths[row.var]));
            break;
        }
        case 'i': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == kSlot498VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot498IntNames[row.var], kSlot498IntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot498IntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot498IntValue[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot498IntMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot498IntMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'n': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot498VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot498UIntNames[row.var], kSlot498UIntNameLengths[row.var]));
            CHECK(empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot498UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot498UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot498MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot498MenuStrings[row.s0]));
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
        case 'G': {
            const auto& b = *static_cast<const Slot498MemberBlock*>(row.obj);
            for (std::size_t k = 0; k < kSlot498MemberCount; ++k) CHECK(b.dw[3 * k] == kSlot498MemberLo[k] && b.dw[3 * k + 1] == kSlot498MemberHi[k] && b.dw[3 * k + 2] == kSlot498MemberSize[k]);   // all 72 dwords written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot498ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot498MenuStrings[row.s0], kSlot498MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot498MenuStrings[row.s1], kSlot498MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot498MenuStrings[row.var], kSlot498MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot498MenuStrings[row.s1], kSlot498MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot498MenuStrings[row.s1], kSlot498MenuStringLengths[row.s1]));
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
    Slot498Objects o;
    poison(o);
    o.separator_alloc = default_family_separator_alloc();
    const Slot498Result r = init_slot498_252ae0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot498RowCount);
    Row rows[kSlot498RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot498RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C') ++items;
    }
    CHECK(vars == kSlot498BoolCount + kSlot498IntCount + kSlot498LocalIntCount + kSlot498UIntCount + kSlot498CmdCount + kSlot498StrVarCount + kSlot498FloatCount + kSlot498UFloatCount);
    CHECK(menu.root.count == items && items == 5 && menu.root.link.next == &o.r04.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot498CellCount && creg.atexit_log.size() == kSlot498RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot498RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : 0x272d0;

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
        Slot498Objects o;
    o.separator_alloc = default_family_separator_alloc();
        const Slot498Result r = init_slot498_252ae0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot498Objects o;
    o.separator_alloc = default_family_separator_alloc();
        const Slot498Result r = init_slot498_252ae0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 4);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_498");
    return 0;
}
