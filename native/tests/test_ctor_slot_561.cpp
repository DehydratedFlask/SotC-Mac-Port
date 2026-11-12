// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 561 (0x3ac00): 2 bool and 0 bounded float variables and 5 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_561_3ac00.hpp"

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

Row rows_of(Slot561Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1};
    out[2] = Row{'i', &o.r02, 0, -1, -1, -1, -1};
    out[3] = Row{'n', &o.r03, 0, -1, -1, -1, -1};
    out[4] = Row{'g', &o.r04, -1, -1, 0, 1, -1};
    out[5] = Row{'e', &o.r05, -1, 0, 1, 2, -1};
    out[6] = Row{'e', &o.r06, -1, 1, 1, 3, -1};
    out[7] = Row{'e', &o.r07, -1, 2, 1, 4, -1};
    out[8] = Row{'e', &o.r08, -1, 3, 1, 5, -1};
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
    CHECK(kSlot561Rva == 0x3ac00 && kSlot561Size == 1808 && kSlot561RowCount == 9);
    CHECK(is_int_functor_tag(kSlot561IntFnTag));
    CHECK(is_int_functor_tag(kSlot561UIntFnTag));
    CHECK(offsetof(Slot561Objects, r00) == 0x0);
    CHECK(offsetof(Slot561Objects, r01) == 0xc8);
    CHECK(offsetof(Slot561Objects, r02) == 0x190);
    CHECK(offsetof(Slot561Objects, r03) == 0x268);
    CHECK(offsetof(Slot561Objects, r04) == 0x340);
    CHECK(offsetof(Slot561Objects, r05) == 0x3e8);
    CHECK(offsetof(Slot561Objects, r06) == 0x490);
    CHECK(offsetof(Slot561Objects, r07) == 0x538);
    CHECK(offsetof(Slot561Objects, r08) == 0x5e0);
    CHECK(sizeof(Slot561Objects) == 0x688);
    for (std::size_t n = 0; n < kSlot561BoolCount; ++n) CHECK(std::strlen(kSlot561BoolNames[n]) == kSlot561BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot561IntCount; ++n) CHECK(std::strlen(kSlot561IntNames[n]) == kSlot561IntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot561UIntCount; ++n) CHECK(std::strlen(kSlot561UIntNames[n]) == kSlot561UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot561StringCount; ++n) CHECK(std::strlen(kSlot561MenuStrings[n]) == kSlot561MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot561BoolFnTag, kSlot561BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot561BoolNames[row.var], kSlot561BoolNameLengths[row.var]));
            break;
        }
        case 'i': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == kSlot561VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot561IntNames[row.var], kSlot561IntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot561IntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot561IntValue[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot561IntMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot561IntMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'n': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot561VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot561UIntNames[row.var], kSlot561UIntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot561UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot561UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot561MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot561MenuStrings[row.s0]));
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
            CHECK(it.flags == kSlot561ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot561MenuStrings[row.s0], kSlot561MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot561MenuStrings[row.s1], kSlot561MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot561MenuStrings[row.s1], kSlot561MenuStringLengths[row.s1]));
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
    Slot561Objects o;
    poison(o);
    const Slot561Result r = init_slot561_3ac00(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot561RowCount);
    Row rows[kSlot561RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot561RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot561BoolCount + kSlot561IntCount + kSlot561LocalIntCount + kSlot561UIntCount + kSlot561CmdCount + kSlot561FloatCount + kSlot561UFloatCount);
    CHECK(menu.root.count == items && items == 5 && menu.root.link.next == &o.r04.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot561CellCount && creg.atexit_log.size() == kSlot561RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot561RowCount; ++i) {
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
        Slot561Objects o;
        const Slot561Result r = init_slot561_3ac00(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot561Objects o;
        const Slot561Result r = init_slot561_3ac00(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 4);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_561");
    return 0;
}
