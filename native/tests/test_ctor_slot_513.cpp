// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 513 (0x1d0770): 4 bool and 0 bounded float variables and 6 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cstdlib>

#include "core/ctor_slot_513_1d0770.hpp"

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

Row rows_of(Slot513Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'r', &o.r00, -1, -1, -1, -1, -1, nullptr};
    out[1] = Row{'n', &o.r01, 0, -1, -1, -1, -1, nullptr};
    out[2] = Row{'l', &o.r02, 0, -1, -1, -1, -1, nullptr};
    out[3] = Row{'i', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'b', &o.r04, 0, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 1, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 2, -1, -1, -1, -1, nullptr};
    out[7] = Row{'b', &o.r07, 3, -1, -1, -1, -1, nullptr};
    out[8] = Row{'e', &o.r08, -1, 2, 0, 1, -1, nullptr};
    out[9] = Row{'e', &o.r09, -1, 3, 0, 2, -1, nullptr};
    out[10] = Row{'e', &o.r10, -1, 4, 0, 3, -1, nullptr};
    out[11] = Row{'e', &o.r11, -1, 5, 0, 4, -1, nullptr};
    out[12] = Row{'e', &o.r12, -1, 1, 0, 5, -1, nullptr};
    out[13] = Row{'e', &o.r13, -1, 6, 0, 6, -1, nullptr};
    out[14] = Row{'w', &o.r14, -1, -1, -1, -1, -1, nullptr};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    dtor = find_local_int_var_class(kSlot513LocalIntSites[var].wrapper_rva)->dtor_rva;
    return dtor;
}

