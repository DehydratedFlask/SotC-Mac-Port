// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 422 (0x56b980): 7 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_422_56b980.hpp"

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

Row rows_of(Slot422Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1, nullptr};
    out[3] = Row{'b', &o.r03, 3, -1, -1, -1, -1, nullptr};
    out[4] = Row{'b', &o.r04, 4, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 5, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 6, -1, -1, -1, -1, nullptr};
    out[7] = Row{'l', &o.r07, 0, -1, -1, -1, -1, nullptr};
    out[8] = Row{'l', &o.r08, 1, -1, -1, -1, -1, nullptr};
    out[9] = Row{'y', &o.r09, 0, -1, -1, -1, -1, nullptr};
    out[10] = Row{'l', &o.r10, 2, -1, -1, -1, -1, nullptr};
    out[11] = Row{'k', &o.r11, -1, -1, -1, -1, -1, nullptr};
    out[12] = Row{'p', &o.r12, -1, -1, -1, -1, -1, nullptr};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    dtor = find_local_int_var_class(kSlot422LocalIntSites[var].wrapper_rva)->dtor_rva;
    return dtor;
}

void section_constants() {
    CHECK(kSlot422Rva == 0x56b980 && kSlot422Size == 2682 && kSlot422RowCount == 13);
    for (const LocalIntSiteSpec& site : kSlot422LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
    CHECK(offsetof(Slot422Objects, r00) == 0x0);
    CHECK(offsetof(Slot422Objects, r01) == 0xc8);
    CHECK(offsetof(Slot422Objects, r02) == 0x190);
    CHECK(offsetof(Slot422Objects, r03) == 0x258);
    CHECK(offsetof(Slot422Objects, r04) == 0x320);
    CHECK(offsetof(Slot422Objects, r05) == 0x3e8);
    CHECK(offsetof(Slot422Objects, r06) == 0x4b0);
    CHECK(offsetof(Slot422Objects, r07) == 0x578);
    CHECK(offsetof(Slot422Objects, r08) == 0x650);
    CHECK(offsetof(Slot422Objects, r09) == 0x728);
    CHECK(offsetof(Slot422Objects, r10) == 0x7f0);
    CHECK(offsetof(Slot422Objects, r11) == 0x8e0);
    CHECK(offsetof(Slot422Objects, r12) == 0x918);
    CHECK(sizeof(Slot422Objects) == 0x920);

    for (std::size_t n = 0; n < kSlot422BoolCount; ++n) CHECK(std::strlen(kSlot422BoolNames[n]) == kSlot422BoolNameLengths[n]);
    for (const BoolVar274730SiteSpec& site : kSlot422BoolXSites) CHECK(std::strlen(site.name) == site.name_length && site.q[3] != 0 && is_bool_functor_tag(site.arg_fn_tag));
    for (const LocalIntSiteSpec& site : kSlot422LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot422StringCount; ++n) CHECK(std::strlen(kSlot422MenuStrings[n]) == kSlot422MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot422BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot422BoolFnTag, kSlot422BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot422BoolFnTag, kSlot422BoolValues[row.var], kSlot422MenuStrings[kSlot422BoolDesc[row.var]], kSlot422MenuStringLengths[kSlot422BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot422BoolNames[row.var], kSlot422BoolNameLengths[row.var]));
            break;
        }
        case 'y': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            const BoolVar274730SiteSpec& site = kSlot422BoolXSites[row.var];
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
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot422LocalIntSites[row.var];
            const LocalIntVarClass* cls = find_local_int_var_class(site.wrapper_rva);
            CHECK(cls != nullptr && (site.arg_fn_tag == cls->fn_vtbl || is_local_int_caller_twin_tag(site.arg_fn_tag, cls->fn_vtbl)));
            CHECK(v.base.vptr == cls->final_vptr && v.base.flags == site.flags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, site.name, site.name_length) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(site.value) && dword(v, 0x68) == static_cast<std::uint32_t>(site.value));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(site.min_value) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(site.max_value) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == cls->fn_vtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot422MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot422MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
            break;
        }
        case 'p': {
            CHECK(*static_cast<const std::uint64_t*>(row.obj) == 0);   // the one store of the original (poison would read 0xa5..)
            break;
        }
        case 'k': {
            const auto& n = *static_cast<const NodeRef56b4b0*>(row.obj);
            const HashedStr& empty = static_empty_hashed_2ed3640();
            CHECK(n.node != nullptr && n.flag == 0 && n.id.hash == empty.hash && n.id.str.ctl == empty.str.ctl);   // 0x14c3a0 copy of the static empty string
            const auto* bytes = reinterpret_cast<const std::uint8_t*>(&n);
            for (int b = 4; b < 8; ++b) CHECK(bytes[b] == 0xa5);                  // HashedStr padding is never written
            for (std::size_t b = 0x31; b < 0x38; ++b) CHECK(bytes[b] == 0xa5);    // past the flag byte: never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot422ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot422MenuStrings[row.s0], kSlot422MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot422MenuStrings[row.s1], kSlot422MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot422MenuStrings[row.var], kSlot422MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot422MenuStrings[row.s1], kSlot422MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot422MenuStrings[row.s1], kSlot422MenuStringLengths[row.s1]));
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
    Slot422Objects o;
    poison(o);
    const Slot422Result r = init_slot422_56b980(o, creg);
    CHECK(r.ok && r.rows_done == kSlot422RowCount);
    Row rows[kSlot422RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot422RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'p' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot422BoolCount + kSlot422BoolXCount + kSlot422IntCount + kSlot422LocalIntCount + kSlot422UIntCount + kSlot422CmdCount + kSlot422StrVarCount + kSlot422FloatCount + kSlot422UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot422CellCount && creg.atexit_log.size() == kSlot422RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot422RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'p' ? kSlot422ZeroPtrDtorRva : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? kSlot422NodeRefDtorRva : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot422Objects o;
        const Slot422Result r = init_slot422_56b980(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_422");
    return 0;
}
