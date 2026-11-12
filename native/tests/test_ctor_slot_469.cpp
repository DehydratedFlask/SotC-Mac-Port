// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 469 (0x394a20): 5 bool and 3 bounded float variables and 10 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_469_394a20.hpp"

namespace {
using namespace l4;

int g_external_0 = 0;   // stands for the object at 0x2ee65e8, which another slot builds

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

Row rows_of(Slot469Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'b', &o.r02, 2, -1, -1, -1, -1, nullptr};
    out[3] = Row{'f', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'f', &o.r04, 1, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 3, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 4, -1, -1, -1, -1, nullptr};
    out[7] = Row{'f', &o.r07, 2, -1, -1, -1, -1, nullptr};
    out[8] = Row{'g', &o.r08, -1, -1, 0, 1, -1, nullptr};
    out[9] = Row{'e', &o.r09, -1, 0, 1, 2, -1, nullptr};
    out[10] = Row{'e', &o.r10, -1, 1, 1, 3, -1, nullptr};
    out[11] = Row{'e', &o.r11, -1, 2, 1, 4, -1, nullptr};
    out[12] = Row{'S', &o.r12, -1, -1, 1, 5, 0, o.external_target_0};
    out[13] = Row{'S', &o.r13, -1, 3, 1, 6, 1, nullptr};
    out[14] = Row{'S', &o.r14, -1, 4, 1, 7, 2, nullptr};
    out[15] = Row{'e', &o.r15, -1, 5, 1, 8, -1, nullptr};
    out[16] = Row{'e', &o.r16, -1, 6, 1, 9, -1, nullptr};
    out[17] = Row{'S', &o.r17, -1, 7, 1, 10, 3, nullptr};
    out[18] = Row{'q', &o.r18, -1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot469Rva == 0x394a20 && kSlot469Size == 3658 && kSlot469RowCount == 19);
    CHECK(is_known_float_arg_functor(kSlot469FloatFnTag));
    CHECK(offsetof(Slot469Objects, r00) == 0x0);
    CHECK(offsetof(Slot469Objects, r01) == 0xc8);
    CHECK(offsetof(Slot469Objects, r02) == 0x190);
    CHECK(offsetof(Slot469Objects, r03) == 0x258);
    CHECK(offsetof(Slot469Objects, r04) == 0x330);
    CHECK(offsetof(Slot469Objects, r05) == 0x408);
    CHECK(offsetof(Slot469Objects, r06) == 0x4d0);
    CHECK(offsetof(Slot469Objects, r07) == 0x598);
    CHECK(offsetof(Slot469Objects, r08) == 0x670);
    CHECK(offsetof(Slot469Objects, r09) == 0x718);
    CHECK(offsetof(Slot469Objects, r10) == 0x7c0);
    CHECK(offsetof(Slot469Objects, r11) == 0x868);
    CHECK(offsetof(Slot469Objects, r12) == 0x910);
    CHECK(offsetof(Slot469Objects, r13) == 0x9b8);
    CHECK(offsetof(Slot469Objects, r14) == 0xa60);
    CHECK(offsetof(Slot469Objects, r15) == 0xb08);
    CHECK(offsetof(Slot469Objects, r16) == 0xbb0);
    CHECK(offsetof(Slot469Objects, r17) == 0xc58);
    CHECK(offsetof(Slot469Objects, r18) == 0xd00);
    CHECK(sizeof(Slot469Objects) == 0xdb0);
    for (std::size_t n = 0; n < kSlot469BoolCount; ++n) CHECK(std::strlen(kSlot469BoolNames[n]) == kSlot469BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot469FloatCount; ++n) CHECK(std::strlen(kSlot469FloatNames[n]) == kSlot469FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot469StringCount; ++n) CHECK(std::strlen(kSlot469MenuStrings[n]) == kSlot469MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot469BoolFnTag, kSlot469BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot469BoolNames[row.var], kSlot469BoolNameLengths[row.var]));
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot469VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot469FloatNames[row.var], kSlot469FloatNameLengths[row.var]));
            CHECK(v.value == kSlot469FloatValueBits[row.var] && v.default_value == kSlot469FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot469FloatLoBits[row.var] && hi == kSlot469FloatHiBits[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot469MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot469MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
           const auto& q = *static_cast<const Slot469Sentinel*>(row.obj);
           const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&q);
           CHECK(q.q00 == base + 0x28 && q.q08 == base + 0x28 && q.q20 == base + 0x28 && q.q10 == base + 0xa8);
           CHECK(q.q18 == 0xa5a5a5a5a5a5a5a5ull);   // never written
           for (std::uint8_t b : q.area) CHECK(b == 0xa5);   // never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot469ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot469MenuStrings[row.s0], kSlot469MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot469MenuStrings[row.s1], kSlot469MenuStringLengths[row.s1]));
                break;
            }
            if (row.target < 0) {
                CHECK(row.kind == 'S' && row.ext != nullptr && it.kind == 2 && it.target == row.ext);   // external target: identity pointer only
                CHECK(same_view(it.text, kSlot469MenuStrings[row.s1], kSlot469MenuStringLengths[row.s1]));
               CHECK(it.range_min == bits_to_float(kSlot469SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot469SliderHiBits[row.slider]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot469MenuStrings[row.s1], kSlot469MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's' || row.kind == 'S') {
                CHECK(it.range_min == bits_to_float(kSlot469SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot469SliderHiBits[row.slider]));
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
    Slot469Objects o;
    poison(o);
    o.external_target_0 = &g_external_0;
    const Slot469Result r = init_slot469_394a20(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot469RowCount);
    Row rows[kSlot469RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot469RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w') ++items;
    }
    CHECK(vars == kSlot469BoolCount + kSlot469IntCount + kSlot469LocalIntCount + kSlot469UIntCount + kSlot469CmdCount + kSlot469StrVarCount + kSlot469FloatCount + kSlot469UFloatCount);
    CHECK(menu.root.count == items && items == 10 && menu.root.link.next == &o.r08.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot469CellCount && creg.atexit_log.size() == kSlot469RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot469RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? kSlot469SentinelDtorRva : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : 0x272d0;

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
        Slot469Objects o;
        const Slot469Result r = init_slot469_394a20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot469Objects o;
        const Slot469Result r = init_slot469_394a20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
    }
    {
        // An external slider target the host did not supply: that row is refused visibly (rows before it were built).
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot469Objects o;
        poison(o);
        o.external_target_0 = nullptr;
        const Slot469Result r = init_slot469_394a20(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 12);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_469");
    return 0;
}