void section_constants() {
    CHECK(kSlot513Rva == 0x1d0770 && kSlot513Size == 4050 && kSlot513RowCount == 15);
    CHECK(is_int_functor_tag(kSlot513IntFnTag));
    for (const LocalIntSiteSpec& site : kSlot513LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
    CHECK(is_int_functor_tag(kSlot513UIntFnTag));
    CHECK(offsetof(Slot513Objects, r00) == 0x0);
    CHECK(offsetof(Slot513Objects, r01) == 0x2d8);
    CHECK(offsetof(Slot513Objects, r02) == 0x3b0);
    CHECK(offsetof(Slot513Objects, r03) == 0x488);
    CHECK(offsetof(Slot513Objects, r04) == 0x560);
    CHECK(offsetof(Slot513Objects, r05) == 0x628);
    CHECK(offsetof(Slot513Objects, r06) == 0x6f0);
    CHECK(offsetof(Slot513Objects, r07) == 0x7b8);
    CHECK(offsetof(Slot513Objects, r08) == 0x880);
    CHECK(offsetof(Slot513Objects, r09) == 0x928);
    CHECK(offsetof(Slot513Objects, r10) == 0x9d0);
    CHECK(offsetof(Slot513Objects, r11) == 0xa78);
    CHECK(offsetof(Slot513Objects, r12) == 0xb20);
    CHECK(offsetof(Slot513Objects, r13) == 0xbc8);
    CHECK(offsetof(Slot513Objects, r14) == 0xc70);
    CHECK(sizeof(Slot513Objects) == 0xca8);
    for (std::size_t n = 0; n < kSlot513BoolCount; ++n) CHECK(std::strlen(kSlot513BoolNames[n]) == kSlot513BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot513IntCount; ++n) CHECK(std::strlen(kSlot513IntNames[n]) == kSlot513IntNameLengths[n]);
    for (const LocalIntSiteSpec& site : kSlot513LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot513UIntCount; ++n) CHECK(std::strlen(kSlot513UIntNames[n]) == kSlot513UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot513StringCount; ++n) CHECK(std::strlen(kSlot513MenuStrings[n]) == kSlot513MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot513BoolFnTag, kSlot513BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot513BoolNames[row.var], kSlot513BoolNameLengths[row.var]));
            break;
        }
        case 'i': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == kSlot513VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot513IntNames[row.var], kSlot513IntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot513IntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot513IntValue[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(kSlot513IntMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(kSlot513IntMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot513LocalIntSites[row.var];
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
        case 'n': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot513VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot513UIntNames[row.var], kSlot513UIntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot513UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot513UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot513MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot513MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
            break;
        }
        case 'r': {
            const auto& rs = *static_cast<const Slot513Records*>(row.obj);
            for (std::size_t n = 0; n < kSlot513RecordCount; ++n) {
                const NamedRecord& rec = rs.rec[n];
                CHECK(same_view(rec.name, kSlot513RecName[n], kSlot513RecNameLength[n]) && rec.name.ctl == nullptr);   // borrowed view: no control block to count
                CHECK(rec.kind == kSlot513RecKind[n] && rec.q28 == 0 && rec.b30 == 0 && rec.flag == kSlot513RecFlag[n]);
                for (std::uint8_t b : rec.gap24) CHECK(b == 0xa5);   // never written
                for (std::uint8_t b : rec.gap32) CHECK(b == 0xa5);   // never written
            }
            break;
        }
        case 'w': {
            const auto& z = *static_cast<const Slot513Zero*>(row.obj);
            CHECK(z.q00 == 0xa5a5a5a5a5a5a5a5ull && z.q08 == 0 && z.q10 == 0 && z.q18 == 0);   // +0x00 is never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot513ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot513MenuStrings[row.s0], kSlot513MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot513MenuStrings[row.s1], kSlot513MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot513MenuStrings[row.s1], kSlot513MenuStringLengths[row.s1]));
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
    Slot513Objects o;
    poison(o);
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot513_record_alloc();
    const Slot513Result r = init_slot513_1d0770(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot513RowCount);
    Row rows[kSlot513RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot513RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w') ++items;
    }
    CHECK(vars == kSlot513BoolCount + kSlot513IntCount + kSlot513LocalIntCount + kSlot513UIntCount + kSlot513CmdCount + kSlot513StrVarCount + kSlot513FloatCount + kSlot513UFloatCount);
    CHECK(menu.root.count == items && items == 6 && menu.root.link.next == &o.r08.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r01.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot513CellCount && creg.atexit_log.size() == kSlot513RowCount - cells + 1 + kSlot513RecsCount);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot513RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? kSlot513RecsDtorRva : rows[i].kind == 'w' ? kSlot513ZeroDtorRva : 0x272d0;
        if (rows[i].kind == 'r') {
            // The registrar initialises the registry on the first record and records its destructor 0xa81b80 once, BEFORE the records' own 0x1c8990.
            const auto& rs = *static_cast<const Slot513Records*>(rows[i].obj);
            check_atexit(creg, at, kRecordRegistryDtorRva, &cell.registry);
            check_atexit(creg, at + 1, dtor, rows[i].obj);
            ++reg_shift;
            CHECK(cell.guard != 0);
            for (RecordPtrVec* vec : {&cell.registry.first, &cell.registry.second}) {
                CHECK(static_cast<std::size_t>(vec->end - vec->begin) == kSlot513RecordCount && vec->cap >= vec->end);
                for (std::size_t n = 0; n < kSlot513RecordCount; ++n) CHECK(vec->begin[n] == &rs.rec[n]);   // constructor order, in both vectors
            }
            std::free(cell.registry.first.begin);
            std::free(cell.registry.second.begin);
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
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot513Objects o;
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot513_record_alloc();
        const Slot513Result r = init_slot513_1d0770(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot513Objects o;
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot513_record_alloc();
        const Slot513Result r = init_slot513_1d0770(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
    }
    {
        // No registry cell: the records row is refused visibly; every row before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot513Objects o;
        poison(o);
        o.record_registry = nullptr;
        o.record_alloc = default_slot513_record_alloc();
        const Slot513Result r = init_slot513_1d0770(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
    }
    {
        // No allocator: the first record's vector growth is refused (the original would fault), nothing is recorded for the records.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot513Objects o;
        poison(o);
        RecordRegistryCell cell{};
        o.record_registry = &cell;
        o.record_alloc = AllocDispatchContract{};
        const Slot513Result r = init_slot513_1d0770(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(cell.registry.first.begin == nullptr);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_513");
    return 0;
}
