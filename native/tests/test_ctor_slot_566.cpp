// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 566 (0x25de0): 2 bool and 0 bounded float variables and 1 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_566_25de0.hpp"
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

Row rows_of(Slot566Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'z', &o.r01, 0, -1, 0, -1, -1, nullptr};
    out[2] = Row{'z', &o.r02, 1, -1, 1, -1, -1, nullptr};
    out[3] = Row{'b', &o.r03, 1, -1, -1, -1, -1, nullptr};
    out[4] = Row{'P', &o.r04, 0, -1, -1, -1, -1, &o.cell04};
    out[5] = Row{'e', &o.r05, -1, 4, 2, 3, -1, nullptr};
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
    CHECK(kSlot566Rva == 0x25de0 && kSlot566Size == 1194 && kSlot566RowCount == 6);
    CHECK(true);
    CHECK(sizeof(ConCmdStringVar) == 0x148);
    for (std::size_t n = 0; n < kSlot566StrVarCount; ++n) CHECK(std::strlen(kSlot566StrNames[n]) == kSlot566StrNameLengths[n]);
    CHECK(offsetof(Slot566Objects, r00) == 0x0);
    CHECK(offsetof(Slot566Objects, r01) == 0xd0);
    CHECK(offsetof(Slot566Objects, r02) == 0x218);
    CHECK(offsetof(Slot566Objects, r03) == 0x360);
    CHECK(offsetof(Slot566Objects, cell04) == 0x428 && offsetof(Slot566Objects, head04) == 0x430);
    CHECK(offsetof(Slot566Objects, r04) == 0x5a8);   // relocated 0xd8 B model: right after the window
    CHECK(offsetof(Slot566Objects, r05) == 0x500);
    CHECK(sizeof(Slot566Objects) == 0x680);

    for (std::size_t n = 0; n < kSlot566BoolCount; ++n) CHECK(std::strlen(kSlot566BoolNames[n]) == kSlot566BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot566UIntCount; ++n) CHECK(std::strlen(kSlot566UIntNames[n]) == kSlot566UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot566StringCount; ++n) CHECK(std::strlen(kSlot566MenuStrings[n]) == kSlot566MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot566BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot566BoolFnTag, kSlot566BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot566BoolFnTag, kSlot566BoolValues[row.var], kSlot566MenuStrings[kSlot566BoolDesc[row.var]], kSlot566MenuStringLengths[kSlot566BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot566BoolNames[row.var], kSlot566BoolNameLengths[row.var]));
            break;
        }
        case 'n': case 'N': case 'P': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kSlot566UIntTwinFinalVptr && v.base.flags == kSlot566VarFlags && v.base.registered == 1 && v.value_ref == row.ext);
            CHECK(same_view(v.base.name, kSlot566UIntNames[row.var], kSlot566UIntNameLengths[row.var]));
            CHECK(empty_view(v.base.description));
            CHECK(*static_cast<const std::uint32_t*>(row.ext) == static_cast<std::uint32_t>(kSlot566UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot566UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kUnboundedIntTwinBSeedClass && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'z': {
            const auto& v = *static_cast<const ConCmdStringVar*>(row.obj);
            const std::int32_t d = kSlot566StrInit[row.var];
            CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == kStringVarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot566StrNames[row.var], kSlot566StrNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(same_view(v.default_value, kSlot566MenuStrings[d], kSlot566MenuStringLengths[d]) && same_view(v.value, kSlot566MenuStrings[d], kSlot566MenuStringLengths[d]));
            CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot566MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot566MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot566ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot566MenuStrings[row.s0], kSlot566MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot566MenuStrings[row.s1], kSlot566MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot566MenuStrings[row.var], kSlot566MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot566MenuStrings[row.s1], kSlot566MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot566MenuStrings[row.s1], kSlot566MenuStringLengths[row.s1]));
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
    Slot566Objects o;
    poison(o); o.cell04 = 0x0;
    const Slot566Result r = init_slot566_25de0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot566RowCount);
    Row rows[kSlot566RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot566RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'P' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L') ++items;
    }
    CHECK(vars == kSlot566BoolCount + kSlot566IntCount + kSlot566LocalIntCount + kSlot566UIntCount + kSlot566CmdCount + kSlot566StrVarCount + kSlot566FloatCount + kSlot566UFloatCount);
    CHECK(menu.root.count == items && items == 1 && menu.root.link.next == &o.r05.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot566CellCount && creg.atexit_log.size() == kSlot566RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot566RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : (rows[i].kind == 'N' || rows[i].kind == 'P') ? kSlot566UIntTwinDtorRva : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? 0 : rows[i].kind == 'L' ? 0 : 0x272d0;

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
        Slot566Objects o;
        const Slot566Result r = init_slot566_25de0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot566Objects o;
        const Slot566Result r = init_slot566_25de0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 5);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_566");
    return 0;
}
