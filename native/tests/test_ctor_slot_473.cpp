// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 473 (0x355a00): 5 bool and 0 bounded float variables and 22 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_473_355a00.hpp"
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

Row rows_of(Slot473Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'h', &o.r00, 0, -1, 0, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 0, -1, -1, -1, -1, nullptr};
    out[2] = Row{'b', &o.r02, 1, -1, -1, -1, -1, nullptr};
    out[3] = Row{'y', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'y', &o.r04, 1, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 2, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 3, -1, -1, -1, -1, nullptr};
    out[7] = Row{'y', &o.r07, 2, -1, -1, -1, -1, nullptr};
    out[8] = Row{'y', &o.r08, 3, -1, -1, -1, -1, nullptr};
    out[9] = Row{'y', &o.r09, 4, -1, -1, -1, -1, nullptr};
    out[10] = Row{'y', &o.r10, 5, -1, -1, -1, -1, nullptr};
    out[11] = Row{'y', &o.r11, 6, -1, -1, -1, -1, nullptr};
    out[12] = Row{'y', &o.r12, 7, -1, -1, -1, -1, nullptr};
    out[13] = Row{'y', &o.r13, 8, -1, -1, -1, -1, nullptr};
    out[14] = Row{'b', &o.r14, 4, -1, -1, -1, -1, nullptr};
    out[15] = Row{'u', &o.r15, 0, -1, -1, -1, -1, nullptr};
    out[16] = Row{'u', &o.r16, 1, -1, -1, -1, -1, nullptr};
    out[17] = Row{'g', &o.r17, -1, -1, 1, 2, -1, nullptr};
    out[18] = Row{'e', &o.r18, -1, 1, 2, 3, -1, nullptr};
    out[19] = Row{'e', &o.r19, -1, 2, 2, 4, -1, nullptr};
    out[20] = Row{'e', &o.r20, -1, 14, 2, 5, -1, nullptr};
    out[21] = Row{'K', &o.r21, -1, -1, 2, -1, -1, nullptr};
    out[22] = Row{'e', &o.r22, -1, 5, 2, 6, -1, nullptr};
    out[23] = Row{'e', &o.r23, -1, 6, 2, 7, -1, nullptr};
    out[24] = Row{'e', &o.r24, -1, 7, 2, 8, -1, nullptr};
    out[25] = Row{'e', &o.r25, -1, 8, 2, 9, -1, nullptr};
    out[26] = Row{'K', &o.r26, -1, -1, 2, -1, -1, nullptr};
    out[27] = Row{'e', &o.r27, -1, 9, 2, 10, -1, nullptr};
    out[28] = Row{'e', &o.r28, -1, 10, 2, 11, -1, nullptr};
    out[29] = Row{'e', &o.r29, -1, 12, 2, 12, -1, nullptr};
    out[30] = Row{'e', &o.r30, -1, 11, 2, 13, -1, nullptr};
    out[31] = Row{'e', &o.r31, -1, 13, 2, 14, -1, nullptr};
    out[32] = Row{'K', &o.r32, -1, -1, 2, -1, -1, nullptr};
    out[33] = Row{'e', &o.r33, -1, 3, 2, 15, -1, nullptr};
    out[34] = Row{'e', &o.r34, -1, 4, 2, 16, -1, nullptr};
    out[35] = Row{'K', &o.r35, -1, -1, 2, -1, -1, nullptr};
    out[36] = Row{'e', &o.r36, -1, 15, 2, 17, -1, nullptr};
    out[37] = Row{'e', &o.r37, -1, 16, 2, 18, -1, nullptr};
    out[38] = Row{'K', &o.r38, -1, -1, 2, -1, -1, nullptr};
    out[39] = Row{'R', &o.r39, -1, -1, -1, -1, -1, nullptr};
    out[40] = Row{'z', &o.r40, 0, -1, 19, -1, -1, nullptr};
    out[41] = Row{'m', &o.r41, 0, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot473Rva == 0x355a00 && kSlot473Size == 6487 && kSlot473RowCount == 42);
    CHECK(is_float_functor_tag(kSlot473UFloatFnTag));
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(sizeof(ConCmdStringVar) == 0x148);
    for (std::size_t n = 0; n < kSlot473StrVarCount; ++n) CHECK(std::strlen(kSlot473StrNames[n]) == kSlot473StrNameLengths[n]);
    CHECK(offsetof(Slot473Objects, r00) == 0x0);
    CHECK(offsetof(Slot473Objects, r01) == 0x30);
    CHECK(offsetof(Slot473Objects, r02) == 0xf8);
    CHECK(offsetof(Slot473Objects, r03) == 0x1c0);
    CHECK(offsetof(Slot473Objects, r04) == 0x288);
    CHECK(offsetof(Slot473Objects, r05) == 0x350);
    CHECK(offsetof(Slot473Objects, r06) == 0x418);
    CHECK(offsetof(Slot473Objects, r07) == 0x4e0);
    CHECK(offsetof(Slot473Objects, r08) == 0x5a8);
    CHECK(offsetof(Slot473Objects, r09) == 0x670);
    CHECK(offsetof(Slot473Objects, r10) == 0x738);
    CHECK(offsetof(Slot473Objects, r11) == 0x800);
    CHECK(offsetof(Slot473Objects, r12) == 0x8c8);
    CHECK(offsetof(Slot473Objects, r13) == 0x990);
    CHECK(offsetof(Slot473Objects, r14) == 0xa58);
    CHECK(offsetof(Slot473Objects, r15) == 0xb20);
    CHECK(offsetof(Slot473Objects, r16) == 0xbf8);
    CHECK(offsetof(Slot473Objects, r17) == 0xcd0);
    CHECK(offsetof(Slot473Objects, r18) == 0xd78);
    CHECK(offsetof(Slot473Objects, r19) == 0xe20);
    CHECK(offsetof(Slot473Objects, r20) == 0xec8);
    CHECK(offsetof(Slot473Objects, r21) == 0xf70);
    CHECK(offsetof(Slot473Objects, r22) == 0x1018);
    CHECK(offsetof(Slot473Objects, r23) == 0x10c0);
    CHECK(offsetof(Slot473Objects, r24) == 0x1168);
    CHECK(offsetof(Slot473Objects, r25) == 0x1210);
    CHECK(offsetof(Slot473Objects, r26) == 0x12b8);
    CHECK(offsetof(Slot473Objects, r27) == 0x1360);
    CHECK(offsetof(Slot473Objects, r28) == 0x1408);
    CHECK(offsetof(Slot473Objects, r29) == 0x14b0);
    CHECK(offsetof(Slot473Objects, r30) == 0x1558);
    CHECK(offsetof(Slot473Objects, r31) == 0x1600);
    CHECK(offsetof(Slot473Objects, r32) == 0x16a8);
    CHECK(offsetof(Slot473Objects, r33) == 0x1750);
    CHECK(offsetof(Slot473Objects, r34) == 0x17f8);
    CHECK(offsetof(Slot473Objects, r35) == 0x18a0);
    CHECK(offsetof(Slot473Objects, r36) == 0x1948);
    CHECK(offsetof(Slot473Objects, r37) == 0x19f0);
    CHECK(offsetof(Slot473Objects, r38) == 0x1a98);
    CHECK(offsetof(Slot473Objects, r39) == 0x1b98);
    CHECK(offsetof(Slot473Objects, r40) == 0x41c0);
    CHECK(offsetof(Slot473Objects, r41) == 0x4308);
    CHECK(sizeof(Slot473Objects) == 0x43b8);

    for (std::size_t n = 0; n < kSlot473BoolCount; ++n) CHECK(std::strlen(kSlot473BoolNames[n]) == kSlot473BoolNameLengths[n]);
    for (const BoolVar274730SiteSpec& site : kSlot473BoolXSites) CHECK(std::strlen(site.name) == site.name_length && site.q[3] != 0 && is_bool_functor_tag(site.arg_fn_tag));
    for (std::size_t n = 0; n < kSlot473UFloatCount; ++n) CHECK(std::strlen(kSlot473UFloatNames[n]) == kSlot473UFloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot473CmdCount; ++n) CHECK(std::strlen(kSlot473CmdNames[n]) == kSlot473CmdNameLengths[n] && kSlot473CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot473StringCount; ++n) CHECK(std::strlen(kSlot473MenuStrings[n]) == kSlot473MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot473BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot473BoolFnTag, kSlot473BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot473BoolFnTag, kSlot473BoolValues[row.var], kSlot473MenuStrings[kSlot473BoolDesc[row.var]], kSlot473MenuStringLengths[kSlot473BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot473BoolNames[row.var], kSlot473BoolNameLengths[row.var]));
            break;
        }
        case 'y': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            const BoolVar274730SiteSpec& site = kSlot473BoolXSites[row.var];
            CHECK(is_bool_functor_tag(site.arg_fn_tag));
            CHECK(v.base.vptr == kBoolVarFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == 1);
            for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
            std::uint64_t w[4] = {};
            std::memcpy(w, v.zero70, sizeof w);
            CHECK(w[0] == reinterpret_cast<std::uint64_t>(v.zero70) && w[1] == site.q[1] && w[2] == site.q[2] && w[3] == site.q[3]);   // q[0] = self reference (q[3] != 0)
            CHECK(v.fn_vptr == kBoolVarCtorFinalFnClass && v.fn_payload == site.fn_payload && v.fn_aux == site.fn_aux);   // the net functor install: the family constant, not the site tag
            CHECK(v.value == site.initial && v.default_value == site.initial);
            CHECK(same_view(v.base.name, site.name, site.name_length) && empty_view(v.base.description));
            check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 1}, {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot473VarFlags);
            CHECK(v.value == kSlot473UFloatValueBits[row.var] && v.default_value == kSlot473UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot473UFloatNames[row.var], kSlot473UFloatNameLengths[row.var]));
            if (kSlot473UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot473MenuStrings[kSlot473UFloatDesc[row.var]], kSlot473MenuStringLengths[kSlot473UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot473CmdNames[row.var], kSlot473MenuStrings[kSlot473CmdDesc[row.var]], kSlot473MenuStringLengths[kSlot473CmdDesc[row.var]], kSlot473CmdHandler[row.var]);
            break;
        }
        case 'z': {
            const auto& v = *static_cast<const ConCmdStringVar*>(row.obj);
            const std::int32_t d = kSlot473StrInit[row.var];
            CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == kStringVarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot473StrNames[row.var], kSlot473StrNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(same_view(v.default_value, kSlot473MenuStrings[d], kSlot473MenuStringLengths[d]) && same_view(v.value, kSlot473MenuStrings[d], kSlot473MenuStringLengths[d]));
            CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot473MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot473MenuStrings[row.s0]));
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
        case 'R': {
            const auto& q = *static_cast<const Slot473Ring*>(row.obj);
            const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&q);
            CHECK(q.q00 == base + 0x28 && q.q08 == base + 0x28 && q.q20 == base + 0x28 && q.q10 == base + 0x2628);
            CHECK(q.q18 == 0xa5a5a5a5a5a5a5a5ull);   // never written
            for (std::uint8_t b : q.area) CHECK(b == 0xa5);   // never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == kSlot473HolderVptr && h.tail == 0 && same_view(h.name, kSlot473MenuStrings[row.s0], kSlot473MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot473ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot473MenuStrings[row.s0], kSlot473MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot473MenuStrings[row.s1], kSlot473MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot473MenuStrings[row.var], kSlot473MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot473MenuStrings[row.s1], kSlot473MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot473MenuStrings[row.s1], kSlot473MenuStringLengths[row.s1]));
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
    Slot473Objects o;
    poison(o);
    o.separator_alloc = default_family_separator_alloc();
    const Slot473Result r = init_slot473_355a00(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot473RowCount);
    Row rows[kSlot473RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot473RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C') ++items;
    }
    CHECK(vars == kSlot473BoolCount + kSlot473BoolXCount + kSlot473IntCount + kSlot473LocalIntCount + kSlot473UIntCount + kSlot473CmdCount + kSlot473StrVarCount + kSlot473FloatCount + kSlot473UFloatCount);
    CHECK(menu.root.count == items && items == 22 && menu.root.link.next == &o.r17.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r01.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot473CellCount && creg.atexit_log.size() == kSlot473RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot473RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? kSlot473HolderDtorRva : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? kSlot473RingDtorRva : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : 0x272d0;

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
        Slot473Objects o;
    o.separator_alloc = default_family_separator_alloc();
        const Slot473Result r = init_slot473_355a00(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot473Objects o;
    o.separator_alloc = default_family_separator_alloc();
        const Slot473Result r = init_slot473_355a00(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 17);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_473");
    return 0;
}
