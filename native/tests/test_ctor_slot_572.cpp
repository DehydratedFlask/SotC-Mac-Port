// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 572 (0x1a20): 0 bool and 0 bounded float variables and 3 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_572_1a20.hpp"

namespace {
using namespace l4;

int g_external_0 = 0;   // stands for the object at 0x2dfcbd0, which another slot builds

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

Row rows_of(Slot572Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'m', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'m', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'m', &o.r02, 2, -1, -1, -1, -1, nullptr};
    out[3] = Row{'m', &o.r03, 3, -1, -1, -1, -1, nullptr};
    out[4] = Row{'m', &o.r04, 4, -1, -1, -1, -1, nullptr};
    out[5] = Row{'e', &o.r05, -1, 2, 4, 5, -1, nullptr};
    out[6] = Row{'e', &o.r06, -1, 4, 4, 6, -1, nullptr};
    out[7] = Row{'x', &o.r07, 8, -1, 4, 7, -1, o.external_target_0};
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
    CHECK(kSlot572Rva == 0x1a20 && kSlot572Size == 1668 && kSlot572RowCount == 8);
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot572Objects, r00) == 0x0);
    CHECK(offsetof(Slot572Objects, r01) == 0xa0);
    CHECK(offsetof(Slot572Objects, r02) == 0x140);
    CHECK(offsetof(Slot572Objects, r03) == 0x1e0);
    CHECK(offsetof(Slot572Objects, r04) == 0x280);
    CHECK(offsetof(Slot572Objects, r05) == 0x320);
    CHECK(offsetof(Slot572Objects, r06) == 0x3c8);
    CHECK(offsetof(Slot572Objects, r07) == 0x470);
    CHECK(sizeof(Slot572Objects) == 0x520);

    for (std::size_t n = 0; n < kSlot572CmdCount; ++n) CHECK(std::strlen(kSlot572CmdNames[n]) == kSlot572CmdNameLengths[n] && kSlot572CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot572StringCount; ++n) CHECK(std::strlen(kSlot572MenuStrings[n]) == kSlot572MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot572CmdNames[row.var], kSlot572MenuStrings[kSlot572CmdDesc[row.var]], kSlot572MenuStringLengths[kSlot572CmdDesc[row.var]], kSlot572CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot572MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot572MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
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
            CHECK(it.flags == kSlot572ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot572MenuStrings[row.s0], kSlot572MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot572MenuStrings[row.s1], kSlot572MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot572MenuStrings[row.var], kSlot572MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot572MenuStrings[row.s1], kSlot572MenuStringLengths[row.s1]));
                break;
            }
            if (row.target < 0) {
                CHECK(row.kind == 'S' && row.ext != nullptr && it.kind == 2 && it.target == row.ext);   // external target: identity pointer only
                CHECK(same_view(it.text, kSlot572MenuStrings[row.s1], kSlot572MenuStringLengths[row.s1]));
               CHECK(false);   // no slider rows in this slot
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot572MenuStrings[row.s1], kSlot572MenuStringLengths[row.s1]));
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
    Slot572Objects o;
    poison(o);
    o.external_target_0 = &g_external_0;
    const Slot572Result r = init_slot572_1a20(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot572RowCount);
    Row rows[kSlot572RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot572RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot572BoolCount + kSlot572IntCount + kSlot572LocalIntCount + kSlot572UIntCount + kSlot572CmdCount + kSlot572StrVarCount + kSlot572FloatCount + kSlot572UFloatCount);
    CHECK(menu.root.count == items && items == 3 && menu.root.link.next == &o.r05.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot572CellCount && creg.atexit_log.size() == kSlot572RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot572RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : 0x272d0;

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
        Slot572Objects o;
        const Slot572Result r = init_slot572_1a20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot572Objects o;
        const Slot572Result r = init_slot572_1a20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 5);
    }
    {
        // An external slider target the host did not supply: that row is refused visibly (rows before it were built).
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot572Objects o;
        poison(o);
        o.external_target_0 = nullptr;
        const Slot572Result r = init_slot572_1a20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 7);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_572");
    return 0;
}
