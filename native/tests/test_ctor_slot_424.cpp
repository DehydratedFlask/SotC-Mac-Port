// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 424 (0x551140): 0 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_424_551140.hpp"
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

Row rows_of(Slot424Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'m', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'m', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'m', &o.r02, 2, -1, -1, -1, -1, nullptr};
    out[3] = Row{'Z', &o.r03, -1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot424Rva == 0x551140 && kSlot424Size == 28370 && kSlot424RowCount == 4);

    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot424Objects, r00) == 0x0);
    CHECK(offsetof(Slot424Objects, r01) == 0xa0);
    CHECK(offsetof(Slot424Objects, r02) == 0x140);
    CHECK(offsetof(Slot424Objects, r03) == 0x1e0);
    CHECK(sizeof(Slot424Objects) == 0x6900);

    for (std::size_t n = 0; n < kSlot424CmdCount; ++n) CHECK(std::strlen(kSlot424CmdNames[n]) == kSlot424CmdNameLengths[n] && kSlot424CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot424StringCount; ++n) CHECK(std::strlen(kSlot424MenuStrings[n]) == kSlot424MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot424CmdNames[row.var], kSlot424MenuStrings[kSlot424CmdDesc[row.var]], kSlot424MenuStringLengths[kSlot424CmdDesc[row.var]], kSlot424CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot424MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot424MenuStrings[row.s0]));
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
        case 'Z': {
            const auto& t = *static_cast<const Slot424StoreTable*>(row.obj);
            // Independent evaluation of the table: an own loop and own lane resolution into a second poisoned buffer; every byte (written or never written) must match.
            alignas(8) std::uint8_t want[sizeof t.bytes];
            std::memset(want, 0xa5, sizeof want);
            std::size_t kinds[4] = {0, 0, 0, 0};
            for (std::size_t n = 0; n < kSlot424StoreCount; ++n) {
                const StoreTableRecord& s = kSlot424Stores[n];
                const std::uint8_t lane_kind[2] = {s.kind0, s.kind1};
                const std::uint64_t lane_val[2] = {s.value0, s.value1};
                for (int ln = 0; ln < (s.width == 16 ? 2 : 1); ++ln) {
                    std::uint64_t v = lane_val[ln];
                    ++kinds[lane_kind[ln]];
                    if (lane_kind[ln] == 1) v = reinterpret_cast<std::uintptr_t>(t.bytes) + v;
                    if (lane_kind[ln] == 2) v = reinterpret_cast<std::uintptr_t>(kSlot424StoreStringBlob) + kSlot424StoreStringOffset[v];
                    std::memcpy(want + s.offset + 8 * ln, &v, s.width == 4 ? 4 : 8);
                }
            }
            CHECK(std::memcmp(want, t.bytes, sizeof want) == 0);
            CHECK(kinds[0] == 1608 && kinds[1] == 135 && kinds[2] == 550 && kinds[3] == 234);   // lane census of the ELF replay
            std::size_t covered = 0;
            for (std::size_t off = 0; off < sizeof want; ++off) {
                std::uint8_t b = t.bytes[off];
                if (b != 0xa5) ++covered;
            }
            CHECK(covered <= 16172);   // a written byte may equal the poison value (never the reverse): never more than the 16172 bytes the stores cover
            CHECK(kSlot424Stores[0].insn_rva == 0x55149a && kSlot424Stores[kSlot424StoreCount - 1].insn_rva == 0x557fdc);
            for (std::size_t n = 0; n < kSlot424StoreStringCount; ++n) {
                const char* text = kSlot424StoreStringBlob + kSlot424StoreStringOffset[n];
                CHECK(std::strlen(text) != 0 && text[0] >= 0x20 && text[0] < 0x7f);   // every pooled string is a printable C string
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
            CHECK(it.flags == kSlot424ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot424MenuStrings[row.s0], kSlot424MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot424MenuStrings[row.s1], kSlot424MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot424MenuStrings[row.var], kSlot424MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot424MenuStrings[row.s1], kSlot424MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot424MenuStrings[row.s1], kSlot424MenuStringLengths[row.s1]));
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
    Slot424Objects o;
    poison(o);
    const Slot424Result r = init_slot424_551140(o, creg);
    CHECK(r.ok && r.rows_done == kSlot424RowCount);
    Row rows[kSlot424RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot424RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'P' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L' && kd != 'Z') ++items;
    }
    CHECK(vars == kSlot424BoolCount + kSlot424IntCount + kSlot424LocalIntCount + kSlot424UIntCount + kSlot424CmdCount + kSlot424StrVarCount + kSlot424FloatCount + kSlot424UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot424CellCount && creg.atexit_log.size() == kSlot424RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot424RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : (rows[i].kind == 'N' || rows[i].kind == 'P') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'Z' ? kSlot424StoreDtorRva : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? 0 : rows[i].kind == 'L' ? 0 : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].kind == 'Z' ? nullptr : rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot424Objects o;
        const Slot424Result r = init_slot424_551140(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_424");
    return 0;
}
