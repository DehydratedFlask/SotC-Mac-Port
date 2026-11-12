// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 427 (0x5388f0): 7 bool and 1 bounded float variables and 7 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cstdlib>

#include "core/ctor_slot_427_5388f0.hpp"
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

Row rows_of(Slot427Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'c', &o.r00, -1, -1, 0, -1, -1, nullptr};
    out[1] = Row{'c', &o.r01, -1, -1, 1, -1, -1, nullptr};
    out[2] = Row{'h', &o.r02, 0, -1, 2, -1, -1, nullptr};
    out[3] = Row{'b', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'g', &o.r04, -1, -1, 3, 4, -1, nullptr};
    out[5] = Row{'e', &o.r05, -1, 3, 4, 5, -1, nullptr};
    out[6] = Row{'b', &o.r06, 1, -1, -1, -1, -1, nullptr};
    out[7] = Row{'e', &o.r07, -1, 6, 6, 7, -1, nullptr};
    out[8] = Row{'E', &o.r08, 0, -1, -1, -1, -1, nullptr};
    out[9] = Row{'E', &o.r09, 1, -1, -1, -1, -1, nullptr};
    out[10] = Row{'z', &o.r10, 0, -1, 8, -1, -1, nullptr};
    out[11] = Row{'b', &o.r11, 2, -1, -1, -1, -1, nullptr};
    out[12] = Row{'L', &o.r12, -1, -1, -1, -1, -1, nullptr};
    out[13] = Row{'y', &o.r13, 0, -1, -1, -1, -1, nullptr};
    out[14] = Row{'n', &o.r14, 0, -1, -1, -1, -1, nullptr};
    out[15] = Row{'y', &o.r15, 1, -1, -1, -1, -1, nullptr};
    out[16] = Row{'b', &o.r16, 3, -1, -1, -1, -1, nullptr};
    out[17] = Row{'f', &o.r17, 0, -1, -1, -1, -1, nullptr};
    out[18] = Row{'b', &o.r18, 4, -1, -1, -1, -1, nullptr};
    out[19] = Row{'b', &o.r19, 5, -1, -1, -1, -1, nullptr};
    out[20] = Row{'t', &o.r20, -1, 17, 9, -1, -1, nullptr};
    out[21] = Row{'t', &o.r21, -1, 18, 9, -1, -1, nullptr};
    out[22] = Row{'t', &o.r22, -1, 19, 9, -1, -1, nullptr};
    out[23] = Row{'b', &o.r23, 6, -1, -1, -1, -1, nullptr};
    out[24] = Row{'e', &o.r24, -1, 23, 10, 11, -1, nullptr};
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
    CHECK(kSlot427Rva == 0x5388f0 && kSlot427Size == 4362 && kSlot427RowCount == 25);
    CHECK(is_known_float_arg_functor(kSlot427FloatFnTag));
    CHECK(is_int_functor_tag(kSlot427UIntFnTag));
    CHECK(sizeof(ConCmdStringVar) == 0x148);
    for (std::size_t n = 0; n < kSlot427StrVarCount; ++n) CHECK(std::strlen(kSlot427StrNames[n]) == kSlot427StrNameLengths[n]);
    CHECK(offsetof(Slot427Objects, r00) == 0x0);
    CHECK(offsetof(Slot427Objects, r01) == 0x10);
    CHECK(offsetof(Slot427Objects, r02) == 0x20);
    CHECK(offsetof(Slot427Objects, r03) == 0x50);
    CHECK(offsetof(Slot427Objects, r04) == 0x118);
    CHECK(offsetof(Slot427Objects, r05) == 0x1c0);
    CHECK(offsetof(Slot427Objects, r06) == 0x268);
    CHECK(offsetof(Slot427Objects, r07) == 0x330);
    CHECK(offsetof(Slot427Objects, r08) == 0x3d8);
    CHECK(offsetof(Slot427Objects, r09) == 0x410);
    CHECK(offsetof(Slot427Objects, r10) == 0x448);
    CHECK(offsetof(Slot427Objects, r11) == 0x590);
    CHECK(offsetof(Slot427Objects, r12) == 0x658);
    CHECK(offsetof(Slot427Objects, r13) == 0x680);
    CHECK(offsetof(Slot427Objects, r14) == 0x748);
    CHECK(offsetof(Slot427Objects, r15) == 0x850);
    CHECK(offsetof(Slot427Objects, r16) == 0x918);
    CHECK(offsetof(Slot427Objects, r17) == 0x9e0);
    CHECK(offsetof(Slot427Objects, r18) == 0xab8);
    CHECK(offsetof(Slot427Objects, r19) == 0xb80);
    CHECK(offsetof(Slot427Objects, r20) == 0xc48);
    CHECK(offsetof(Slot427Objects, r21) == 0xcf0);
    CHECK(offsetof(Slot427Objects, r22) == 0xd98);
    CHECK(offsetof(Slot427Objects, r23) == 0xe40);
    CHECK(offsetof(Slot427Objects, r24) == 0xf08);
    CHECK(sizeof(Slot427Objects) == 0xfc8);

    for (std::size_t n = 0; n < kSlot427BoolCount; ++n) CHECK(std::strlen(kSlot427BoolNames[n]) == kSlot427BoolNameLengths[n]);
    for (const BoolVar274730SiteSpec& site : kSlot427BoolXSites) CHECK(std::strlen(site.name) == site.name_length && site.q[3] != 0 && is_bool_functor_tag(site.arg_fn_tag));
    for (std::size_t n = 0; n < kSlot427FloatCount; ++n) CHECK(std::strlen(kSlot427FloatNames[n]) == kSlot427FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot427UIntCount; ++n) CHECK(std::strlen(kSlot427UIntNames[n]) == kSlot427UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot427StringCount; ++n) CHECK(std::strlen(kSlot427MenuStrings[n]) == kSlot427MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot427BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot427BoolFnTag, kSlot427BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot427BoolFnTag, kSlot427BoolValues[row.var], kSlot427MenuStrings[kSlot427BoolDesc[row.var]], kSlot427MenuStringLengths[kSlot427BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot427BoolNames[row.var], kSlot427BoolNameLengths[row.var]));
            break;
        }
        case 'y': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            const BoolVar274730SiteSpec& site = kSlot427BoolXSites[row.var];
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
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot427VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot427FloatNames[row.var], kSlot427FloatNameLengths[row.var]));
            CHECK(v.value == kSlot427FloatValueBits[row.var] && v.default_value == kSlot427FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot427FloatLoBits[row.var] && hi == kSlot427FloatHiBits[row.var]);
            break;
        }
        case 'n': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot427VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot427UIntNames[row.var], kSlot427UIntNameLengths[row.var]));
            CHECK(empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot427UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot427UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'z': {
            const auto& v = *static_cast<const ConCmdStringVar*>(row.obj);
            const std::int32_t d = kSlot427StrInit[row.var];
            CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == kStringVarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot427StrNames[row.var], kSlot427StrNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(same_view(v.default_value, kSlot427MenuStrings[d], kSlot427MenuStringLengths[d]) && same_view(v.value, kSlot427MenuStrings[d], kSlot427MenuStringLengths[d]));
            CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot427MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot427MenuStrings[row.s0]));
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
        case 'E': {
            const auto& rec = *static_cast<const NamedRecord*>(row.obj);
            CHECK(same_view(rec.name, kSlot427Rec1Name[row.var], kSlot427Rec1NameLength[row.var]) && rec.name.ctl == nullptr);   // borrowed view: no control block to count
            CHECK(rec.kind == kSlot427Rec1Kind[row.var] && rec.q28 == 0 && rec.b30 == 0 && rec.flag == kSlot427Rec1Flag[row.var]);
            for (std::uint8_t b : rec.gap24) CHECK(b == 0xa5);   // never written
            for (std::uint8_t b : rec.gap32) CHECK(b == 0xa5);   // never written
            break;
        }
        case 'L': {
            const auto& z = *static_cast<const Slot427ZeroCell*>(row.obj);
            CHECK(z.q00 == 0 && z.q08 == 0 && z.q10 == 0);   // all three qwords zeroed
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == kSlot427HolderVptr && h.tail == 0 && same_view(h.name, kSlot427MenuStrings[row.s0], kSlot427MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot427ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot427MenuStrings[row.s0], kSlot427MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot427MenuStrings[row.s1], kSlot427MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot427MenuStrings[row.var], kSlot427MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot427MenuStrings[row.s1], kSlot427MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot427MenuStrings[row.s1], kSlot427MenuStringLengths[row.s1]));
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
    Slot427Objects o;
    poison(o);
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot427_record_alloc();
    const Slot427Result r = init_slot427_5388f0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot427RowCount);
    Row rows[kSlot427RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot427RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L') ++items;
    }
    CHECK(vars == kSlot427BoolCount + kSlot427BoolXCount + kSlot427IntCount + kSlot427LocalIntCount + kSlot427UIntCount + kSlot427CmdCount + kSlot427StrVarCount + kSlot427FloatCount + kSlot427UFloatCount);
    CHECK(menu.root.count == items && items == 7 && menu.root.link.next == &o.r04.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r03.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot427CellCount && creg.atexit_log.size() == kSlot427RowCount - cells + 1 + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    bool rec_registry_seen = false;
    const NamedRecord* rec1_ptr[kSlot427Rec1Count] = {};
    std::size_t rec1_seen = 0;
    for (std::size_t i = 0; i < kSlot427RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? kSlot427HolderDtorRva : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? kSlot427Rec1DtorRva : rows[i].kind == 'L' ? kSlot427ZeroCellDtorRva : 0x272d0;
        if (rows[i].kind == 'E') {
            // The registrar initialises the registry on the first record and records its destructor 0xa81b80 once, BEFORE the record's own 0xa81950.
            std::size_t own = at;
            if (!rec_registry_seen) {
                check_atexit(creg, at, kRecordRegistryDtorRva, &cell.registry);
                rec_registry_seen = true;
                ++reg_shift;
                own = at + 1;
            }
            check_atexit(creg, own, dtor, rows[i].obj);
            CHECK(rec1_seen < kSlot427Rec1Count);
            rec1_ptr[rec1_seen++] = static_cast<const NamedRecord*>(rows[i].obj);
            continue;
        }

        if (dtor == 0x272d0 && !root_seen) {
            check_atexit(creg, at, 0x27280, &menu.root);
            root_seen = true;
            check_atexit(creg, at + 1, dtor, rows[i].obj);
        } else {
            check_atexit(creg, at, dtor, rows[i].obj);
        }
    }
    CHECK(rec1_seen == kSlot427Rec1Count && cell.guard != 0);
    for (RecordPtrVec* vec : {&cell.registry.first, &cell.registry.second}) {
        CHECK(static_cast<std::size_t>(vec->end - vec->begin) == kSlot427Rec1Count && vec->cap >= vec->end);
        for (std::size_t q = 0; q < kSlot427Rec1Count; ++q) CHECK(vec->begin[q] == rec1_ptr[q]);   // constructor order, in both vectors
    }
    std::free(cell.registry.first.begin);
    std::free(cell.registry.second.begin);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot427Objects o;
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot427_record_alloc();
        const Slot427Result r = init_slot427_5388f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 3);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot427Objects o;
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot427_record_alloc();
        const Slot427Result r = init_slot427_5388f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 4);
    }
    {
        // No registry cell: the records row is refused visibly; every row before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot427Objects o;
        poison(o);
        o.record_registry = nullptr;
        o.record_alloc = default_slot427_record_alloc();
        const Slot427Result r = init_slot427_5388f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
    }
    {
        // No allocator: the first record's vector growth is refused (the original would fault), nothing is recorded for the records.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot427Objects o;
        poison(o);
        RecordRegistryCell cell{};
        o.record_registry = &cell;
        o.record_alloc = AllocDispatchContract{};
        const Slot427Result r = init_slot427_5388f0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
        CHECK(cell.registry.first.begin == nullptr);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_427");
    return 0;
}
