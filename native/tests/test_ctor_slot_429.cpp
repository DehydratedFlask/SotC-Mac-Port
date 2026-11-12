// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 429 (0x52e2b0): 1 bool and 0 bounded float variables and 5 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_429_52e2b0.hpp"
#include "core/helper_f8f630_strncpy.hpp"

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

Row rows_of(Slot429Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'x', &o.r01, 2, -1, 0, 1, -1, o.external_target_0};
    out[2] = Row{'x', &o.r02, 4, -1, 0, 3, -1, o.external_target_0};
    out[3] = Row{'g', &o.r03, -1, -1, 0, 5, -1, nullptr};
    out[4] = Row{'g', &o.r04, -1, -1, 0, 6, -1, nullptr};
    out[5] = Row{'e', &o.r05, -1, 0, 0, 7, -1, nullptr};
    out[6] = Row{'p', &o.r06, -1, -1, -1, -1, -1, nullptr};
    out[7] = Row{'v', &o.r07, 0, -1, -1, -1, -1, nullptr};
    out[8] = Row{'v', &o.r08, 1, -1, -1, -1, -1, nullptr};
    out[9] = Row{'v', &o.r09, 2, -1, -1, -1, -1, nullptr};
    out[10] = Row{'m', &o.r10, 0, -1, -1, -1, -1, nullptr};
    out[11] = Row{'m', &o.r11, 1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot429Rva == 0x52e2b0 && kSlot429Size == 1734 && kSlot429RowCount == 12);

    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot429Objects, r00) == 0x0);
    CHECK(offsetof(Slot429Objects, r01) == 0xc8);
    CHECK(offsetof(Slot429Objects, r02) == 0x170);
    CHECK(offsetof(Slot429Objects, r03) == 0x218);
    CHECK(offsetof(Slot429Objects, r04) == 0x2c0);
    CHECK(offsetof(Slot429Objects, r05) == 0x368);
    CHECK(offsetof(Slot429Objects, r06) == 0x410);
    CHECK(offsetof(Slot429Objects, r07) == 0x420);
    CHECK(offsetof(Slot429Objects, r08) == 0x440);
    CHECK(offsetof(Slot429Objects, r09) == 0x460);
    CHECK(offsetof(Slot429Objects, r10) == 0x4c0);
    CHECK(offsetof(Slot429Objects, r11) == 0x560);
    CHECK(sizeof(Slot429Objects) == 0x608);

    for (std::size_t n = 0; n < kSlot429BoolCount; ++n) CHECK(std::strlen(kSlot429BoolNames[n]) == kSlot429BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot429CmdCount; ++n) CHECK(std::strlen(kSlot429CmdNames[n]) == kSlot429CmdNameLengths[n] && kSlot429CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot429StringCount; ++n) CHECK(std::strlen(kSlot429MenuStrings[n]) == kSlot429MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot429BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot429BoolFnTag, kSlot429BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot429BoolFnTag, kSlot429BoolValues[row.var], kSlot429MenuStrings[kSlot429BoolDesc[row.var]], kSlot429MenuStringLengths[kSlot429BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot429BoolNames[row.var], kSlot429BoolNameLengths[row.var]));
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot429CmdNames[row.var], kSlot429MenuStrings[kSlot429CmdDesc[row.var]], kSlot429MenuStringLengths[kSlot429CmdDesc[row.var]], kSlot429CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot429MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot429MenuStrings[row.s0]));
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
        case 'v': {
            const auto& z = *static_cast<const Slot429Zero*>(row.obj);
            CHECK(z.q00 == 0xa5a5a5a5a5a5a5a5ull && z.q08 == 0 && z.q10 == 0 && z.q18 == 0);   // begin / end / cap zeroed; +0x00 is never written
            break;
        }
        case 'p': {
            CHECK(*static_cast<const std::uint64_t*>(row.obj) == 0);   // the one store of the original (poison would read 0xa5..)
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot429ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot429MenuStrings[row.s0], kSlot429MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot429MenuStrings[row.s1], kSlot429MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot429MenuStrings[row.var], kSlot429MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot429MenuStrings[row.s1], kSlot429MenuStringLengths[row.s1]));
                break;
            }
            if (row.target < 0) {
                CHECK(row.kind == 'S' && row.ext != nullptr && it.kind == 2 && it.target == row.ext);   // external target: identity pointer only
                CHECK(same_view(it.text, kSlot429MenuStrings[row.s1], kSlot429MenuStringLengths[row.s1]));
               CHECK(false);   // no slider rows in this slot
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot429MenuStrings[row.s1], kSlot429MenuStringLengths[row.s1]));
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
    Slot429Objects o;
    poison(o);
    o.external_target_0 = &g_external_0;
    const Slot429Result r = init_slot429_52e2b0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot429RowCount);
    Row rows[kSlot429RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot429RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'P' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L' && kd != 'Z') ++items;
    }
    CHECK(vars == kSlot429BoolCount + kSlot429IntCount + kSlot429LocalIntCount + kSlot429UIntCount + kSlot429CmdCount + kSlot429StrVarCount + kSlot429FloatCount + kSlot429UFloatCount);
    CHECK(menu.root.count == items && items == 5 && menu.root.link.next == &o.r01.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot429CellCount && creg.atexit_log.size() == kSlot429RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot429RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : (rows[i].kind == 'N' || rows[i].kind == 'P') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? kSlot429ZVecDtorRva[rows[i].var] : rows[i].kind == 'p' ? kSlot429ZeroPtrDtorRva : rows[i].kind == 'Z' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? 0 : rows[i].kind == 'L' ? 0 : 0x272d0;

        if (dtor == 0x272d0 && !root_seen) {
            check_atexit(creg, at, 0x27280, &menu.root);
            root_seen = true;
            check_atexit(creg, at + 1, dtor, rows[i].obj);
        } else {
            check_atexit(creg, at, dtor, rows[i].kind == 'Z' ? nullptr : rows[i].obj);
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
        Slot429Objects o;
        const Slot429Result r = init_slot429_52e2b0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot429Objects o;
        const Slot429Result r = init_slot429_52e2b0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
    }
    {
        // An external slider target the host did not supply: that row is refused visibly (rows before it were built).
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot429Objects o;
        poison(o);
        o.external_target_0 = nullptr;
        const Slot429Result r = init_slot429_52e2b0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_429");
    return 0;
}
