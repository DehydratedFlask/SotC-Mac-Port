// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 464 (0x3db070): 1 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cstdlib>

#include "core/ctor_slot_464_3db070.hpp"

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

Row rows_of(Slot464Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'r', &o.r01, -1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot464Rva == 0x3db070 && kSlot464Size == 697 && kSlot464RowCount == 2);
    CHECK(offsetof(Slot464Objects, r00) == 0x0);
    CHECK(offsetof(Slot464Objects, r01) == 0xc8);
    CHECK(sizeof(Slot464Objects) == 0x1f8);

    for (std::size_t n = 0; n < kSlot464BoolCount; ++n) CHECK(std::strlen(kSlot464BoolNames[n]) == kSlot464BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot464StringCount; ++n) CHECK(std::strlen(kSlot464MenuStrings[n]) == kSlot464MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot464BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot464BoolFnTag, kSlot464BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot464BoolFnTag, kSlot464BoolValues[row.var], kSlot464MenuStrings[kSlot464BoolDesc[row.var]], kSlot464MenuStringLengths[kSlot464BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot464BoolNames[row.var], kSlot464BoolNameLengths[row.var]));
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot464MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot464MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
            break;
        }
        case 'r': {
            const auto& rs = *static_cast<const Slot464Records*>(row.obj);
            for (std::size_t n = 0; n < kSlot464RecordCount; ++n) {
                const NamedRecord& rec = rs.rec[n];
                CHECK(same_view(rec.name, kSlot464RecName[n], kSlot464RecNameLength[n]) && rec.name.ctl == nullptr);   // borrowed view: no control block to count
                CHECK(rec.kind == kSlot464RecKind[n] && rec.q28 == 0 && rec.b30 == 0 && rec.flag == kSlot464RecFlag[n]);
                for (std::uint8_t b : rec.gap24) CHECK(b == 0xa5);   // never written
                for (std::uint8_t b : rec.gap32) CHECK(b == 0xa5);   // never written
            }
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot464ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot464MenuStrings[row.s0], kSlot464MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot464MenuStrings[row.s1], kSlot464MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot464MenuStrings[row.var], kSlot464MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot464MenuStrings[row.s1], kSlot464MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot464MenuStrings[row.s1], kSlot464MenuStringLengths[row.s1]));
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
    Slot464Objects o;
    poison(o);
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot464_record_alloc();
    const Slot464Result r = init_slot464_3db070(o, creg);
    CHECK(r.ok && r.rows_done == kSlot464RowCount);
    Row rows[kSlot464RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot464RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot464BoolCount + kSlot464IntCount + kSlot464LocalIntCount + kSlot464UIntCount + kSlot464CmdCount + kSlot464StrVarCount + kSlot464FloatCount + kSlot464UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot464CellCount && creg.atexit_log.size() == kSlot464RowCount - cells + kSlot464RecsCount);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot464RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? kSlot464RecsDtorRva : rows[i].kind == 'w' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : 0x272d0;
        if (rows[i].kind == 'r') {
            // The registrar initialises the registry on the first record and records its destructor 0xa81b80 once, BEFORE the records' own 0x1c8990.
            const auto& rs = *static_cast<const Slot464Records*>(rows[i].obj);
            check_atexit(creg, at, kRecordRegistryDtorRva, &cell.registry);
            check_atexit(creg, at + 1, dtor, rows[i].obj);
            ++reg_shift;
            CHECK(cell.guard != 0);
            for (RecordPtrVec* vec : {&cell.registry.first, &cell.registry.second}) {
                CHECK(static_cast<std::size_t>(vec->end - vec->begin) == kSlot464RecordCount && vec->cap >= vec->end);
                for (std::size_t n = 0; n < kSlot464RecordCount; ++n) CHECK(vec->begin[n] == &rs.rec[n]);   // constructor order, in both vectors
            }
            std::free(cell.registry.first.begin);
            std::free(cell.registry.second.begin);
            continue;
        }

        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot464Objects o;
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot464_record_alloc();
        const Slot464Result r = init_slot464_3db070(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
    {
        // No registry cell: the records row is refused visibly; every row before it was built.
        ConCmdRegistry creg;
        Slot464Objects o;
        poison(o);
        o.record_registry = nullptr;
        o.record_alloc = default_slot464_record_alloc();
        const Slot464Result r = init_slot464_3db070(o, creg);
        CHECK(!r.ok && r.rows_done == 1);
    }
    {
        // No allocator: the first record's vector growth is refused (the original would fault), nothing is recorded for the records.
        ConCmdRegistry creg;
        Slot464Objects o;
        poison(o);
        RecordRegistryCell cell{};
        o.record_registry = &cell;
        o.record_alloc = AllocDispatchContract{};
        const Slot464Result r = init_slot464_3db070(o, creg);
        CHECK(!r.ok && r.rows_done == 1);
        CHECK(cell.registry.first.begin == nullptr);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_464");
    return 0;
}
