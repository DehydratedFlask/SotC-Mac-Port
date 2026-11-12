// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 399 (0x632ca0): 6 bool and 4 bounded float variables and 10 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_399_632ca0.hpp"

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

Row rows_of(Slot399Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1};
    out[3] = Row{'f', &o.r03, 0, -1, -1, -1, -1};
    out[4] = Row{'f', &o.r04, 1, -1, -1, -1, -1};
    out[5] = Row{'b', &o.r05, 3, -1, -1, -1, -1};
    out[6] = Row{'f', &o.r06, 2, -1, -1, -1, -1};
    out[7] = Row{'f', &o.r07, 3, -1, -1, -1, -1};
    out[8] = Row{'b', &o.r08, 4, -1, -1, -1, -1};
    out[9] = Row{'l', &o.r09, 0, -1, -1, -1, -1};
    out[10] = Row{'l', &o.r10, 1, -1, -1, -1, -1};
    out[11] = Row{'e', &o.r11, -1, 9, 0, 1, -1};
    out[12] = Row{'e', &o.r12, -1, 10, 0, 2, -1};
    out[13] = Row{'e', &o.r13, -1, 1, 3, 4, -1};
    out[14] = Row{'e', &o.r14, -1, 2, 3, 5, -1};
    out[15] = Row{'S', &o.r15, -1, 3, 3, 6, 0};
    out[16] = Row{'S', &o.r16, -1, 6, 7, 8, 1};
    out[17] = Row{'e', &o.r17, -1, 5, 7, 9, -1};
    out[18] = Row{'S', &o.r18, -1, 4, 7, 10, 2};
    out[19] = Row{'S', &o.r19, -1, 7, 7, 11, 3};
    out[20] = Row{'e', &o.r20, -1, 8, 12, 13, -1};
    out[21] = Row{'b', &o.r21, 5, -1, -1, -1, -1};
    out[22] = Row{'c', &o.r22, -1, -1, 14, -1, -1};
    out[23] = Row{'c', &o.r23, -1, -1, 15, -1, -1};
    out[24] = Row{'c', &o.r24, -1, -1, 16, -1, -1};
    out[25] = Row{'c', &o.r25, -1, -1, 17, -1, -1};
    out[26] = Row{'c', &o.r26, -1, -1, 18, -1, -1};
    out[27] = Row{'c', &o.r27, -1, -1, 19, -1, -1};
    out[28] = Row{'c', &o.r28, -1, -1, 20, -1, -1};
    out[29] = Row{'c', &o.r29, -1, -1, 21, -1, -1};
    out[30] = Row{'c', &o.r30, -1, -1, 22, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    dtor = find_local_int_var_class(kSlot399LocalIntSites[var].wrapper_rva)->dtor_rva;
    return dtor;
}

void section_constants() {
    CHECK(kSlot399Rva == 0x632ca0 && kSlot399Size == 4642 && kSlot399RowCount == 31);
    CHECK(is_known_float_arg_functor(kSlot399FloatFnTag));
    for (const LocalIntSiteSpec& site : kSlot399LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
    CHECK(offsetof(Slot399Objects, r00) == 0x0);
    CHECK(offsetof(Slot399Objects, r01) == 0xc8);
    CHECK(offsetof(Slot399Objects, r02) == 0x190);
    CHECK(offsetof(Slot399Objects, r03) == 0x258);
    CHECK(offsetof(Slot399Objects, r04) == 0x330);
    CHECK(offsetof(Slot399Objects, r05) == 0x408);
    CHECK(offsetof(Slot399Objects, r06) == 0x4d0);
    CHECK(offsetof(Slot399Objects, r07) == 0x5a8);
    CHECK(offsetof(Slot399Objects, r08) == 0x680);
    CHECK(offsetof(Slot399Objects, r09) == 0x748);
    CHECK(offsetof(Slot399Objects, r10) == 0x820);
    CHECK(offsetof(Slot399Objects, r11) == 0x8f8);
    CHECK(offsetof(Slot399Objects, r12) == 0x9a0);
    CHECK(offsetof(Slot399Objects, r13) == 0xa48);
    CHECK(offsetof(Slot399Objects, r14) == 0xaf0);
    CHECK(offsetof(Slot399Objects, r15) == 0xb98);
    CHECK(offsetof(Slot399Objects, r16) == 0xc40);
    CHECK(offsetof(Slot399Objects, r17) == 0xce8);
    CHECK(offsetof(Slot399Objects, r18) == 0xd90);
    CHECK(offsetof(Slot399Objects, r19) == 0xe38);
    CHECK(offsetof(Slot399Objects, r20) == 0xee0);
    CHECK(offsetof(Slot399Objects, r21) == 0xf90);
    CHECK(offsetof(Slot399Objects, r22) == 0x1058);
    CHECK(offsetof(Slot399Objects, r23) == 0x1068);
    CHECK(offsetof(Slot399Objects, r24) == 0x1078);
    CHECK(offsetof(Slot399Objects, r25) == 0x1090);
    CHECK(offsetof(Slot399Objects, r26) == 0x10a0);
    CHECK(offsetof(Slot399Objects, r27) == 0x10c0);
    CHECK(offsetof(Slot399Objects, r28) == 0x10e0);
    CHECK(offsetof(Slot399Objects, r29) == 0x10f0);
    CHECK(offsetof(Slot399Objects, r30) == 0x1100);
    CHECK(sizeof(Slot399Objects) == 0x1110);
    for (std::size_t n = 0; n < kSlot399BoolCount; ++n) CHECK(std::strlen(kSlot399BoolNames[n]) == kSlot399BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot399FloatCount; ++n) CHECK(std::strlen(kSlot399FloatNames[n]) == kSlot399FloatNameLengths[n]);
    for (const LocalIntSiteSpec& site : kSlot399LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot399StringCount; ++n) CHECK(std::strlen(kSlot399MenuStrings[n]) == kSlot399MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot399BoolFnTag, kSlot399BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot399BoolNames[row.var], kSlot399BoolNameLengths[row.var]));
            break;
        }
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot399LocalIntSites[row.var];
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
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot399VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot399FloatNames[row.var], kSlot399FloatNameLengths[row.var]));
            CHECK(v.value == kSlot399FloatValueBits[row.var] && v.default_value == kSlot399FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot399FloatLoBits[row.var] && hi == kSlot399FloatHiBits[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot399MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot399MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot399ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot399MenuStrings[row.s0], kSlot399MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot399MenuStrings[row.s1], kSlot399MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot399MenuStrings[row.s1], kSlot399MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's' || row.kind == 'S') {
                CHECK(it.range_min == bits_to_float(kSlot399SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot399SliderHiBits[row.slider]));
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
    Slot399Objects o;
    poison(o);
    const Slot399Result r = init_slot399_632ca0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot399RowCount);
    Row rows[kSlot399RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot399RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot399BoolCount + kSlot399IntCount + kSlot399LocalIntCount + kSlot399UIntCount + kSlot399CmdCount + kSlot399FloatCount + kSlot399UFloatCount);
    CHECK(menu.root.count == items && items == 10 && menu.root.link.next == &o.r11.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot399CellCount && creg.atexit_log.size() == kSlot399RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot399RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
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
        Slot399Objects o;
        const Slot399Result r = init_slot399_632ca0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot399Objects o;
        const Slot399Result r = init_slot399_632ca0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 11);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_399");
    return 0;
}
