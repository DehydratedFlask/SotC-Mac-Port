// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 445 (0x4cfe80): 1 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_445_4cfe80.hpp"
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

Row rows_of(Slot445Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'h', &o.r00, 0, -1, 0, -1, -1, nullptr};
    out[1] = Row{'h', &o.r01, 1, -1, 1, -1, -1, nullptr};
    out[2] = Row{'h', &o.r02, 0, -1, 2, -1, -1, nullptr};
    out[3] = Row{'h', &o.r03, 1, -1, 3, -1, -1, nullptr};
    out[4] = Row{'b', &o.r04, 0, -1, -1, -1, -1, nullptr};
    out[5] = Row{'H', &o.r05, -1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot445Rva == 0x4cfe80 && kSlot445Size == 2016 && kSlot445RowCount == 6);

    CHECK(offsetof(Slot445Objects, r00) == 0x0);
    CHECK(offsetof(Slot445Objects, r01) == 0x30);
    CHECK(offsetof(Slot445Objects, r02) == 0x60);
    CHECK(offsetof(Slot445Objects, r03) == 0x90);
    CHECK(offsetof(Slot445Objects, r04) == 0xc0);
    CHECK(offsetof(Slot445Objects, r05) == 0x188);
    CHECK(sizeof(Slot445Objects) == 0x2f0);

    for (std::size_t n = 0; n < kSlot445BoolCount; ++n) CHECK(std::strlen(kSlot445BoolNames[n]) == kSlot445BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot445StringCount; ++n) CHECK(std::strlen(kSlot445MenuStrings[n]) == kSlot445MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot445BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot445BoolFnTag, kSlot445BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot445BoolFnTag, kSlot445BoolValues[row.var], kSlot445MenuStrings[kSlot445BoolDesc[row.var]], kSlot445MenuStringLengths[kSlot445BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot445BoolNames[row.var], kSlot445BoolNameLengths[row.var]));
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot445MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot445MenuStrings[row.s0]));
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
        case 'H': {
            const auto& t = *static_cast<const Slot445HashTable*>(row.obj);
            // Independent evaluation: own loop, expected dwords from the generator's Python port of the hash, into a second poisoned buffer; every byte (written or never written) must match.
            alignas(4) std::uint8_t want[sizeof t.bytes];
            std::memset(want, 0xa5, sizeof want);
            for (std::size_t n = 0; n < kSlot445HashStoreCount; ++n) {
                const std::uint32_t v = kSlot445HashExpected[n];
                std::memcpy(want + kSlot445HashStores[n].offset, &v, 4);
                CHECK(fnv_cstr_a2ddc0(kSlot445HashStringBlob + kSlot445HashStringOffset[kSlot445HashStores[n].string]) == v);   // the native hash agrees with the independent oracle
            }
            CHECK(std::memcmp(want, t.bytes, sizeof want) == 0);
            CHECK(kSlot445HashStores[0].call_rva == 0x4d0146 && kSlot445HashStores[kSlot445HashStoreCount - 1].store_rva == 0x4d063f);
            for (std::size_t n = 0; n < kSlot445HashStringCount; ++n) {
                const char* text = kSlot445HashStringBlob + kSlot445HashStringOffset[n];
                CHECK(std::strlen(text) != 0 && text[0] >= 0x20 && text[0] < 0x7f);
            }
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == (row.var == 0 ? kSlot445HolderVptr : kSlot445HolderVptrB) && h.tail == 0 && same_view(h.name, kSlot445MenuStrings[row.s0], kSlot445MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot445ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot445MenuStrings[row.s0], kSlot445MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot445MenuStrings[row.s1], kSlot445MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot445MenuStrings[row.var], kSlot445MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot445MenuStrings[row.s1], kSlot445MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot445MenuStrings[row.s1], kSlot445MenuStringLengths[row.s1]));
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
    Slot445Objects o;
    poison(o);
    const Slot445Result r = init_slot445_4cfe80(o, creg);
    CHECK(r.ok && r.rows_done == kSlot445RowCount);
    Row rows[kSlot445RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot445RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'P' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G' || kd == 'H') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L' && kd != 'Z' && kd != 'H') ++items;
    }
    CHECK(vars == kSlot445BoolCount + kSlot445IntCount + kSlot445LocalIntCount + kSlot445UIntCount + kSlot445CmdCount + kSlot445StrVarCount + kSlot445FloatCount + kSlot445UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r04.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot445CellCount && creg.atexit_log.size() == kSlot445RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot445RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G' || rows[i].kind == 'H') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : (rows[i].kind == 'N' || rows[i].kind == 'P') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? (rows[i].var == 0 ? kSlot445HolderDtorRva : kSlot445HolderDtorRvaB) : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'Z' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? 0 : rows[i].kind == 'L' ? 0 : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].kind == 'Z' ? nullptr : rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot445Objects o;
        const Slot445Result r = init_slot445_4cfe80(o, creg);
        CHECK(!r.ok && r.rows_done == 4);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_445");
    return 0;
}
