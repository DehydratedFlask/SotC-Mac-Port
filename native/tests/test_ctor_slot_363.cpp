// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 363 (0x751c70): 0 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_363_751c70.hpp"

namespace {
using namespace l4;

struct Row {
    char kind;            // b bool, f float, t 0x26db0 toggle, e 0x26ea0 toggle, s 0x27120 slider, g group
    const void* obj;
    int var;              // index into the bool / float tables (variables)
    int target;           // row index of the target variable (items)
    int s0;               // menu string: parent / label
    int s1;               // menu string: explicit text / caption
    int slider;           // slider table index
};

Row rows_of(Slot363Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'l', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'l', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'l', &o.r02, 2, -1, -1, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot363Rva == 0x751c70 && kSlot363Size == 701 && kSlot363RowCount == 3);
    for (const LocalIntSiteSpec& site : kSlot363LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
    CHECK(offsetof(Slot363Objects, r00) == 0x0);
    CHECK(offsetof(Slot363Objects, r01) == 0xd8);
    CHECK(offsetof(Slot363Objects, r02) == 0x1b0);
    CHECK(sizeof(Slot363Objects) == 0x288);
    for (const LocalIntSiteSpec& site : kSlot363LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot363StringCount; ++n) CHECK(std::strlen(kSlot363MenuStrings[n]) == kSlot363MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot363LocalIntSites[row.var];
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
            CHECK(c.name == kSlot363MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot363MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot363ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot363MenuStrings[row.s0], kSlot363MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot363MenuStrings[row.s1], kSlot363MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = is_float ? static_cast<const ConCmdRangedVar*>(tg.obj)->base : static_cast<const ConCmdBoolVar*>(tg.obj)->base;
            if (row.kind == 'e') {
                CHECK(same_view(it.text, kSlot363MenuStrings[row.s1], kSlot363MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's') {
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
    Slot363Objects o;
    poison(o);
    const Slot363Result r = init_slot363_751c70(o, creg);
    CHECK(r.ok && r.rows_done == kSlot363RowCount);
    Row rows[kSlot363RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot363RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot363BoolCount + kSlot363IntCount + kSlot363LocalIntCount + kSlot363FloatCount + kSlot363UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot363CellCount && creg.atexit_log.size() == kSlot363RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot363RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : rows[i].kind == 'i' ? 0x18a60 : rows[i].kind == 'l' ? find_local_int_var_class(kSlot363LocalIntSites[rows[i].var].wrapper_rva)->dtor_rva : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot363Objects o;
        const Slot363Result r = init_slot363_751c70(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_363");
    return 0;
}
