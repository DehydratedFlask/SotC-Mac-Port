// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 453 (0x48ac10): 0 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cstdlib>

#include "core/ctor_slot_453_48ac10.hpp"
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

Row rows_of(Slot453Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'r', &o.r00, -1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot453Rva == 0x48ac10 && kSlot453Size == 856 && kSlot453RowCount == 1);
    CHECK(offsetof(Slot453Objects, r00) == 0x0);
    CHECK(sizeof(Slot453Objects) == 0x280);

    for (std::size_t n = 0; n < kSlot453StringCount; ++n) CHECK(std::strlen(kSlot453MenuStrings[n]) == kSlot453MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot453MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot453MenuStrings[row.s0]));
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
        case 'r': {
            const auto& rs = *static_cast<const Slot453Records*>(row.obj);
            for (std::size_t n = 0; n < kSlot453RecordCount; ++n) {
                const NamedRecord& rec = rs.rec[n];
                CHECK(same_view(rec.name, kSlot453RecName[n], kSlot453RecNameLength[n]) && rec.name.ctl == nullptr);   // borrowed view: no control block to count
                CHECK(rec.kind == kSlot453RecKind[n] && rec.q28 == 0 && rec.b30 == 0 && rec.flag == kSlot453RecFlag[n]);
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
            CHECK(it.flags == kSlot453ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot453MenuStrings[row.s0], kSlot453MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot453MenuStrings[row.s1], kSlot453MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot453MenuStrings[row.var], kSlot453MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot453MenuStrings[row.s1], kSlot453MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot453MenuStrings[row.s1], kSlot453MenuStringLengths[row.s1]));
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
    Slot453Objects o;
    poison(o);
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot453_record_alloc();
    const Slot453Result r = init_slot453_48ac10(o, creg);
    CHECK(r.ok && r.rows_done == kSlot453RowCount);
    Row rows[kSlot453RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot453RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C') ++items;
    }
    CHECK(vars == kSlot453BoolCount + kSlot453IntCount + kSlot453LocalIntCount + kSlot453UIntCount + kSlot453CmdCount + kSlot453StrVarCount + kSlot453FloatCount + kSlot453UFloatCount);
    CHECK(items == 0);
    CHECK(vars == 0 && creg.pending.size() == 0);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot453CellCount && creg.atexit_log.size() == kSlot453RowCount - cells + kSlot453RecsCount);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot453RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? kSlot453RecsDtorRva : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : 0x272d0;
        if (rows[i].kind == 'r') {
            // The registrar initialises the registry on the first record and records its destructor 0xa81b80 once, BEFORE the records' own 0x1c8990.
            const auto& rs = *static_cast<const Slot453Records*>(rows[i].obj);
            check_atexit(creg, at, kRecordRegistryDtorRva, &cell.registry);
            check_atexit(creg, at + 1, dtor, rows[i].obj);
            ++reg_shift;
            CHECK(cell.guard != 0);
            for (RecordPtrVec* vec : {&cell.registry.first, &cell.registry.second}) {
                CHECK(static_cast<std::size_t>(vec->end - vec->begin) == kSlot453RecordCount && vec->cap >= vec->end);
                for (std::size_t n = 0; n < kSlot453RecordCount; ++n) CHECK(vec->begin[n] == &rs.rec[n]);   // constructor order, in both vectors
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
        // Cells-only page: no variable registration exists; the rows are name-hash stores (no manager contract is read).
        ConCmdRegistry creg;
        Slot453Objects o;
    RecordRegistryCell cell{};   // the process registry cell: zero = the original's BSS state
    o.record_registry = &cell;
    o.record_alloc = default_slot453_record_alloc();
        const Slot453Result r = init_slot453_48ac10(o, creg);
        CHECK(r.ok && creg.pending.size() == 0);
    }
    {
        // No registry cell: the records row is refused visibly; every row before it was built.
        ConCmdRegistry creg;
        Slot453Objects o;
        poison(o);
        o.record_registry = nullptr;
        o.record_alloc = default_slot453_record_alloc();
        const Slot453Result r = init_slot453_48ac10(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
    }
    {
        // No allocator: the first record's vector growth is refused (the original would fault), nothing is recorded for the records.
        ConCmdRegistry creg;
        Slot453Objects o;
        poison(o);
        RecordRegistryCell cell{};
        o.record_registry = &cell;
        o.record_alloc = AllocDispatchContract{};
        const Slot453Result r = init_slot453_48ac10(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(cell.registry.first.begin == nullptr);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_453");
    return 0;
}
